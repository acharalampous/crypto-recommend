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
    this->total_referenced = 0;
} 

tweet::~tweet(){
}

int tweet::get_id(){
    return this->tweet_id;
}

double tweet::get_totalscore(){
    return this->totalscore;
}

double tweet::get_sentiment(){
    return this->sentiment;
}

std::string& tweet::get_data(){
    return this->data;
}

std::vector<int>& tweet::get_cryptos(){
    return this->cryptos;
}

int tweet::get_total_referenced(){
    return this->total_referenced;
}

void tweet::eval_sentiment(vector<cryptocurrency*>& cryptos, 
                           unordered_map<string,cryptocurrency*>& crypto_tags,
                           unordered_map<string, double>& lexicon
                          ){
    
    string temp_data = this->data;
    string term; 
    size_t prev = 0, pos;

    for(unsigned int i = 0; i < cryptos.size(); i++){
        this->cryptos.push_back(0); // nothing referenced yet
    }
    
    /* Get all terms and checks if cc reference or words in lexicon */
    while ((pos = temp_data.find_first_of("\t", prev)) != string::npos){
        if (pos > prev){
            /* Get term and check */
            term = temp_data.substr(prev, pos - prev);

            /* Check if term is a cc reference */     
            unordered_map<string, cryptocurrency*>::const_iterator cc_finder = crypto_tags.find(term);
            if (cc_finder != crypto_tags.end()){ // cc reference
                this->cryptos[cc_finder->second->get_id()] = 1; // save cc in tweet cryptos
            }
            
            /* Check if term is in lexicon and if yes add sentiment to totalscore */
            unordered_map<string, double>::const_iterator lxc_finder = lexicon.find(term);
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
            this->cryptos[cc_finder->second->get_id()] = 1; // save cc in tweet cryptos
        }
        
        /* Check if term is in lexicon and if yes add sentiment to totalscore */
        unordered_map<string, double>::const_iterator lxc_finder = lexicon.find(term);
        if (lxc_finder != lexicon.end()){ // term exists in lexicon
            this->totalscore += lxc_finder->second;
        }
    }

    this->sentiment = totalscore / (double)(sqrt((totalscore * totalscore) + ALPHA));
    for(unsigned int i = 0; i < this->cryptos.size(); i++){
        this->total_referenced += this->cryptos[i];
    }

}



////////////
/** USER **/
////////////
user::user(int id, int index, int type){
    this->user_id = id;
    this->user_index = index;
    this->user_type = type;
    this->zero_vec = 1;
}

user::~user(){
}

int user::get_id(){
    return this->user_id;
}

int user::get_index(){
    return this->user_index;
}

int user::get_type(){
    return this->user_type;
}

std::vector<double>& user::get_sentiments(){
    return this->sentiments;
}

std::vector<int>& user::get_tweets(){
    return this->tweets;
}

std::vector<int>& user::get_cryptos(){
    return this->cryptos;
}

int user::get_zero_vec(){
    return this->zero_vec;
}

double user::get_avg_sentiment(){
    return this->avg_sentiment;
}

void user::add_tweet(int t_index){
    this->tweets.push_back(t_index);
}

