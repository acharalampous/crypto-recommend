/*******************************/
/* clusters.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once
#include <fstream>
#include <vector>
#include <ctime>

#include "utils.h"
#include "dataset.h"
#include "cl_algorithms.h"

#define CONF_FILE1 "cluster1.conf" // configuration file for clustering tweets
#define CONF_FILE2 "cluster2.conf" // configuration file for clustering users - problem 2

template <class T> class cl_init_algorithm;
template <class T> class cl_assign_algorithm;
template <class T> class cl_update_algorithm;

struct exe_args; 

typedef double (*dist_func)(vector_item<double>&, vector_item<double>&);

/* Implementation of all necessary structs and methods that 
 * will be used in order to implement all the clustering 
 * algorithms 
 */


/* Class that holds info about the cluster that a vector_item belongs to */
class cluster_info{
    private:
        int is_centroid; // 1 if vector is the centroid of the cluster that it belongs to, else 0
        int cluster_num; // number of cluster that vector belongs to
        int index; // index of vector in dataset(that is mapped to this info)
        double distance; // distance from centroid, -1 if not assigned
    public:
        /* Con-De Structor */

        /* Given the index of vector, create a new mapped info about it */
        cluster_info(int);

        /* Accessors */
        int get_is_centroid(); 
        int get_cluster_num();
        int get_index();
        double get_distance();

        /* Mutators */
        void set_centroid(); // sets is_centroid as 1
        void set_cluster(int); // sets cluster_num to the given int 
        void set_index(int); // sets index to the given int
        void set_distance(double); // set distance from centroid to given double
        void reset_info(); // sets is_centroid as 0 and cluster_num as -1
};


/* Class that represents the cluster. Holds all necessary info of cluster, */
/* like pointer to centroid and all points in it                           */
template <class T>
class cluster{
    private:
        int cluster_num; // number of cluster
        int centroid_type; // 1: if centroid in dataset, 0: not in dataset, -1 no centroid
        vector_item<T>* centroid; // pointer to vector in dataset that is centroid
        std::vector<vector_item<T>*> vectors; // holds pointers to all vectors that are in cluster
        double silhouette;

    public:
        /* Con-De Structor */

        /* Given the number of cluster, create a new empty cluster */
        cluster(int);
        ~cluster();

        void add_vector(vector_item<T>*); // adds a new non-centoid vector in cluster
    
        /* Calculates and prints the cluster's average silhouette and finally returns the total */
        double evaluation(std::vector<cluster<T>*>&, dist_func&);
    
        /* Accessors */
        int get_cluster_num();
        int get_centroid_type();
        vector_item<T>* get_centroid(); 
        vector_item<T>* get_vector(int);
        std::vector<vector_item<T>*>& get_vectors();        
        int get_size();
        double get_silhouette();

        /* Mutators */
        void set_centroid(vector_item<T>*); // sets given vector as centroid
        void set_silhouette(double); // set silhouette to cluster
        void set_centroid_type(int); // sets centroid type 
        
        void print();
        void print_to_file(std::ofstream&);
};

/* Class that has all necessary structs collected */
template <class T>
class cl_management{
    private:
        dataset<T>* all_vectors; // dataset with all vectors
        std::vector<cluster_info*> vectors_info; // cluster info for each vector
        std::vector<cluster<T>*> clusters; // all clusters
        cl_init_algorithm<T>* init_algorithm; // algorithm for initiliazing clusters
        cl_assign_algorithm<T>* assign_algorithm; // algorithm for assigning vector to clusters
        cl_update_algorithm<T>* update_algorithm; // algorithm for updating centroids in clusters
        dist_func dist_function; // pointer to distance function, eucliean or cosine

        int max_updates; // max number of total updates to be done
        int metric; // metric to be used, 1: euclidean, 2: cosine
        int k; // number of total clusters
        double avg_silhouette; // avg silhouette of all points
        clock_t cl_start; // time started clustering
        clock_t cl_end; // time ended clustering
        int complete; // if complete = 1, print every item in cluster


        /* In case of lsh or hypercube */
        int L; // number of tables to be created
        int hf_num; // number of hash function to be created
        int hc_probes; // hc: number of neighbours to be checked
        int hc_M; // hc: number of items to be checked in total

    public:
        /* Con-De Structor */

        /* Given the metric, algorithms and number of clusters, initiliaze clusters controlling class */
        cl_management(exe_args&, int, int, int);
        ~cl_management();

        void clustering(exe_args&, int, int, int);

        /* Given a file stream, get all vectors and assign in dataset */
        void fill_dataset(std::ifstream&);

        /* Given vector of user, fill dataset */
        void fill_dataset(std::vector<user*>&);

        /* Clustering algorithms */
        void init_clusters(); // init clusters using the algorithm provided
        void assign_clusters(); // assign vectors to clusters using the assignment algorithm
        int update_clusters(); // update centroids using the algorithm provided
        
        /* Calculate and print silhouette of clusters and vectors */
        void silhouette();

        /* Timings */
        void tick();
        void tock();
        double get_time_elapsed(); // get time between tick and tock

        /*Accessors */
        dataset<T>* get_dataset();
        std::vector<cluster_info*>& get_vectors_info();
        std::vector<cluster<T>*>& get_clusters();
        int get_k();
        int get_max_updates();
        dist_func get_dist_func();

        void print();
        void print_to_file(std::ofstream&);

        void get_neighbours(vector_item<double>&, int, std::unordered_set<int>&);

        int nearest_cl(vector_item<double>&);
};