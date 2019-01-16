#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "rService.h"
#include "tweet.h"
#include "utils.h"

using namespace std;

int main(int argc, char* argv[]){
    srand(time(NULL));

    D = 203; // set dimension for tweets

    /* File names */
    string f_in = "";
    string f_lex = "";
    string f_cn = "";
    string f_out = "";
    int val = 0;

    /* Get executable parameters */
    int result = get_parameters(argc, argv, f_in, f_lex, f_cn, f_out, val);
    if(result != 0){
        printValidParameters();
        return result;
    }

    /* Check given parameters */
    result = validate_parameters(f_in, f_lex, f_cn, f_out);
    if(result != 0)
        return result;


    r_service* recsys = new r_service(f_in, f_lex, f_cn, f_out, val);

    cout << "1. Registering CryptoCurrency tags... ";
    cout.flush();
    recsys->register_cryptos();
    cout << "DONE" << endl;
    
    cout << "2. Registering sentiments lexicon... ";
    cout.flush();
    recsys->register_words();
    cout << "DONE" << endl;
    
    cout << "3. Registering tweets... ";
    cout.flush();
    recsys->register_tweets();
    cout << "DONE" << endl;

    cout << "4. Evaluating users' sentiments... ";
    cout.flush();
    recsys->eval_users();
    cout << "DONE" << endl;
    
    cout << "5. Creating Imaginary users  [";
    cout.flush();
    if(recsys->create_im_users() != 0){
        cout << "Error while creating imaginary users. Abort." << endl;
        delete recsys;
        return -1;
    }
    cout << "] DONE" << endl;

    /* Change dimension for coins sentiments */
    D = 100;

    if(recsys->get_validate() == 1){
        cout << "6. Validating Algorithms  [";
        cout.flush();
        recsys->get_known_coins();
        recsys->validation(10);
        cout << "] DONE" << endl;
    }
    else{
        cout << "6. Recommending Coins  [";
        cout.flush();
        recsys->lsh_recommend();
        recsys->cluster_recommend();
        cout << "] DONE" << endl;
    }

    cout << endl << "Finished Executing. Deleting Everything... ";
    cout.flush();
    delete recsys;
    cout << "DONE" << endl << endl;

    
    cout << "Program exiting with exit code 0" << endl;
    return 0;
}