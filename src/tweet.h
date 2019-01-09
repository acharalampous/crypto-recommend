/*******************************/
/* tweet.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <string>
#include <unordered_set>
#include <vector>

/* Implemenention of the classes that will be used to represent cryptocurrencies,
 * tweets and users. 
 */


/** cryptocurrencies **/
class cryptocurrency{
    private:
        std::string cc_name; // name of cryptocurrency
        int id; // index of cryptocurrency
    public:
        /* Con-De Structor */
        cryptocurrency(std::string, int); // given the cryptocurrency index and name construct a new cryptocurrency

        /* Accessors */
        std::string& get_name(); // get cryptocurrency name
        int get_id(); // get index of cc

        /* Mutators */
        void set_name(std::string);

};

/** tweet **/
class tweet{
    private:
        int tweet_id; // id of tweet 
        float totalscore; // total score of tweet before normalization
        float sentiment; // sentiment of tweet after normalization
        std::string data; // text of the tweet
        std::unordered_set<int>* cryptos; // index of each cryptocurrency mentioned in tweet 
    public:
        /* Con-De Structor */
        tweet(int, std::string); // given the tweet id and data construct a new tweet
        ~tweet();

        /* Accessors */
        int get_id();
        float get_totalscore();
        float get_sentiment();
        std::string& get_data();
        std::unordered_set<int>* get_cryptos();
};

/** user **/
class user{
    private:
        int user_id; // id of user
        std::vector<float>* sentiments;
        std::vector<int>* tweets;
        std::unordered_set<int>* cryptos;
    public:
        /* Con-De Structor */
        user(int); // given the user id create a new user
        ~user();

        /* Accessors */
        int get_id();
        std::vector<float>* get_sentiments();
        std::vector<int>* get_tweets();
        std::unordered_set<int>* get_cryptos();
};