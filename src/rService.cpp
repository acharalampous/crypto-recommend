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
#include <map>
#include <random>
#include <ctime>

#include "rService.h"
#include "lsh.h"

using namespace std;

/*  Implementation of all functions of the classes used for the
 *  recommendation service. Definitions found in r_service.h.
 */

/////////////////
/** r_service **/
/////////////////
r_service::r_service(string& f_in, string& f_lex, string& f_cn ,string& f_out, int& val){
    this->P = DEFAULT_P;
    this->validate = val;
    this->input.open(f_in);
    this->l_file.open(f_lex);
    this->c_file.open(f_cn);
    this->out.open(f_out);

    this->cl_manage = NULL;
    this->lsh = NULL;
    this->r_dataset = NULL;
    this->i_dataset = NULL;
    this->cl_rec = NULL;
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

    if(cl_rec != NULL)
        delete cl_rec;

    for(unsigned int i = 0; i < known_set.size(); i++)
        delete known_set[i];
    known_set.clear();
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

int r_service::get_validate(){
    return this->validate;
}

void r_service::register_cryptos(){
    string line;
    int i = 0; // id of each cryptocurrency
    vector<cryptocurrency*>& cryptos = this->get_cryptos();
    unordered_map<string, cryptocurrency*>& crypto_tags = this->get_crypto_tags();

    /* Scan whole file */
    while(getline(c_file, line)){
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

void r_service::register_words(){
    string line;
    
    /* Scan whole lexicon and store every word found */
    while(getline(l_file, line)){
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


void r_service::register_tweets(){
    string line;
    string prev_id = ""; // keep to check fo new user
    string user_id; // current tweet's user
    int tweet_id;
    string t_content; // tweet content
    user* cur_user = NULL;
    int flag = -1; // 

    int total_tweets = 0;
    /* Scan first line for P parameter, if not given use default */
    getline(input, line);
    try{
        /* Clear /r if exists at end of line */
        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if(line.compare(0, 2, "P:") == 0){ // P parameter was provided
            this->P = stoi(line.substr(2, line.length() - 2));
            flag = 1;
        }
        else{
            total_tweets++;
        }
    }catch(invalid_argument& e1){}

    /* Get number of tweets and make space for them in vector*/
    while(getline(input, line))
        total_tweets++;

    this->tweets.resize(total_tweets + 1); // +1 in order to place each tweet to the exact index with its id

    /* Read file again from start */
    input.clear();
    input.seekg(0, ios::beg);

    if(flag == 1)
        getline(input, line); // skip P definition

    int index = 0;
    /* Scan whole file and store every tweet found */
    while(getline(input, line)){
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
    exe_args parameters(1);

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

    this->out << "1. LSH-COSINE Recommendation - Problem A" << endl;
    this->out << "----------------------------------------" << endl;

    /* Place real users in dataset to be used in lsh */
    fill_r_dataset();
    
    /* Initialize lsh for placing real users in it */
    init_lsh();
    
    /* Fill lsh with real users */
    fill_lsh(1);

    /* Find 5 recomendations for all real users */ 
    lsh_find_recs(5, *(this->r_dataset));

    delete this->lsh;
    this->lsh = NULL;
    
////////////////////////////////////////////////////////////
    this->out << endl << endl << "==========================================" << endl << endl; 
    this->out << "1. LSH-COSINE Recommendation - Problem B" << endl;
    this->out << "----------------------------------------" << endl;

    /* Place imaginary users in dataset to be used in lsh */
    fill_i_dataset();

    /*Initialize lsh for placing imaginary users in it */
    init_lsh();

    /* Fill lsh with imaginary users */
    fill_lsh(2);

    /* Find 2 recommendations for all real users */
    lsh_find_recs(2, *(this->i_dataset));

    this->out << endl << endl << "==========================================" << endl << endl; 
    this->out << endl << endl << "==========================================" << endl << endl; 

}

void r_service::fill_r_dataset(){
    /* Check if r_dataset is not yet filled */
    if(this->r_dataset != NULL){
        return;
    }

    /* Place every real user's sentiment's vector in dataset */
    this->r_dataset = new dataset<double>();
    for(unsigned int i = 0; i < users.size(); i++){
        user* cur_user = users[i];
        r_dataset->add_vector(*cur_user); 
    }

}

void r_service::fill_i_dataset(){
    /* Check if i_dataset is not yet filled */
    if(this->i_dataset != NULL){
        return;
    }

    /* Place every imaginary user's sentiment's vector in dataset */
    this->i_dataset = new dataset<double>();
    for(unsigned int i = 0; i < im_users.size(); i++){
        user* cur_user = im_users[i];
        i_dataset->add_vector(*cur_user);
    }
}

void r_service::init_lsh(){
    /* if not define use default L and k for lsh */
    int L = DEFAULT_L; 
    int k = DEFAULT_K;

    /* Check if lsh already created */
    if(this->lsh != NULL){
        cout << "Error. LSH already created. Abort" << endl;
        return;
    }
    
    /* Use L if defined */
    #ifdef __L
        L = __L;
    #endif
    
    /* Use k if defined */
    #ifdef __K
        k = __K
    #endif

    /* Create lsh using cosine metric and given L/k */
    this->lsh = new LSH<double>(2, L, k, 0);
}

void r_service::fill_lsh(int flag){
    /* Check if lsh is initialized */
    if(this->lsh == NULL){
        cout << "Error. LSH not initialized yet" << endl;
        return;
    }

    /* REAL USERS */
    if(flag == 1){ // import real users to lsh
        int n = this->r_dataset->get_counter();
        for(int i = 0; i < n; i++){
            if(users[i]->get_zero_vec() != 1) // not a zero vec
                this->lsh->add_vector(r_dataset->get_item(i));
        }
    }

    /* IMAGINARY USERS */
    else{ // import imaginary users
        int n = this->i_dataset->get_counter();
        for(int i = 0; i < n; i++){
            if(im_users[i]->get_zero_vec() != 1) // not a zero vec
                this->lsh->add_vector(i_dataset->get_item(i));
        }
    }
}

void r_service::lsh_find_recs(int cc_nums, dataset<double>& data){
    
    /* Find recommendations for all users */
    int num_of_users = users.size();
    for(int i = 0; i < num_of_users; i++){
        user* usr = users[i]; // get current user
        this->out << usr->get_id() << " "; 

        /* Check if zero vector, so no recommendations */
        if(usr->get_zero_vec() == 1){ // zero vec
            this->out << "Not enough information about user" << endl;
            continue;
        }

        /* Turn user to query in order to insert as query in lsh */
        vector_item<double>& query = *(this->r_dataset->get_item(i));
        unordered_set<int> neighs; // all neighbours will be kept here

        /* Get all neighbours in all tables */
        this->lsh->get_neighbours(query, neighs);
        
        dist_func dist = &cs_distance;
        /* Keep P nearest neighbours */
        vector<int> p_nearest; // P nearest neighbours will be placed here
        this->get_P_nearest(query, neighs, data, p_nearest, dist, 1);
        
        sim_func sim = &cs_similarity;

        /* Having the p_nearest neighbours, find recommendations and print */
        if(cc_nums == 5)
            this->calc_similarity(*usr, p_nearest, this->users, cc_nums, sim);
        else
            this->calc_similarity(*usr, p_nearest, this->im_users, cc_nums, sim);


        this->out << endl;     
    }
}

void r_service::get_P_nearest(vector_item<double>& query, unordered_set<int>& neighs,
                              dataset<double>& data, vector<int>& p_nearest, dist_func& dist, int flag){
    multimap<double, int> distances; // will keep sorted all distances with neighbours
    pair<double, int> x; // pair to be inserted in multimap each time

    /* Get distances from all neighbours */
    for(unordered_set<int>::iterator it = neighs.begin(); it != neighs.end(); it++){
        /* Ger current neighbour */
        int n_index = *it;
        vector_item<double>& cur_neigh = *(data.get_item(n_index));

        /* Calculate distance with neighbour */
        double cur_dist = dist(query, cur_neigh);

        /* Add to sorted map */
        x.first = cur_dist;
        x.second = n_index;
        distances.insert(x);
    }

    if(flag == 1){
        /* Keep P nearest neighbours */
        int p = 0; // number of neighbours kept so far
        for(multimap<double, int>::iterator it = distances.begin(); it != distances.end(); it++){
            p_nearest.push_back(it->second);
            p++;
            if(p == this->P)
                break;
        }
    }
    else{
        /* Keep P nearest neighbours */
        int p = 0; // number of neighbours kept so far
        for(multimap<double, int>::iterator it = distances.begin(); it != distances.end(); it++){
            p_nearest.push_back(this->index_map[it->second]);
            p++;
            if(p == this->P)
                break;
        }
    }
}


void r_service::calc_similarity(user& query, vector<int> neighbours, vector<user*>& data, int cc_nums, sim_func& sim){
    double q_avg = query.get_avg_sentiment(); // avg rating of user
    vector<double>& q_sentiments = query.get_sentiments(); // current ratings of user
    vector<int>& q_cryptos = query.get_cryptos(); // cryptos that user has referenced
    vector<double> z; // sum of absolute of similarities
    vector<double> sums_sim; // sum of similarites * rating
    vector<int> q_rated; // 0: coin not rated, 1: rated

    /* Initialize "rating" for cryptos */
    for(unsigned int i = 0; i < this->cryptos.size(); i++){
        z.push_back(0.0);
        sums_sim.push_back(0.0);
        q_rated.push_back(q_cryptos[i]);
    }

    /* Calculate similarity with all nn */
    for(unsigned int i = 0; i < neighbours.size(); i++){
        int n_index = neighbours[i];
        user& cur_neighbour = *(data[n_index]); // get neighbour

        /* Get similarity between query and neighbour */
        double similarity = sim(query, cur_neighbour);

        /* Get sentiments and avg rating of current neighbour */ 
        vector<double>& n_sentiments = cur_neighbour.get_sentiments();
        double n_avg = cur_neighbour.get_avg_sentiment(); // avg rating of neighbour

        /* Get current rating for all unrated cryptos of user */
        for(unsigned int j = 0; j < q_sentiments.size(); j++){
            if(q_cryptos[j] == 1) // crypto already rated
                continue;

            /* Keep abs similarity for coin */
            z[j] += abs(similarity);

            /* Get similarity * (coin rating - avg rating) */
            sums_sim[j] += similarity * (n_sentiments[j] - n_avg);
        }
    }

    multimap<double, int> coins_ratings; // sorted coins per rating
    pair<double, int> x;

    /* Calculate "rating" and sort all coins */
    for(unsigned int i = 0; i < cryptos.size(); i++){
        if(q_cryptos[i] == 1) // crypto already rated
            continue;

        sums_sim[i] = q_avg + ((double(1) / z[i]) * sums_sim[i]);
        x.first = sums_sim[i];
        x.second = i;

        coins_ratings.insert(x);
    }

    /* Recommend top coins */ 
    int num = 0;
    multimap<double, int>::iterator it;
    for(it = coins_ratings.begin(); it != coins_ratings.end(); it++){
        this->out << "\"" << this->cryptos[it->second]->get_name() << "\" ";
        num++;

        q_rated[it->second] = 1; // mark as already recommended
        /* Check if recommended enough coins */
        if(num == cc_nums)
            break;
    }

    /* If not enough coins recommended, recommend some more */
    if(num != cc_nums){
        for(unsigned int i = 0; i < q_sentiments.size(); i++){
            if(q_rated[i] == 0){ // not recommended yet
                this->out << "\"" << this->cryptos[i]->get_name() << "\" ";
                num++;

                q_rated[i] = 1; // mark as already recommended
                /* Check if recommended enough coins */
                if(num == cc_nums)
                    break;
            } 
        }
    }

}

void r_service::cluster_recommend(){
    
    init_cl_rec();

    this->out << "2. Clustering Recommendation - Problem A" << endl;
    this->out << "----------------------------------------" << endl;

    /* Place real users in dataset to be used in clustering */
    this->cl_rec->fill_dataset(this->users, this->index_map, this->index_map2);
    
    /* Cluster real users */
    cl_rec_clustering();

    /* Find 5 recomendations for all real users */ 
    cl_find_recs_real();

    delete this->cl_rec;
    this->cl_rec = NULL;
    this->index_map.clear();
    this->index_map2.clear();
    // /////////////////////////////////////////////////
    
    this->out << endl << endl << "==========================================" << endl << endl; 

    this->out << "2. Clustering Recommendation- Problem B" << endl;
    this->out << "---------------------------------------" << endl;

    init_cl_rec();

    /* Place real users in dataset to be used in clustering */
    this->cl_rec->fill_dataset(this->im_users, this->index_map, this->index_map2);
    
    /* Cluster real users */
    cl_rec_clustering();

    /* Find 5 recomendations for all real users */ 
    cl_find_recs_im();
}

int r_service::init_cl_rec(){
    exe_args parameters(2);

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
    
    //print_exe_details(parameters);
    this->cl_rec = new cl_management<double>(parameters, init, assign, upd);   

    return 0;
}

void r_service::cl_rec_clustering(){
    int i = 0;
    int max_updates = this->cl_rec->get_max_updates();
    
    this->cl_rec->init_clusters();

    this->cl_rec->assign_clusters();

    int made_changes = 1; 
    while(1){
        if(made_changes == 0 || i >= max_updates)
            break;

        made_changes = this->cl_rec->update_clusters();
        this->cl_rec->assign_clusters();
        i++;
    }
}

void r_service::cl_find_recs_real(){
    int num_of_users = this->users.size();
    vector<cluster_info*>& vectors_info = this->cl_rec->get_vectors_info();
    dataset<double>& data = *(this->cl_rec->get_dataset());

    /* Recommend to all users */
    for(int i = 0; i < num_of_users; i++){
        user& usr = *(this->users[i]); // get user
        this->out << usr.get_id() << " ";

        /* Check if zero vector, so no recommendations */
        if(usr.get_zero_vec() == 1){ // zero vec
            this->out << "Not enough information about user" << endl;
            continue;
        }

        /* Get index of user in cluster dataset */
        int data_index = this->index_map2[i];

        /* Get number of vector that user belongs to */
        int cluster_num = vectors_info[data_index]->get_cluster_num();

        vector_item<double>& query = *(data.get_item(data_index));
        unordered_set<int> neighs; // all neighbours will be kept here

        /* Get all neighbours in all tables */
        this->cl_rec->get_neighbours(query, cluster_num, neighs);

        dist_func dist = &eucl_distance;
        /* Keep P nearest neighbours */
        vector<int> p_nearest; // P nearest neighbours will be placed here
        this->get_P_nearest(query, neighs, data, p_nearest, dist, 2);
        
        /* Having the p_nearest neighbours, find recommendations and print */
        sim_func sim = &eucl_similarity;
        this->calc_similarity(usr, p_nearest, this->users, 5, sim);

        this->out << endl;    
    }

}


void r_service::cl_find_recs_im(){
    int num_of_users = this->users.size();
    dataset<double>& data = *(this->cl_rec->get_dataset());

    /* Recommend to all users */
    for(int i = 0; i < num_of_users; i++){
        user& usr = *(this->users[i]); // get user
        this->out << usr.get_id() << " ";

        /* Check if zero vector, so no recommendations */
        if(usr.get_zero_vec() == 1){ // zero vec
            this->out << "Not enough information about user" << endl;
            continue;
        }

        vector_item<double> query(usr, -1);
        unordered_set<int> neighs; // all neighbours will be kept here

        int cluster_num = find_usr_cluster(query);

        /* Get all neighbours in all tables */
        this->cl_rec->get_neighbours(query, cluster_num, neighs);

        dist_func dist = &eucl_distance;
        /* Keep P nearest neighbours */
        vector<int> p_nearest; // P nearest neighbours will be placed here
        this->get_P_nearest(query, neighs, data, p_nearest, dist, 2);
        
        /* Having the p_nearest neighbours, find recommendations and print */
        sim_func sim = &eucl_similarity;
        this->calc_similarity(usr, p_nearest, this->im_users, 5, sim);

        this->out << endl;    
    }

}

int r_service::find_usr_cluster(vector_item<double>& query){
    int num = this->cl_rec->nearest_cl(query);

    return num;
}

void r_service::get_known_coins(){
    int num_of_users = this->users.size();

    /* Check all users */
    for(int i = 0; i < num_of_users; i++){
        user& usr = *(this->users[i]);
        if(usr.get_zero_vec() == 1)
            continue;     

        /* Get user's coins and their sentiment */
        vector<int>& usr_coins = usr.get_cryptos();
        vector<double>& usr_sentiments = usr.get_sentiments();

        int num_of_coins = usr_coins.size();

        /* Check all coins and gather know values */
        for(int j = 0; j < num_of_coins; j++){
            if(usr_coins[j] == 1){
                double val = usr_sentiments[j];
                
                int r; // random place in known set
                if(known_set.size() == 0)
                    r = 0; 
                else
                    r = rand() % (known_set.size() + 1); 

                this->known_set.insert(known_set.begin() + r, (new val_pair(i, j, val)));
            }
        }
    }
}


void r_service::validation(int ffold_num){
    /* Maps index of users with index of coins */
    unordered_map<int, vector<pair<int,double>>> v_set;
    unordered_map<int, vector<pair<int,double>>>::iterator finder;

    this->total_MAE1 = 0.0;
    this->total_MAE2 = 0.0;
    this->total_MAE3 = 0.0;
    this->total_MAE4 = 0.0;
    this->tmp_MAE = 0.0;

    this->fold_num = ffold_num;
    
    vector<pair<int, int>> subsets;
    this->get_subset_indexes(subsets);

    for(int i = 0; i < fold_num; i++){
        int s_start = subsets[i].first;
        int s_end = subsets[i].second;
        int s_total = s_end - s_start + 1;

        for(int j = s_start; j < s_end; j++){
            int usr_index = this->known_set[j]->usr_index;
            int coin_index = this->known_set[j]->coin_index;
            double val = this->known_set[j]->val;

            int flag = users[usr_index]->reset_coin(coin_index);
            if(flag == 0){
                finder = v_set.find(usr_index);
                if(finder != v_set.end())
                    finder->second.push_back(pair<int, double>(coin_index, val));
                else
                    v_set[usr_index] = {pair<int, double>(coin_index, val)};
            }
            else{
                int to_del = 0;
                finder = v_set.find(usr_index);
                if(finder == v_set.end()){
                    to_del = 1;
                }
                else{
                    to_del = finder->second.size() + 1;
                    v_set.erase(finder);

                }
                s_total -= to_del; // if user turned to zero_vec, dont calculate it later in avg MAE

            }
        }


            /* Place real users in dataset to be used in lsh */
        fill_r_dataset();

        /* Initialize lsh for placing real users in it */
        init_lsh();

        /* Fill lsh with real users */
        fill_lsh(1);

        /* Find 5 recomendations for all real users */ 
        lsh_find_recs(1, *(this->r_dataset), v_set);

        this->total_MAE1 += this->tmp_MAE / (double)s_total;

        this->tmp_MAE = 0.0;

        delete this->lsh;
        this->lsh = NULL;

        /* Place imaginary users in dataset to be used in lsh */
        fill_i_dataset();

        /*Initialize lsh for placing imaginary users in it */
        init_lsh();

        /* Fill lsh with imaginary users */
        fill_lsh(2);

        /* Find 2 recommendations for all real users */
        lsh_find_recs(2, *(this->i_dataset), v_set);

        this->total_MAE2 += this->tmp_MAE / (double)s_total;

        this->tmp_MAE = 0.0;


        delete this->lsh;
        this->lsh = NULL;

        init_cl_rec();

        /* Place real users in dataset to be used in clustering */
        this->cl_rec->fill_dataset(this->users, this->index_map, this->index_map2);
        
        /* Cluster real users */
        cl_rec_clustering();

        /* Find 5 recomendations for all real users */ 
        cl_find_recs_real(v_set);

        delete this->cl_rec;
        this->cl_rec = NULL;
        this->index_map.clear();
        this->index_map2.clear();

        this->total_MAE3 += this->tmp_MAE / (double)s_total;

        this->tmp_MAE = 0.0;

        init_cl_rec();

        /* Place real users in dataset to be used in clustering */
        this->cl_rec->fill_dataset(this->im_users, this->index_map, this->index_map2);

        /* Cluster real users */
        cl_rec_clustering();

        /* Find 5 recomendations for all real users */ 
        cl_find_recs_im(v_set);

        this->total_MAE4 += this->tmp_MAE / (double)s_total;

        this->tmp_MAE = 0.0;

        delete this->cl_rec;
        this->cl_rec = NULL;
        this->index_map.clear();
        this->index_map2.clear();

        reset_coins(s_start, s_end);
        v_set.clear();


    }
    this->total_MAE1 = this->total_MAE1 / (double)this->fold_num;
    this->total_MAE2 = this->total_MAE2 / (double)this->fold_num;
    this->total_MAE3 = this->total_MAE3 / (double)this->fold_num;
    this->total_MAE4 = this->total_MAE4 / (double)this->fold_num;

    cout << "MAE1 " << this->total_MAE1 << endl;
    cout << "MAE2 " << this->total_MAE2 << endl;
    cout << "MAE3 " << this->total_MAE3 << endl;
    cout << "MAE4 " << this->total_MAE4 << endl;

}


void r_service::get_subset_indexes(vector<pair<int, int>>& subsets){
    int num_of_known = this->known_set.size();

    int pair_per_set = num_of_known / this->fold_num;
    int remain = num_of_known % this->fold_num;


    int s_start = 0;
    int s_end = -1;
    for(int i = 0; i < fold_num; i++){
        s_start = s_end + 1;
        s_end = s_start + pair_per_set - 1;
        if(i < remain)
            s_end++;

        subsets.push_back(pair<int, int>(s_start, s_end));
    }
}

void r_service::lsh_find_recs(int flag, dataset<double>& data, unordered_map<int, vector<pair<int,double>>>& v_set){
    unordered_map<int, vector<pair<int,double>>>::iterator it;

    for(it = v_set.begin(); it != v_set.end(); it++){
        int usr_index = it->first;
        vector<pair<int, double>>& coins_vals = it->second;
        user* usr = users[usr_index]; // get current user 

        /* Check if zero vector, so no recommendations */
        if(usr->get_zero_vec() == 1){ // zero vec
            continue;
        }

        /* Turn user to query in order to insert as query in lsh */
        vector_item<double>& query = *(this->r_dataset->get_item(usr_index));
        unordered_set<int> neighs; // all neighbours will be kept here

        /* Get all neighbours in all tables */
        this->lsh->get_neighbours(query, neighs);
        
        dist_func dist = &cs_distance;
        /* Keep P nearest neighbours */
        vector<int> p_nearest; // P nearest neighbours will be placed here
        this->get_P_nearest(query, neighs, data, p_nearest, dist, 1);
        
        sim_func sim = &cs_similarity;

        /* Having the p_nearest neighbours, find recommendations and print */
        if(flag == 1)
            this->calc_similarity(*usr, p_nearest, this->users, sim, coins_vals);   
        else    
            this->calc_similarity(*usr, p_nearest, this->im_users, sim, coins_vals);   

    }
}


void r_service::calc_similarity(user& query, vector<int> neighbours, vector<user*>& data, sim_func& sim, vector<pair<int,double>>& coins_vals){

    double q_avg = query.get_avg_sentiment(); // avg rating of user
    vector<double> z; // sum of absolute of similarities
    vector<double> sums_sim; // sum of similarites * rating

    /* Initialize "rating" for cryptos */
    for(unsigned int i = 0; i < this->cryptos.size(); i++){
        z.push_back(0.0);
        sums_sim.push_back(0.0);
    }

    /* Calculate similarity with all nn */
    for(unsigned int i = 0; i < neighbours.size(); i++){
        int n_index = neighbours[i];
        user& cur_neighbour = *(data[n_index]); // get neighbour

        /* Get similarity between query and neighbour */
        double similarity = sim(query, cur_neighbour);

        /* Get sentiments and avg rating of current neighbour */ 
        vector<double>& n_sentiments = cur_neighbour.get_sentiments();
        double n_avg = cur_neighbour.get_avg_sentiment(); // avg rating of neighbour

        /* Get current rating for all unrated cryptos of user */
        for(unsigned int j = 0; j < coins_vals.size(); j++){
            int c_index = coins_vals[j].first; // get coin's index
            /* Keep abs similarity for coin */
            z[c_index] += abs(similarity);

            /* Get similarity * (coin rating - avg rating) */
            sums_sim[c_index] += similarity * (n_sentiments[c_index] - n_avg);
        }
    }

    /* Calculate "rating" and sort all coins */
    for(unsigned int i = 0; i < coins_vals.size(); i++){
        int c_index = coins_vals[i].first;

        if(z[c_index] == 0)
            sums_sim[c_index] = q_avg;
        else
            sums_sim[c_index] = q_avg + ((double(1) / z[c_index]) * sums_sim[c_index]); // predicted rating
        
        double rating = coins_vals[i].second;

        this->tmp_MAE += abs(rating - sums_sim[c_index]);

    }

}

void r_service::reset_coins(int s_start, int s_end){
    for(int i = s_start; i < s_end; i++){
        int usr_index = this->known_set[i]->usr_index;
        int coin_index = this->known_set[i]->coin_index;
        double val = this->known_set[i]->val;

        users[usr_index]->re_reset_coin(coin_index, val);
    }
}

void r_service::cl_find_recs_real(unordered_map<int, vector<pair<int,double>>>& v_set){
    unordered_map<int, vector<pair<int,double>>>::iterator it;    

    vector<cluster_info*>& vectors_info = this->cl_rec->get_vectors_info();
    dataset<double>& data = *(this->cl_rec->get_dataset());

    /* Recommend to all users */
    for(it = v_set.begin(); it != v_set.end(); it++){
        int usr_index = it->first;
        vector<pair<int, double>>& coins_vals = it->second;
        user& usr = *(this->users[usr_index]); // get user

        /* Check if zero vector, so no recommendations */
        if(usr.get_zero_vec() == 1){ // zero vec
            continue;
        }

        /* Get index of user in cluster dataset */
        int data_index = this->index_map2[usr_index];

        /* Get number of vector that user belongs to */
        int cluster_num = vectors_info[data_index]->get_cluster_num();

        vector_item<double>& query = *(data.get_item(data_index));
        unordered_set<int> neighs; // all neighbours will be kept here

        /* Get all neighbours in all tables */
        this->cl_rec->get_neighbours(query, cluster_num, neighs);

        dist_func dist = &eucl_distance;
        /* Keep P nearest neighbours */
        vector<int> p_nearest; // P nearest neighbours will be placed here
        this->get_P_nearest(query, neighs, data, p_nearest, dist, 2);
        
        /* Having the p_nearest neighbours, find recommendations and print */
        sim_func sim = &eucl_similarity;
        this->calc_similarity(usr, p_nearest, this->users, sim, coins_vals);
    }

}


void r_service::cl_find_recs_im(unordered_map<int, vector<pair<int,double>>>& v_set){
    unordered_map<int, vector<pair<int,double>>>::iterator it;  

    dataset<double>& data = *(this->cl_rec->get_dataset());

    /* Recommend to all users */
    for(it = v_set.begin(); it != v_set.end(); it++){
        int usr_index = it->first;
        vector<pair<int, double>>& coins_vals = it->second;
        user& usr = *(this->users[usr_index]); // get user

        /* Check if zero vector, so no recommendations */
        if(usr.get_zero_vec() == 1){ // zero vec
            continue;
        }

        vector_item<double> query(usr, -1);
        unordered_set<int> neighs; // all neighbours will be kept here

        int cluster_num = find_usr_cluster(query);

        /* Get all neighbours in all tables */
        this->cl_rec->get_neighbours(query, cluster_num, neighs);

        dist_func dist = &eucl_distance;
        /* Keep P nearest neighbours */
        vector<int> p_nearest; // P nearest neighbours will be placed here
        this->get_P_nearest(query, neighs, data, p_nearest, dist, 2);
        
        /* Having the p_nearest neighbours, find recommendations and print */
        sim_func sim = &eucl_similarity;
        this->calc_similarity(usr, p_nearest, this->im_users, sim, coins_vals);
    }

}