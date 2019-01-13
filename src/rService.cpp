/*******************************/
/* rService.cpp */
/* Recommendation service */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <string>

#include "rService.h"
#include "lsh.h"

using namespace std;

/*  Implementation of all functions of the classes used for the
 *  recommendation service. Definitions found in r_service.h.
 */

/////////////////
/** r_service **/
/////////////////
r_service::r_service(){
    this->cl_manage = NULL;
    this->lsh = NULL;
    this->r_dataset = NULL;
    this->i_dataset = NULL;
}

r_service::~r_service(){
    /* Destroy all users */
    for(unsigned int i = 0; i < users.size(); i++)
        delete users[i];
    users.clear();

    /* Destroy all imaginary users */
    for(unsigned int i = 0; i < im_users.size(); i++)
        delete im_users[i];
    im_users.clear();
    
    /* Destroy all tweets */
    for(unsigned int i = 0; i < tweets.size(); i++)
        delete tweets[i];
    tweets.clear();

    crypto_tags.clear();
    lexicon.clear();

    /* Destroy all cryptos */
    for(unsigned int i = 0; i < cryptos.size(); i++)
        delete cryptos[i];
    cryptos.clear();


    if(cl_manage != NULL)
        delete cl_manage;

    if(lsh != NULL)
        delete lsh;

    if(r_dataset != NULL)
        delete r_dataset;
    
    if(i_dataset != NULL)
        delete i_dataset;
}

vector<cryptocurrency*>& r_service::get_cryptos(){
    return this->cryptos;
}

unordered_map<string, cryptocurrency*>& r_service::get_crypto_tags(){
    return this->crypto_tags;
}

unordered_map<string, double>& r_service::get_lexicon(){
    return this->lexicon;
}

void r_service::register_cryptos(ifstream& cc){
    string line;
    int i = 0; // id of each cryptocurrency
    vector<cryptocurrency*>& cryptos = this->get_cryptos();
    unordered_map<string, cryptocurrency*>& crypto_tags = this->get_crypto_tags();

    /* Scan whole file */
    while(getline(cc, line)){
        int j = 0; // num of tag
        size_t pos, prev = 0;
        string tag; // cryptocurrency tag

        /* Clear /r if exists at end of line */
        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        /* Get next word(tag) from line */
        while ((pos = line.find_first_of("\t", prev)) != string::npos)
        {
            if (pos > prev){
                tag = line.substr(prev, pos - prev);
                if(j == 0){ // construct new cryptocurrency
                    cryptos.push_back(new cryptocurrency(tag, i));
                }
                else if(j == 4){ // set cryptocurrency's name
                    cryptos[i]->set_name(tag);
                }
                crypto_tags[tag] = cryptos[i]; // map tag's cryptocurrency
            }
            prev = pos + 1;
            j++;
        }
        
        if (prev < line.length()){
            tag = line.substr(prev, string::npos);
            if(j == 0){ // construct new cryptocurrency
                cryptos.push_back(new cryptocurrency(tag, i));
            }
            else if(j == 4){ // set cryptocurrency's name
                cryptos[i]->set_name(tag);
            }
            crypto_tags[tag] = cryptos[i]; // set tags' cryptocurrency
        }
        i++;
    }
}

void r_service::register_words(ifstream& lxc){
    string line;
    
    /* Scan whole lexicon and store every word found */
    while(getline(lxc, line)){
        size_t prev = 0, pos;
        string word;

        /* Clear /r if exists at end of line */
        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
    

        /* Reading word */
        pos = line.find_first_of("\t", prev);
        
        word = line.substr(prev, pos - prev);
        prev = pos + 1;

        /* Getting sentiment of word */
        double val = stod(line.substr(prev, string::npos));

        lexicon[word] = val;
    }
}


void r_service::register_tweets(ifstream& tweets){
    string line;
    string prev_id = ""; // keep to check fo new user
    string user_id; // current tweet's user
    int tweet_id;
    string t_content; // tweet content
    user* cur_user = NULL;

    /* Get number of tweets and make space for them in vector*/
    int total_tweets = 0;
    while(getline(tweets, line))
        total_tweets++;

    this->tweets.resize(total_tweets + 1); // +1 in order to place each tweet to the exact index with its id

    /* Read file again from start */
    tweets.clear();
    tweets.seekg(0, ios::beg);


    int index = 0;
    /* Scan whole file and store every tweet found */
    while(getline(tweets, line)){
        size_t prev = 0, pos;

        /* Clear /r if exists at end of line */
        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        /* Reading user id */
        pos = line.find_first_of("\t", prev);
        
        user_id = line.substr(prev, pos - prev);
        prev = pos + 1;

        /* Checking for new user */
        if(user_id.compare(prev_id) != 0){
            cur_user = new user(stoi(user_id), index++, 1);
            this->users.push_back(cur_user);
            prev_id = user_id;
        }

        /* Reading tweet id */
        pos = line.find_first_of("\t", prev);
        
        tweet_id = stoi(line.substr(prev, pos - prev));
        prev = pos + 1;

        /* Keep tweet only */
        t_content = line.substr(prev, line.size() - prev);

        tweet* cur_tweet = new tweet(tweet_id, t_content);
        this->tweets[tweet_id] = cur_tweet;
        cur_user->add_tweet(tweet_id);

        cur_tweet->eval_sentiment(this->get_cryptos(), this->get_crypto_tags(), this->get_lexicon());
    }
}

