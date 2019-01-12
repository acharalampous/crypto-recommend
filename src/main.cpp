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

    cout << "D = " << D << endl;
    #undef D
    #define D 100

    cout << "D = " << D << endl;


    
    recsys->fill_r_dataset();
    recsys->fill_i_dataset();



    cout << "Deleting Everything" << endl;
    delete recsys;

    
    cout << "Program exiting with exit code 0" << endl;
    return 0;
}