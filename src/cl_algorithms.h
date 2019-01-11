/*******************************/
/* cl_algorithms.h */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#pragma once

#include "dataset.h"
#include "clusters.h"
#include "lsh.h"
#include "hypercube.h"

template <class T> class cl_management;
template <class T> class cluster;
template <class T> class LSH;
template <class T> class hypercube;

/* Pointer to metric function */
typedef double (*dist_func)(vector_item<double>&, vector_item<double>&);

/* Implementation of all algorithms that will be used in clustering */


/* Initializing algorithms */
/* Abstract type of initializing algorithm */
template <class T>
class cl_init_algorithm{
    protected:
        
    public:
        virtual ~cl_init_algorithm(){}

        /* All init algorithms must have this method */
        virtual void init_clusters(cl_management<T>&) = 0;
        virtual int get_alg_id() = 0;

};

/* Algorithm that selects randomly each centroid of cluster */
template <class T>
class cl_init_random: public cl_init_algorithm<T>{
    private:

    public:
        /* Initialize clusters using random selection */
        void init_clusters(cl_management<T>&); 

        /* Returns algorithm id = 1 */
        int get_alg_id();

};

/* Implementation of k-means++ algorithm */
template <class T>
class cl_init_kmeans: public cl_init_algorithm<T>{
    private:
        /* Maps index of item to distance */
        struct dist_mapping{
            int index;
            double distance;

            dist_mapping(int i, double d) : index(i), distance(d){ }
        };
    public:
        /* Initialize clusters using random selection */
        void init_clusters(cl_management<T>&);
        
        /* Given the probabilities array, perform distribution and return */
        /* the array cell that was chosen, after doing binary search      */
        int get_next_centroid(std::vector<dist_mapping>&);
        
        /* Returns algorithm id = 2 */
        int get_alg_id();

};



/* Assignment algorithms */
template <class T>
class cl_assign_algorithm{
    protected:

    public:
        virtual ~cl_assign_algorithm(){}
        virtual void assign_clusters(cl_management<T>&) = 0;
        virtual int get_alg_id() = 0;

        /* LSH / HC */
        virtual void init_lsh(int a, int b, int c, int d) { }
        virtual void init_hc(int a, int b, int c, int d) { }
        virtual void add_vector(vector_item<T>* a) { }

};

template <class T>
class cl_assign_lloyd : public cl_assign_algorithm<T>{
    private:

    public:
        /* Assign to clusters using lloyds */
        void assign_clusters(cl_management<T>&);

        /* Returns algorithm id = 1 */
        int get_alg_id(); // returns algorithm id, in this case 1
};


template <class T>
class cl_assign_lsh: public cl_assign_algorithm<T>{
    private:
        LSH<T>* lsh;
    public:
        ~cl_assign_lsh();

        /* Initializes the lsh with the given parameters */
        void init_lsh(int, int, int, int);

        /* Returns algorithm id = 2 */
        int get_alg_id(); // returns algorithm id, in this case 2

        /* Add vector to LSH buckets */
        void add_vector(vector_item<T>*);

        /* Assing to clusters using lsh range search */
        void assign_clusters(cl_management<T>&);   
};

template <class T>
class cl_assign_hc: public cl_assign_algorithm<T>{
    private:
        hypercube<T>* hc;
    public:
        ~cl_assign_hc();
        void init_hc(int, int, int, int);
        
        /* Returns algorithm id = 3 */
        int get_alg_id();

        /* Add vector to HC buckets */
        void add_vector(vector_item<T>*);

        /* Assing to clusters using hc range search */
        void assign_clusters(cl_management<T>&);
};


/* Update algorithms */
template <class T>
class cl_update_algorithm{
    protected:

    public:
        virtual ~cl_update_algorithm(){}

        virtual int update_clusters(cl_management<T>&) = 0;
        virtual int get_alg_id() = 0;
};

template <class T>
class cl_update_kmeans : public cl_update_algorithm<T>{
    private:

    public:

        /* Update clusters using kmeans algorithm */
        int update_clusters(cl_management<T>&);
        
        /* Get new centroid, calculating avg for each point */
        vector_item<T>* get_new_centroid(cluster<T>&);
        
        /* Returns algorithm id = 1 */        
        int get_alg_id();

        
};

template <class T>
class cl_update_pam : public cl_update_algorithm<T>{
    private:

    public:
        /* Update clusters using medoids partioning algorithm */
        int update_clusters(cl_management<T>&);

        /* Calculates the average distance for every vector in given cluster */
        /* and returns the vector with minimum average distance              */
        vector_item<T>* get_new_centroid(cluster<T>&, dist_func&);
        
        /* Returns algorithm id = 2 */                
        int get_alg_id();

};