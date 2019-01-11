/*******************************/
/* tweet.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <cmath>

#include "tweet.h"
#include "rService.h"
using namespace std;

/*  Implementation of all functions of the cryptocurrencies',
 *  tweets' and users' class methods. Definitions found in tweet.h.
 */



//////////////////////
/** CRYPTOCURRENCY **/
//////////////////////
cryptocurrency::cryptocurrency(string name, int id){
    this->cc_name = name;
    this->id = id;
}

/* Accessors */
string& cryptocurrency::get_name(){
    return this->cc_name;
} 

int cryptocurrency::get_id(){
    return this->id;
}

/* Mutators*/
void cryptocurrency::set_name(string new_name){
    this->cc_name = new_name;
}



/////////////
/** TWEET **/
/////////////
tweet::tweet(int tweet_id, string data){
    this->tweet_id = tweet_id;
    this->totalscore = 0.0;
    this->sentiment = 0.0;
    this->data = data;
    this->cryptos = new unordered_set<int>;

} 

tweet::~tweet(){
    delete this->cryptos;
}

int tweet::get_id(){
    return this->tweet_id;
}

float tweet::get_totalscore(){
    return this->totalscore;
}

float tweet::get_sentiment(){
    return this->sentiment;
}

std::string& tweet::get_data(){
    return this->data;
}

std::unordered_set<int>* tweet::get_cryptos(){
    return this->cryptos;
}

void tweet::eval_sentiment(vector<cryptocurrency*>& cryptos, 
                           unordered_map<string,cryptocurrency*>& crypto_tags,
                           unordered_map<string, float>& lexicon
                          ){
    
    string temp_data = this->data;
    string term; 
    size_t prev = 0, pos;
    
    /* Get all terms and checks if cc reference or words in lexicon */
    while ((pos = temp_data.find_first_of("\t", prev)) != string::npos){
        if (pos > prev){
            /* Get term and check */
            term = temp_data.substr(prev, pos - prev);

            /* Check if term is a cc reference */     
            unordered_map<string, cryptocurrency*>::const_iterator cc_finder = crypto_tags.find(term);
            if (cc_finder != crypto_tags.end()){ // cc reference
                this->cryptos->insert(cc_finder->second->get_id()); // save cc in tweet cryptos
            }
            
            /* Check if term is in lexicon and if yes add sentiment to totalscore */
            unordered_map<string, float>::const_iterator lxc_finder = lexicon.find(term);
            if (lxc_finder != lexicon.end()){ // term exists in lexicon
                this->totalscore += lxc_finder->second;
            }

        }
        prev = pos+1;
    }
    
    if (prev < temp_data.length()){
        term = temp_data.substr(prev, pos - prev);

        /* Check if term is a cc reference */     
        unordered_map<string, cryptocurrency*>::const_iterator cc_finder = crypto_tags.find(term);
        if (cc_finder != crypto_tags.end()){ // cc reference
            this->cryptos->insert(cc_finder->second->get_id()); // save cc in tweet cryptos
        }
        
        /* Check if term is in lexicon and if yes add sentiment to totalscore */
        unordered_map<string, float>::const_iterator lxc_finder = lexicon.find(term);
        if (lxc_finder != lexicon.end()){ // term exists in lexicon
            this->totalscore += lxc_finder->second;
        }
    }

    this->sentiment = totalscore / (sqrt((totalscore * totalscore) + ALPHA));
}



////////////
/** USER **/
////////////
user::user(int id, int type){
    this->user_id = id;
    this->user_type = type;
    this->sentiments = new vector<float>;
    this->tweets = new vector<int>;
    this->cryptos = new unordered_set<int>;
}

user::~user(){
    delete this->sentiments;
    delete this->tweets;
    delete this->cryptos;
}

int user::get_id(){
    return this->user_id;
}

std::vector<float>* user::get_sentiments(){
    return this->sentiments;
}

std::vector<int>* user::get_tweets(){
    return this->tweets;
}

std::unordered_set<int>* user::get_cryptos(){
    return this->cryptos;
}

void user::add_tweet(int t_index){
    this->tweets->push_back(t_index);
}

void user::eval_sentiment(std::vector<tweet*>& tweets, std::vector<cryptocurrency*>& cryptos){
    
    /* Initialize all cryptocurrencies's sentiment as 0.0 */
    for(unsigned int i = 0; i < cryptos.size(); i++){
        this->sentiments->push_back(0.0);
    }

    /* Get sentiments from all evaluated tweets */
    tweet* cur_tweet = NULL;
    for(unsigned int i = 0; i < this->tweets->size(); i++){
        cur_tweet = tweets.at((this->tweets->at(i))); // fetch tweet

        /* Get all cryptos referenced in current tweet */
        unordered_set<int>* tweet_cryptos = cur_tweet->get_cryptos();
        float t_sentiment = cur_tweet->get_sentiment();
        
        /* For each cryptocurrency in tweet, add to equivalent sentiment of user */
        for(unordered_set<int>::iterator it = tweet_cryptos->begin(); it != tweet_cryptos->end(); it++){
            /* Get cryptocurrency index and add to user's cc sentiment */
            int cc_index = *it; 
            this->sentiments->at(cc_index) += t_sentiment;
            this->cryptos->insert(cc_index); // save cc in user referenced cryptos
        }
    }


    // cout << user_type << "." << user_id << endl;
    // for(unsigned int i = 0; i < this->sentiments->size(); i++){
    //     cout << this->sentiments->at(i) << " ";
    // }

    // cout << endl << endl;
}