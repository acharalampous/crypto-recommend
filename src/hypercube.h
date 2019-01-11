/*******************************/
/* hypercube.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "metrics.h"

template <class T> class cl_management;
template <class T> class euclidean;
template <class T> class csimilarity;

/*  Implementation of the class that uses the LSH algorithm with hypercube*/

/* Class of hypercube that holds all the metrics */
template <class T>
class hypercube{
    private:
        euclidean<T>* eu_table; // euclidian table
        csimilarity<T>* cs_table; // cosine similarity table
        std::unordered_map<int, int>* fs; // maps each hi value to 1 or 0
        int metric; // 1: euclidean distance, 2: cosine similarity
        int probes; // number of neighbour buckets to check
        int M; // number of total vectors to check
    public:
        /* Given number of metrics, number of input, probes and M create hypercube structure */
        hypercube(int, int, int, int);
        ~hypercube();

        /* Search if int is in map[i]. If not, performs coin toss and the value is added    */
        /* Finally its value is returned(1 or 0)                                            */
        int check_map(int, int);

        /* Add a new vector in the hash table */
        void add_vector(vector_item<T>*);

        /* Assign vectors to clusters using hc range search */
        void assign_clusters(cl_management<T>&);

        /* Finds ANN of given vector, searching in hash table */
        void findANN(vector_item<T>&, float, float&, std::string&, std::ofstream&);

        /* Finds and returns all neighbour bucket numbers found in probes */
        std::vector<int>* find_neighbours(int, int);

        /* Find total size of structure in bytes */
        long int get_total_size();
        
        /* Accessors */
        int get_probes();
        int get_M();
};