void r_service::eval_users(){

    /* Evaluate all users' sentiments about registered cryptocurrencies */
    for(unsigned int i = 0; i < this->users.size(); i++){
        this->users[i]->eval_sentiment(this->tweets, this->cryptos);
    }
}

int r_service::create_im_users(){

    /* Place tweets in clusters */
    int result = cluster_tweets();
    if(result != 0) // error while creating clusters
        return result;

    /* Turn clusters to users */
    clusters_to_users();

    /* Evaluate created users[clusters] generating all cjs */
    eval_im_users();

    return 0;

}

int r_service::cluster_tweets(){
    exe_args parameters;

    /* Read config file for clustering settings */
    int result = validate_parameters(parameters);
    if(result == -1){
        cout << "Error with clustering parameters" << endl;
        return -1;
    }
    else if(result == -3){
        printValidConfig();
        return -2;
    }

    int init = parameters.init;
    int assign = parameters.assign;
    int upd = parameters.upd;
    
    print_exe_details(parameters);
    this->cl_manage = new cl_management<double>(parameters, init, assign, upd);   
    this->cl_manage->clustering(parameters, init, assign, upd);    

    return 0;
}

void r_service::clusters_to_users(){
    int id = 0;

    /* Get All Clusters created */
    vector<cluster<double>*>& clusters = this->cl_manage->get_clusters() ;
    int num_of_clusters = this->cl_manage->get_k();

    /* Turn clusters to users */
    user* cur_user = NULL;
    for(int i = 0; i < num_of_clusters; i++){
        /* Create user */
        cur_user = new user(id, id, 2);
        id++;
        this->im_users.push_back(cur_user);

        /* Get current cluster */
        cluster<double>* cur_cl = clusters[i];
        
        /* If centroid is from dataset must be inserted as tweet */
        if(cur_cl->get_centroid_type() == 1){
            /* Get tweet that is centroid, then its id and insert to user */
            vector_item<double>* centroid = cur_cl->get_centroid();

            int tweet_id = stoi(centroid->get_id());
            cur_user->add_tweet(tweet_id);
        }

        /* Get all tweets from cluster and add to user */
        int num_of_tweets = cur_cl->get_size();
        for(int j = 0; j < num_of_tweets; j++){
            vector_item<double>* cur_tweet = cur_cl->get_vector(j);

            int tweet_id = stoi(cur_tweet->get_id());
            cur_user->add_tweet(tweet_id);
        }
        
    }
}

void r_service::eval_im_users(){

    /* Evaluate all imaginary users' sentiments about registered cryptocurrencies */
    for(unsigned int i = 0; i < this->im_users.size(); i++){
        this->im_users[i]->eval_sentiment(this->tweets, this->cryptos);
    }
}

void r_service::lsh_recommend(){
    fill_r_dataset();


    fill_i_dataset();
    
    init_lsh();
    
    fill_lsh(1);

    lsh_find_recs(5);
}

void r_service::fill_r_dataset(){
    if(this->r_dataset != NULL){
        return;
    }

    this->r_dataset = new dataset<double>();
    for(unsigned int i = 0; i < users.size(); i++){
        user* cur_user = users[i];
        r_dataset->add_vector(*cur_user); 
    }

}

void r_service::fill_i_dataset(){
    if(this->i_dataset != NULL){
        return;
    }

    this->i_dataset = new dataset<double>();

    for(unsigned int i = 0; i < im_users.size(); i++){
        user* cur_user = im_users[i];
        i_dataset->add_vector(*cur_user);
    }
}

void r_service::init_lsh(){
    if(this->lsh != NULL){
        cout << "Error. LSH already created. Abort" << endl;
        return;

    }
    int L = DEFAULT_L;
    int k = DEFAULT_K;
    
    #ifdef __L
        L = __L;
    #endif
    
    #ifdef __K
        k = __K
    #endif

    this->lsh = new LSH<double>(2, L, k, 0);
}

void r_service::fill_lsh(int flag){
    if(this->lsh == NULL){
        cout << "Error. LSH not initialized yet" << endl;
        return;
    }

    if(flag == 1){ // import real users to lsh
        int n = this->r_dataset->get_counter();
        for(int i = 0; i < n; i++){
            if(users[i]->get_zero_vec() != 1) // not a zero vec
                this->lsh->add_vector(r_dataset->get_item(i));
        }
    }
    else{ // import imaginary users
        int n = this->i_dataset->get_counter();
        for(int i = 0; i < n; i++){
            if(im_users[i]->get_zero_vec() != 1) // not a zero vec
                this->lsh->add_vector(i_dataset->get_item(i));
        }
    }
}

void r_service::lsh_find_recs(int cc_nums){
    ofstream out("outPut");
    int num_of_users = users.size();
    for(int i = 0; i < num_of_users; i++){
        user* usr = users[i];
        out << usr->get_id() << " ";

        if(usr->get_zero_vec() == 1){ // zero vec
            out << "Not enough information about user" << endl;
            continue;
        }
        vector_item<double>& query = *(this->r_dataset->get_item(i));
        unordered_set<int> neighs;

        this->lsh->get_neighbours(query, neighs);
        out << endl;

        
    }
}