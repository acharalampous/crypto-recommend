/*******************************/
/* tweet.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define ALPHA 15
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
        //int tweet_index; // index of tweet in container
        double totalscore; // total score of tweet before normalization
        double sentiment; // sentiment of tweet after normalization
        std::string data; // text of the tweet
        std::vector<int> cryptos; // index of each cryptocurrency mentioned in tweet 
        int total_referenced; // number of total cryptos that the tweet was referenced
    public:
        /* Con-De Structor */
        tweet(int, std::string); // given the tweet id, index and data construct a new tweet
        ~tweet();



        /* Accessors */
        int get_id();
        double get_totalscore();
        double get_sentiment();
        std::string& get_data();
        std::vector<int>& get_cryptos();
        int get_total_referenced();

        /* Evaluate sentiment, totalscore and find all cryptos reference in tweet, using the lexicon */
        void eval_sentiment(std::vector<cryptocurrency*>&, std::unordered_map<std::string, cryptocurrency*>&, 
                            std::unordered_map<std::string, double>&);
};

/** user **/
class user{
    private:
        int user_id; // id of user
        int user_index; // index of user in users' dataset
        int user_type; // 1: real user, 2: imaginary user[from clustering]
        std::vector<double> sentiments; // sentiment for each currency [uj]
        std::vector<int> tweets; // index of each tweet of user
        std::vector<int> cryptos; // index of each crypto, the user referenced is 1, else 0
        int zero_vec; // 1: all sentiments are zero, 0: at least one non-zero sentiment 
        double avg_sentiment; // average sentiment of all cryptos referenced
    public:
        /* Con-De Structor */
        user(int, int, int); // given the user id and index create a new user
        ~user();

        /* Accessors */
        int get_id();
        int get_index();
        int get_type();
        std::vector<double>& get_sentiments();
        std::vector<int>& get_tweets();
        std::vector<int>& get_cryptos();
        int get_zero_vec();
        double get_avg_sentiment();

        void add_tweet(int); // add to user the given tweet's index

        /* Given the registered tweets and coins, evaluate the sentiment for all coins */
        void eval_sentiment(std::vector<tweet*>&, std::vector<cryptocurrency*>&);
};