void user::eval_sentiment(std::vector<tweet*>& tweets, std::vector<cryptocurrency*>& cryptos){
    /* Initialize all cryptocurrencies's sentiment as 0.0 */
    for(unsigned int i = 0; i < cryptos.size(); i++){
        this->sentiments.push_back(0.0);
        this->cryptos.push_back(0); // nothing referenced yet
    }

    double total_s = 0; // total sentiment
    int total_referenced = 0; // number of total cryptos that the user was referenced
    
    /* Get sentiments from all evaluated tweets */
    tweet* cur_tweet = NULL;
    for(unsigned int i = 0; i < this->tweets.size(); i++){
        cur_tweet = tweets[this->tweets[i]]; // fetch tweet

        /* Get all cryptos referenced in current tweet */
        vector<int>& tweet_cryptos = cur_tweet->get_cryptos();
        float t_sentiment = cur_tweet->get_sentiment();

        /* For each cryptocurrency in tweet, add to equivalent sentiment of user */
        for(unsigned int i = 0; i < tweet_cryptos.size(); i++){
            /* Check if tweet was referenced to current crypto */
            if(tweet_cryptos[i] == 0)
                continue;

            /* Get cryptocurrency index and add to user's cc sentiment */
            int cc_index = i; 
            this->sentiments[cc_index] += t_sentiment;
            total_s += t_sentiment;

            /* Check if crypto already referenced, else save it */
            if(this->cryptos[cc_index] == 0){
                this->cryptos[cc_index] = 1; // save cc in user referenced cryptos
                total_referenced++;
            }
        }
    }
    
    if(total_referenced != 0){
        if(total_s != 0){
            this->avg_sentiment = total_s / double(total_referenced);
            this->zero_vec = 0;
        }
        else
            this->avg_sentiment = 0;

        for(unsigned int i = 0; i < this->cryptos.size(); i++){
            if(this->cryptos[i] == 0)
                this->sentiments[i] = this->avg_sentiment;
            else{
                if(this->sentiments[i] != 0)
                    this->zero_vec = 0;
            }
        }
    }
    else{
        this->avg_sentiment = 0.0;
    }

    // cout << user_type << "." << user_id << endl;
    // for(unsigned int i = 0; i < this->sentiments.size(); i++){
    //     cout << this->sentiments[i] << " ";
    // }

    // cout << endl << endl;
}

int user::reset_coin(int index){
    if(zero_vec == 1)
        return -1;

    double val = this->sentiments[index];
    int c_rated = 0;

    /* Find how many coins were rated from user */
    for(unsigned int i = 0; i < cryptos.size(); i++){
        if(cryptos[i] == 1)
            c_rated++;
    }

    /* Normalize avg_sentiment */
    double tmp_avg = avg_sentiment * c_rated;
    tmp_avg -= val;

    /* Set coin as unrated */
    this->cryptos[index] = 0; 

    if(c_rated == 1){ // only this coin was rated
        this->avg_sentiment = 0;
        zero_vec = 1;
        return -1;
    }
    else{
        if(tmp_avg == 0.0)
            this->avg_sentiment = 0.0;
        else
            this->avg_sentiment = tmp_avg / (double)(c_rated - 1);
        
        for(unsigned int i = 0; i < cryptos.size(); i++){
            if(cryptos[i] == 0)
                sentiments[i] = avg_sentiment;
        }
        
    }

    /* Check for zero_vec */
    int flag = 0;
    for(unsigned int i = 0; i < cryptos.size(); i++){
        if(sentiments[i] != 0.0){
            flag = 1;
            break;
        }
    }

    if(flag == 0){
        zero_vec = 1;
        return -1;
    }

    return 0;
}

void user::re_reset_coin(int index, double val){
    int c_rated = 0;

    /* Find number of coins that the user was referenced */
    for(unsigned int i = 0; i < cryptos.size(); i++){
        if(cryptos[i] == 1)
            c_rated++;
    }

    /* Normalize avg_sentiment */
    double tmp_avg = avg_sentiment * c_rated;
    tmp_avg += val;
    this->avg_sentiment = tmp_avg / (double)(c_rated + 1);

    /* Reset coin's sentiment */
    this->cryptos[index] = 1; 
    this->sentiments[index] = val;


    /* Set unrated coins as avg */
    for(unsigned int i = 0; i < cryptos.size(); i++){
        if(cryptos[i] == 0)
            sentiments[i] = avg_sentiment;
    }


    /* Check for zero_vec */
    int flag = 0;
    for(unsigned int i = 0; i < cryptos.size(); i++){
        if(sentiments[i] != 0.0){
            flag = 1;
            zero_vec = 0;
            break;
        }
    }

    if(flag == 0){
        zero_vec = 1;
    }

    return;
}