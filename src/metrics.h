/*******************************/
/* metrics.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <array>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>

#include "utils.h"
#include "dataset.h"
#include "hypercube.h"

#define DEFAULT_K 4 // default number of hash functions(LSH)
#define HC_DEFAULT_K 3 // default number of hash functions(HC)
#define HC_DEFAULT_M 10 // default points to be checked(HC)
#define HC_DEFAULT_PROBES 2 // default neighbours to check (HC)
#define W 2 // window for euclidean
#define TS_DIVISOR 4 // for tablesize in euclidean

/* For random vector r, in euclidean */
#define MIN_Ri -40 
#define MAX_Ri 40


template <class T> class cluster;
class cluster_info;
class vector_check;

template <class T> class hypercube;


/*  Implementations of all metrics used in LSH  */


/////////////////
/** EUCLIDEAN **/
/////////////////


/** euclidean_vec **/
/* Items(Records) that are hold in buckets. Has pointer to vector */
/* from dataset and the value of g(p).                            */
template <class T>
class euclidean_vec{
    private:
        vector_item<T>* vec; // pointer to vector in dataset
        std::vector<int>* g; // holds all hash function values
    public:
        /* Con-De structor */
        euclidean_vec(vector_item<T>*, std::vector<int>*);
        ~euclidean_vec();

        /* Acessors */
        vector_item<T>& get_vec(); // get vector item
        std::vector<int>& get_g(); // get hvalues
        long int get_size(); // get sizeof euclidean vec in bytes

        /* Debugging */
        void print();
};


/** euclideanHF **/
/* Hash function for euclidean distance metric */
template <class T>
class euclideanHF{
    private:
        std::array<float, D> v; // random vector
        float t; // random displacement
    public:
        euclideanHF();
        
        int getValue(std::array<T ,D>&); // get hash value
        long int get_size(); // get sizeof euclidean hf in bytes

        /* Debugging */
        void print();
};


/** Euclidean metric **/
template <class T>
class euclidean{
	private:
        const int k; // number of hash functions
        long int M; // for locating bucket
        int tableSize; // number of buckets
        std::vector<euclideanHF<T>> hfs; // hash functions
        std::vector<euclidean_vec<T>*>* buckets; // all buckets
		std::vector<int> r; // random vector
	public:
        /* Con-De structor */
		euclidean(int, int);
        ~euclidean();
        
        /* Returns the result of hash functions for a vector(bucket of item) */
        int get_bucket_num(std::vector<int>&);

        /* Returns the value of the given hash function for the provided vector */
        int get_val_hf(std::array<T,D>&, int);

        /* Add new vector to hash table, computing in what bucket to place */ 
        void add_vector(vector_item<T>*);

        /* Add vector directly to specific hash bucket */
        void add_vector(vector_item<T>*, std::vector<int>*, int); 

        /* Returns a pointer to the bucket with the given index */
        std::vector<euclidean_vec<T>*>& get_bucket(int);

        /* Find and gather all candidate vectors using lsh range search */
        int first_assign(cluster<double>*, double&, std::unordered_set<std::string>&, std::vector<vector_check*>&, std::vector<cluster_info*>&, int&);

        /* Find and gather all candidate vectors using hc range search */
        int first_assign(cluster<double>*, double&, hypercube<T>&, std::vector<vector_check*>&, std::vector<cluster_info*>&, int&);

        /* Given a query vector, finds the nearest neighbours(for LSH) */ 
        void findANN(vector_item<T>&, float, float&, std::string&, std::ofstream&, std::unordered_set<std::string>&);

        /* Returns 1 if gs given are the same, else 0 */
        int comp_gs(std::vector<int>&,std::vector<int>&);

        /* Accessors */
        int get_k();
        long int get_size();
};




/////////////////////////
/** COSINE SIMILARITY **/
/////////////////////////

/** csimilarityHF **/
/* Hash function for cosine similarity metric */
template <class T>
class csimilarityHF{
    private:
        std::array<float, D> r; // random vector
    public:
        /* Con-De structor */
        csimilarityHF();
        
        int getValue(std::array<T,D>&); // get hash value
        long int get_size(); // get size of struct in bytes

        /* Debugging */
        void print();
};

/** csimilarity **/
template <class T>
class csimilarity{
	private:
        int k; // number of hash functions
        int tableSize; // number of buckets
        std::vector<csimilarityHF<T>> hfs; // hash functions
        std::vector<vector_item<T>*>* buckets; // all buckets
	public:
        /* Con-De structor */
		csimilarity(int);
        ~csimilarity();

        void add_vector(vector_item<T>*);

        /* Returns a pointer to the bucket with the given index */
        std::vector<vector_item<T>*>& get_bucket(int);

        /* Find and gather all candidate vectors using lsh range search */
        int first_assign(cluster<double>*, double&, std::unordered_set<std::string>&, std::vector<vector_check*>&, std::vector<cluster_info*>&, int&);

        /* Find and gather all candidate vectors using hc range search */
        int first_assign(cluster<double>*, double&, hypercube<T>&, std::vector<vector_check*>&, std::vector<cluster_info*>&, int&);

        /* Given a query vector, finds the nearest neighbours */ 
        void findANN(vector_item<T>&, float, float&, std::string&, std::ofstream&, std::unordered_set<std::string>&);

        /* Returns the value of the hash function given for the provided vector */
        int get_val_hf(std::array<T,D>&, int);

        /* Accessors */
        int get_k();

        long int get_size();
};
