/*******************************/
/* lsh.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <unordered_set>
#include <string>

#include "lsh.h"
#include "metrics.h"

using namespace std;

//template class LSH<int>;
template class LSH<double>;

/*  Implementation of all functions of the class
 *  that is used for LSH. Definitions found in
 *  lsh.h.
 */

/* L: num of tables for each metric, k: num of hash functions, */
/* n: dataset size */ 
template <class T>
LSH<T>::LSH(int metrics, int L, int k, int n){
    this->L = L;
    if(metrics == 1){ // euclidean is defined
        for(int i = 0; i < L; i++)
            eu_tables.push_back(new euclidean<T>(k, n));
    }

    else if(metrics == 2){ // cosine is defined
        for(int i = 0; i < L; i++)
            cs_tables.push_back(new csimilarity<T>(k));
    }
}

template <class T>
LSH<T>::~LSH(){
    for(unsigned int i = 0; i < eu_tables.size(); i++){
        delete eu_tables[i];
    }

    eu_tables.clear();

    for(unsigned int i = 0; i < cs_tables.size(); i++){
        delete cs_tables[i];
    }

    cs_tables.clear();
}

template <class T>
void LSH<T>::add_vector(vector_item<T>* new_vector){
    for(unsigned int i = 0; i < eu_tables.size(); i++){
        eu_tables[i]->add_vector(new_vector);
    }

    for(unsigned int i = 0; i < cs_tables.size(); i++){
        cs_tables[i]->add_vector(new_vector);
    }
}

template <class T>
void LSH<T>::assign_clusters(cl_management<T>& cl_manage){
    vector<cluster<T>*>& clusters = cl_manage.get_clusters();
    int num_of_centroids = clusters.size(); // get number of centroids to be checked
    vector<unordered_set<string>*> checked_set; // set that holds items that were checked

    vector<vector<vector_check*>*> vectors_to_check; // avoid finding vectors and recalculating same distances 
    vector<cluster_info*> vectors_info = cl_manage.get_vectors_info();

    int vectors_left = vectors_info.size();

    /* Initialize cluster helping structs */
    for(int i = 0; i < num_of_centroids; i++){
        checked_set.push_back(new unordered_set<string>);
        vectors_to_check.push_back(new vector<vector_check*>);

        vectors_left -= clusters[i]->get_centroid_type();
    }

    /* Get starting radius */
    dist_func dist = cl_manage.get_dist_func();
    double r = get_starting_r(clusters, dist);

    /* Euclidean will be used in LSH */
    if(eu_tables.size() != 0){
        for(int i = 0; i < num_of_centroids; i++){
            for(unsigned int j = 0; j < eu_tables.size(); j++){
                /* First gather all canditates for current clusters */
                eu_tables[j]->first_assign(clusters[i], r, *(checked_set[i]), *(vectors_to_check[i]), vectors_info, vectors_left);
            }
        }
        while(1){
            int flag = 0;
            r = r * 2;
            
            for(int i = 0; i < num_of_centroids; i++){
                flag += add_to_clusters(clusters[i], r, *(vectors_to_check[i]), vectors_info, vectors_left);
            }

            /* Check if changes were made */
            if(flag == 0 || vectors_left == 0)
                break;
        }

        final_assign(cl_manage);
    }
    else if(cs_tables.size() != 0){
        for(int i = 0; i < num_of_centroids; i++){
            for(unsigned int j = 0; j < eu_tables.size(); j++){
                cs_tables[j]->first_assign(clusters[i], r, *(checked_set[i]), *(vectors_to_check[i]), vectors_info, vectors_left);
            }
        }

        while(1){
            int flag = 0;
            r = r * 2;
            
            for(int i = 0; i < num_of_centroids; i++){
                flag += add_to_clusters(clusters[i], r, *(vectors_to_check[i]), vectors_info, vectors_left);
            }

            /* Check if changes were made */
            if(flag == 0 || vectors_left == 0)
                break;
        }
        final_assign(cl_manage);
    }

    /* Delete containters used */
    for(unsigned int i = 0; i < vectors_to_check.size(); i++){
        for(unsigned int j = 0; j < vectors_to_check[i]->size(); j++){
            vector<vector_check*>* temp_vec = vectors_to_check[i];            
            if(temp_vec->at(j) != NULL){
                delete temp_vec->at(j);
            }
        }
        delete vectors_to_check[i];
        delete checked_set[i];
    }
}

template <class T>
void LSH<T>::findANN(vector_item<T>& query, float radius, float& min_dist, string& ANN_name, ofstream& output){
    unordered_set<string> checked_set; // set that holds items that were checked
    for(unsigned int i = 0; i < eu_tables.size(); i++){
        eu_tables[i]->findANN(query, radius, min_dist, ANN_name, output, checked_set);
    }

    for(unsigned int i = 0; i < cs_tables.size(); i++){
        cs_tables[i]->findANN(query, radius, min_dist, ANN_name, output, checked_set);
    }
}

template <class T>
long int LSH<T>::get_total_size(){
    long int total_size = 0;

    total_size += sizeof(*this);
    for(unsigned int i = 0; i < eu_tables.size(); i++){
        total_size += eu_tables[i]->get_size();
    }

    for(unsigned int i = 0; i < cs_tables.size(); i++){
        total_size += cs_tables[i]->get_size();
    }

    return total_size;
}