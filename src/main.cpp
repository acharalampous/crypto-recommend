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

    string f_in = "";
    string f_lex = "";
    string f_cn = "";
    string f_out = "";
    int val = 0;

    int result = get_parameters(argc, argv, f_in, f_lex, f_cn, f_out, val);
    if(result != 0){
        printValidParameters();
        return result;
    }

    result = validate_parameters(f_in, f_lex, f_cn, f_out);
    if(result != 0)
        return result;


    r_service* recsys = new r_service(f_in, f_lex, f_cn, f_out, val);

    recsys->register_cryptos();
    recsys->register_words();
    recsys->register_tweets();

    recsys->eval_users();

    if(recsys->create_im_users() != 0){
        cout << "Exiting with err" << endl;
        return -1;
    }

    D = 100;

    if(recsys->get_validate() == 1){
        recsys->get_known_coins();
        recsys->validation(10);
    }
    else{
        recsys->lsh_recommend();
        recsys->cluster_recommend();

    }

    cout << "Deleting Everything" << endl;
    delete recsys;

    
    cout << "Program exiting with exit code 0" << endl;
    return 0;
}