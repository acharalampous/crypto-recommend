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

using namespace std;

/*  Implementation of all functions of the classes used for the
 *  recommendation service. Definitions found in r_service.h.
 */

/////////////////
/** r_service **/
/////////////////
r_service::r_service(){
    this->users = new vector<user*>;
    this->tweets = new vector<tweet*>;
    this->cryptos = new vector<cryptocurrency*>;
    this->crypto_tags = new unordered_map<string, cryptocurrency*>;
    this->lexicon = new unordered_map<string, float>;    
}

r_service::~r_service(){
    /* Destroy all users */
    for(unsigned int i = 0; i < users->size(); i++)
        delete users->at(i);
    delete this->users;
    
    /* Destroy all tweets */
    for(unsigned int i = 0; i < tweets->size(); i++)
        delete tweets->at(i);
    delete this->tweets;

    delete this->crypto_tags;
    delete this->lexicon;

    /* Destroy all cryptos */
    for(unsigned int i = 0; i < cryptos->size(); i++)
        delete cryptos->at(i);
    delete this->cryptos;
}

vector<cryptocurrency*>* r_service::get_cryptos(){
    return this->cryptos;
}

unordered_map<string, cryptocurrency*>* r_service::get_crypto_tags(){
    return this->crypto_tags;
}

unordered_map<string, float>* r_service::get_lexicon(){
    return this->lexicon;
}

void r_service::register_cryptos(ifstream& cc){
    string line;
    int i = 0; // id of each cryptocurrency
    vector<cryptocurrency*>* cryptos = this->get_cryptos();
    unordered_map<string, cryptocurrency*>* crypto_tags = this->get_crypto_tags();

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
                    cryptos->push_back(new cryptocurrency(tag, i));
                }
                else if(j == 4){ // set cryptocurrency's name
                    (*cryptos)[i]->set_name(tag);
                }
                (*crypto_tags)[tag] = (*cryptos)[i]; // map tag's cryptocurrency
            }
            prev = pos + 1;
            j++;
        }
        
        if (prev < line.length()){
            tag = line.substr(prev, string::npos);
            if(j == 0){ // construct new cryptocurrency
                cryptos->push_back(new cryptocurrency(tag, i));
            }
            else if(j == 4){ // set cryptocurrency's name
                (*cryptos)[i]->set_name(tag);
            }
            (*crypto_tags)[tag] = (*cryptos)[i]; // set tags' cryptocurrency
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
        float val = stof(line.substr(prev, string::npos));

        (*lexicon)[word] = val;
    }
}


void r_service::register_tweets(ifstream& tweets){
    string line;
    string prev_id = ""; // keep to check fo new user
    string user_id; // current tweet's user
    int tweet_id;
    string t_content; // tweet content
    user* cur_user = NULL;


    unsigned int t_index = 0; // tweet index in container
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
            cur_user = new user(stoi(user_id));
            this->users->push_back(cur_user);
            prev_id = user_id;
        }

        /* Reading tweet id */
        pos = line.find_first_of("\t,", prev);
        
        tweet_id = stoi(line.substr(prev, pos - prev));
        prev = pos + 1;

        /* Keep tweet only */
        t_content = line.substr(prev, line.size() - prev);

        tweet* cur_tweet = new tweet(tweet_id, t_index, t_content);
        this->tweets->push_back(cur_tweet);
        cur_user->add_tweet(t_index);

        cur_tweet->eval_sentiment(*(this->get_cryptos()), *(this->get_crypto_tags()), *(this->get_lexicon()));

        t_index++;
    }
}

void r_service::eval_users(){

    /* Evaluate all users' sentiments about registered cryptocurrencies */
    for(vector<user*>::iterator usr = this->users->begin(); usr != this->users->end(); usr++){
        (*usr)->eval_sentiment(*(this->tweets), *(this->cryptos));
    }
}