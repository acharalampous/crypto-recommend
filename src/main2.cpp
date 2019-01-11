/*******************************/
/* main.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

#include "dataset.h"
#include "clusters.h"
#include "cl_algorithms.h"

using namespace std;

int main(int argc, char* argv[]){
    exe_args parameters;

    /* Get parameters provided from command line */
    int result = get_parameters(argc, argv, parameters);
    if(result == -2){
        printValidParameters();
        return -1;
    }

    /* Check if valid parameters and read config file for settings */
    ofstream output;
    result = validate_parameters(parameters, output);
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

    int choice = 1; // 0: execute 1 combination, 1: execute all combinations

    /* If -1c was provided, ask user for algorithms */
    if(parameters.all_combinations == 0){
        choice = read_combination(init, assign, upd);
    }

    /* Execute only one combination */
    if(choice == 0){
        print_exe_details(parameters, init, assign, upd);
        cl_management<double>* cl_manage = new cl_management<double>(parameters, init, assign, upd);   
        cl_manage->clustering(parameters, output, init, assign, upd);    
        delete cl_manage;
    } // All Combinations
    else if(choice == 1){
        cout << ".........................." << endl;
        cout << "Executing all combinations" << endl;
        cout << ".........................." << endl;
        for(int init = 1; init <= 2; init++){
            for(int assign = 1; assign <= 3; assign++){
                for(int upd = 1; upd <= 2; upd++){
                    print_exe_details(parameters, init, assign, upd); // Print all parameters provided
                    cl_management<double>* cl_manage = new cl_management<double>(parameters, init, assign, upd); // INIT CL_MANAGE   
                    cl_manage->clustering(parameters, output, init, assign, upd); // CLUSTERING WORK
                    delete cl_manage; // DESTORY cl_manage
                }
            }
        }
    } // Exit chosen
    else if(choice == 2){
        cout << "Choosed to exit! Abort." << endl;
        return 1;
    }


    cout << "Execution finished succesfully!" << endl;
    cout << "Check " << parameters.output_file << " for results." << endl;
    
    return 0;
}
