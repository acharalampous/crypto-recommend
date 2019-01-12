/*******************************/
/* rService.h */
/* Recommendation service */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

#include "tweet.h"
#include "clusters.h"
#include "lsh.h"

/* Implementation of all necessary structs and methods that 
 * will be used in order to implement all the recommendation
 * algorithms and service.
 */

/* r_service */
class r_service{
    private:
        std::vector<user*> users; // all users that tweeted
        std::vector<user*> im_users; // all imaginary users created from clustering tweets
        std::vector<tweet*> tweets; // all the tweets
        std::vector<cryptocurrency*> cryptos; // all cryptocurrencies
        std::unordered_map<std::string, cryptocurrency*> crypto_tags; // all tags of cryptos
        std::unordered_map<std::string, double> lexicon; // all words with their sentiment value
    
        cl_management<double>* cl_manage; // clustering options for creating fictionary users

        /* Problem 1 - COSINE LSH RECOMMENDATION */
        dataset<double>* r_dataset; // real users dataset
        dataset<double>* i_dataset; // imaginary users dataset
        LSH<double>* lsh;

    public:
        /* Con-De Structor */
        r_service();
        ~r_service();

        /* Accessors */
        std::vector<cryptocurrency*>& get_cryptos(); // get cryptos vector
        std::unordered_map<std::string, cryptocurrency*>& get_crypto_tags(); // get crypto tags map
        std::unordered_map<std::string, double>& get_lexicon(); // get all words map


        
        /*  Read and register all cryptocurrencies found in string given.
            Cryptocurrency's name will be tag5(if exists), else tag1
            format: cc1_tag1 cc1_tag2 cc1_tag3 cc1_tag4 cc1_tag5 ...
                    cc2_tag1 cc2_tag2 cc2_tag3 ...
                    cc3_tag1 .... 
                    ...
                    ...
        */
        void register_cryptos(std::ifstream&);

        /*  Read and register all words with the sentiment found in string given.
            format: word1 score
                    word2 score
                    word3 score 
                    ...
                    ...
        */
        void register_words(std::ifstream&);

        /*  Read and register all tweets found in string given.
            format: userId TweetId term1 term2 term 3 ...
                    UserId TweetId term1 term2 .....
                    ...
                    ...
        */
        void register_tweets(std::ifstream&);

        /* Evaluate sentiments for all users registered, according to their tweets */
        void eval_users();

        /*  Using the tweets from part2[Clustering], create k clusters.
            Each cluster from the clustering process, will be an imaginary user
            and each item in cluster, its tweets.
        */
        int create_im_users();

        /* First phase of creating imaginary users! As in 2nd part, tweets are placed */
        /* in cluster, using the parameters found in the configuration file */ 
        int cluster_tweets();

        /* Turn all created clusters into users and assign their tweets to them */
        void clusters_to_users();

        /* Evaluate sentiments for all imaginary users registered, according to their tweets */
        void eval_im_users();
};