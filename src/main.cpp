#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "rService.h"
#include "tweet.h"
#include "utils.h"

using namespace std;

int main(void){
    srand(time(NULL));
    ifstream cc("files/coins_queries.csv");
    ifstream lxc("files/vader_lexicon.csv");
    ifstream tweets("files/tweets_dataset_small.csv");
    
    r_service* recsys = new r_service;

    recsys->register_cryptos(cc);
    recsys->register_words(lxc);
    recsys->register_tweets(tweets);

    recsys->eval_users();

    if(recsys->create_im_users() != 0){
        cout << "Exiting with err" << endl;
        return -1;
    }

    D = 100;

    recsys->get_known_coins();
    recsys->validation(10);
    
    // recsys->lsh_recommend();
    // recsys->cluster_recommend();




    cout << "Deleting Everything" << endl;
    delete recsys;

    
    cout << "Program exiting with exit code 0" << endl;
    return 0;
}