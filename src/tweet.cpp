#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "tweet.h"

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



////////////
/** USER **/
////////////
user::user(int id){
    this->user_id = id;
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