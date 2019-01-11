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


////////////////////////////////////////////////////////////
    exe_args parameters;

    /* Read config file for clustering settings */
    ofstream output;
    int result = validate_parameters(parameters, output);
    if(result == -1){
        return -1;
    }
    else if(result == -2){
        printValidParameters();
        return -1;
    }
    else if(result == -3){
        printValidConfig();
        return -2;
    }

    /* Algorithms */
    int init;
    int assign;
    int upd;

    int choice = read_combination(init, assign, upd);

    cl_management<double>* cl_manage = NULL;
    if(choice == 0){
        print_exe_details(parameters, init, assign, upd);
        cl_manage = new cl_management<double>(parameters, init, assign, upd);   
        cl_manage->clustering(parameters, init, assign, upd);    
    }
    else if(choice == 2){
        cout << "Choosed to exit! Abort." << endl;
        delete recsys;
        return 1;
    }


////////////////////////////////////////////////////////////

    cout << "Deleting Everything" << endl;
    delete recsys;
    delete cl_manage;

    
    cout << "Program exiting with exit code 0" << endl;
    return 0;
}