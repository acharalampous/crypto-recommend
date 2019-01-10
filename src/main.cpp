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

    unordered_map<string, float>* lex = recsys->get_lexicon();

    cout << (*lex)["good"] << endl;
    cout << "h" << endl;
}