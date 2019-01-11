/*******************************/
/* utils.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <array>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>

#include "dataset.h"
#include "clusters.h"
#include "lsh.h"

template <class T> class cl_management;


/*  Header file for all variant functions and structs used
 *  to complete the LSH algorithm.
 */

template <class T> class cluster;
class cluster_info;

typedef double (*dist_func)(vector_item<double>&, vector_item<double>&);

/* Struct that holds all the parameters given through command line and config */
/* file, for the clustering algorithms                                        */
typedef struct exe_args{
    int all_combinations; // 1: execute all 12 combinations, 0: execute specific
    int metric; // 1: euclidean, 2: cosine
    int k; // number of clusters to be created
    int max_updates; // max number of updates-assigns
    int complete; // for printing
    int L; // number of hash tables in lsh/hc
    int hf; // number of hash functions in lsh/hc
    int hc_probes; // number of probes for lsh/hc
    int hc_M; // number of M for lsh/hc
    std::string input_file; // input file
    std::string output_file; // output file
    std::string config_file; // configuration file


    exe_args();

}exe_args;

/* struct that maps with distance to centroid(LSH/HC) */
typedef struct vector_check{
    vector_item<double>* item;
    double distance;

    vector_check(vector_item<double>* item, double d) : item(item), distance(d){ }
}vector_check;


/* Extract parameters that were given during execution */ 
int get_parameters(int, char**, exe_args&);

/* Given the parameters from command line, open conf file */
/* and check for valid parameters. Returns 0 if ok, else  */
/* -1 for invalid command line parameters or -2 for inv   */
/* configuration file                                     */
int validate_parameters(exe_args&, std::ofstream&);

/* Open config file and read all configuration parameters */
int read_config_file(std::ifstream&, exe_args&);

/*  Print the valid form of given parameters */
void printValidParameters();

/* Print the valid form of configuration file */
void printValidConfig();

/* In case of single combination execution, read algorithms */
/* from user                                                */
int read_combination(int&, int&, int&);

void print_exe_details(exe_args&, int, int, int);
/*  Given a string, it check char-char to see if integer.   */
/*  Is yes, returns 1, else 0.                              */
int isNumber(char*);

/* Returns the number of digits of integer's binary form */
int get_int_len(int);

/* Computes and returns number of combinations (Comb(a,b)) */
int get_combinations(int, int);

/* Computes and returns the factorial of the given number */
int get_factorial(int);

/* Computes and returns the hamming distance of two integer(their binary form) */
int hamming_dist(int, int);

/* Given two arrays(vectors), calculates and returns their inner product */  
template <class T>
float vector_product(std::array<float,D>&, std::array<T, D>&);

/* Given a number of integers, concantetates them and returns the value */
long long int h_concantenate(std::vector<int>&);

/* Returns the modulo of the given integers */
long long int my_mod(int, long int);

/* Extract metrics from string, and return the corresponding number */
int get_metrics(std::string&);

/* Extract radius from query file and return it */
double get_radius(std::string&);

/* Computes the euclidean distance of 2 vectors */
template <class T>
double eucl_distance(vector_item<T>&, vector_item<T>&);

/* Computes the cosine distance of 2 vectors */
template <class T>
double cs_distance(vector_item<T>&, vector_item<T>&);

/* Given a specific vector and it's cluster, finds and returns the index of    */
/* the second best cluster for it using the distance function(metric) provided */
template <class T>
int get_second_best(vector_item<T>&, int, std::vector<cluster<T>*>&, dist_func&);

/* Given a specific vector and a cluster, calculates and returns the average */
/* distance from all the vectors in the cluster, using the metric provided   */
template <class T>
double calculate_b(vector_item<T>&, cluster<T>*, dist_func&);

/* Given a vector of clusters, find and return the starting radius for range     */
/* search in lsh/hc. The radius equals the minimum distance between all centrods */
/* dived by 2 (R = min_dist / 2)                                                 */
double get_starting_r(std::vector<cluster<double>*>&, dist_func&);

/* Range search and assign to centroids from vectors(LSH/HC) */
int add_to_clusters(cluster<double>*, double&, std::vector<vector_check*>&, std::vector<cluster_info*>&, int&);

/* Assing all unassigned vectors to nearest centroid(LSH/HC) */
void final_assign(cl_management<double>&);
/* Asks user if he wants to continue to a new execution and if he wants */
/* to use different files, returning the corresponding choice */
int new_execution(std::ifstream&, std::ifstream&, std::ofstream&);

/* Search given set if contains string(item_id). Returns 1 if exists, else 0 */
int in_set(std::unordered_set<std::string>&, std::string&);

/* Performs exchausting search in the given dataset using the metric provided */
/* Finally returns the minimum distance */
template <class T>
float exchausting_s(dataset<T>&, vector_item<T>&, int);