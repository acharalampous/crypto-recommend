/*******************************/
/* lsh.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <string>
#include <vector>

#include "utils.h"
#include "metrics.h"
#include "clusters.h"

#define DEFAULT_L 5 // default number of hash tables 

template <class T> class cl_management;
template <class T> class euclidean;
template <class T> class csimilarity;

/*  Implementation of the class that uses the LSH algorithm */

/* Class of LSH that holds all the metrics */
template <class T>
class LSH{
    private:
        int L; // number of hash tables for each metric
        std::vector<euclidean<T>*> eu_tables; // euclidian tables
        std::vector<csimilarity<T>*> cs_tables; // cosine similarity tables 
    public:
        /* Given number of tables, metrics and number of input, create LSH structure */
        LSH(int, int, int, int);
        ~LSH();

        /* Add a new vector in all the hash tables */
        void add_vector(vector_item<T>*);

        /* Assign vectors to clusters using lsh ranges search */
        void assign_clusters(cl_management<T>&);

        /* Finds ANN of given vector, searching in all hash tables */
        void findANN(vector_item<T>&, float, float&, std::string&, std::ofstream&);

        /* Computes and returns the total size of the lsh struct */
        long int get_total_size();
};