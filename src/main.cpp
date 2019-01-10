#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "rService.h"
#include "tweet.h"

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


    cout << "Deleting Everything" << endl;
    delete recsys;

    
    cout << "Program exiting with exit code 0" << endl;
    return 0;
}