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

#define __K 4;
#define __L 5;

/* Implementation of all necessary structs and methods that 
 * will be used in order to implement all the recommendation
 * algorithms and service.
 */

/* r_service */
class r_service{
    private:
        int P; // number of nearest neighbours
        std::ofstream out; // output file
        std::vector<user*> users; // all users that tweeted
        std::vector<user*> im_users; // all imaginary users created from clustering tweets
        std::vector<tweet*> tweets; // all the tweets
        std::vector<cryptocurrency*> cryptos; // all cryptocurrencies
        std::unordered_map<std::string, cryptocurrency*> crypto_tags; // all tags of cryptos
        std::unordered_map<std::string, double> lexicon; // all words with their sentiment value
    
        cl_management<double>* cl_manage; // clustering options for creating fictionary users

        /***** Problem 1 - COSINE LSH RECOMMENDATION *****/
        dataset<double>* r_dataset; // real users dataset
        dataset<double>* i_dataset; // imaginary users dataset
        LSH<double>* lsh;

        /***** Problem 2 - CLUSTERING RECOMMENDATION *****/
        cl_management<double>* cl_rec; // cluster recommendation
        std::vector<int> index_map; // maps cluster dataset index <--> user index
        std::unordered_map<int, int> index_map2; // maps user index <--> cluster dataset index
    
        /****** Validation *****/
        std::vector<val_pair*> known_set;
        double total_MAE1; // total mean absolute error
        double total_MAE2; // total mean absolute error
        double total_MAE3; // total mean absolute error
        double total_MAE4; // total mean absolute error
        double tmp_MAE; // total mean absolute error

        int fold_num;
    
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



        /***** Problem 1 *****/

        /* Problem 1 - Using LSH find 5 recommendations for all users */
        void lsh_recommend();

        /* Place all real users' sentiment score in dataset as vector */
        void fill_r_dataset();

        /* Place all imaginary users' sentiment score in dataset as vector */
        void fill_i_dataset();

        /* Initialize lsh with given settings for coins recommendations */
        void init_lsh();
        
        /* Fill lsh with a dataset. If given int equals 1: place real users, 2: imaginary */
        /* Only non-zero vectors are placed in lsh */
        void fill_lsh(int);

        /* Given the number of recomendations and the dataset used in lsh, find for every */
        /* real user recommendations, and print them in the output file provided during execution */ 
        void lsh_find_recs(int, dataset<double>&); // find lsh recommendation

        /* Given a vector of user, neighbours and their dataset, find P nearest and place them sorted in vector */
        void get_P_nearest(vector_item<double>&, std::unordered_set<int>&, dataset<double>&, std::vector<int>&, dist_func&, int);

        /* Given user, it's nn and the dataset that are placed, use cosine similarity to recommend coins to user */
        void calc_similarity(user&, std::vector<int>, std::vector<user*>&, int, sim_func&);


        /***** Problem 2 *****/

        void cluster_recommend();

        int init_cl_rec();
    
        void cl_rec_clustering();

        void cl_find_recs_real();

        void cl_find_recs_im();

        int find_usr_cluster(vector_item<double>& );

        /***** Validation *****/
        /* Gather from all users, all the known coins values. Fill known_set with pairs of usr-coin and value */
        void get_known_coins();

        void validation(int);

        void get_subset_indexes(std::vector<std::pair<int, int>>&);

        void lsh_find_recs(int, dataset<double>&, std::unordered_map<int, std::vector<std::pair<int,double>>>&);

        void calc_similarity(user&, std::vector<int>, std::vector<user*>&, sim_func&, std::vector<std::pair<int,double>>&);

        void reset_coins(int, int);

        void cl_find_recs_real(std::unordered_map<int, std::vector<std::pair<int,double>>>&);
        void cl_find_recs_im(std::unordered_map<int, std::vector<std::pair<int,double>>>&);

};