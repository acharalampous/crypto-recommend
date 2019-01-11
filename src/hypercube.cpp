/*******************************/
/* hypercube.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <random>

#include "utils.h"
#include "hypercube.h"
#include "metrics.h"

using namespace std;

//template class hypercube<int>;
template class hypercube<double>;

/*  Implementation of all functions of the class
 *  that is used for hypercube. Definitions found in
 *  hypercube.h.
 */

/* k: num of hash functions, probes: num of neighbour bucket to be checked */
/* M: num of items to be checked */ 
template <class T>
hypercube<T>::hypercube(int metric, int k, int probes, int M){
    eu_table = NULL;
    cs_table = NULL;
    fs = NULL;

    this->probes = probes;
    this->M = M;
    this->metric = metric;

    /* Create fi */
    fs = new unordered_map<int,int>[k];
    
    if(metric == 1){ // euclidean is defined
        int tablesize = pow(2, k) * TS_DIVISOR;
        eu_table = new euclidean<T>(k, tablesize);
    } 
    else if(metric == 2) // cosine is defined
        cs_table = new csimilarity<T>(k);
}

template <class T>
hypercube<T>::~hypercube(){
    if(eu_table != NULL)
        delete eu_table;
    if(cs_table != NULL)
        delete cs_table;

    if(fs != NULL)
        delete [] fs;
}

template <class T>
int hypercube<T>::check_map(int key, int index){
    unordered_map<int,int>::iterator it = fs[index].find(key);
    if(it == fs[index].end()){ // key is not yet registered    
        
        /* Create a new mapping between key and 0/1 */
        random_device rd;
	    mt19937 gen(rd());
        uniform_int_distribution<int> rand_01(0,1);
        
        fs[index][key] = rand_01(gen);

        return fs[index][key];
    }
    else // key was found
        return it->second;
}

template <class T>
void hypercube<T>::add_vector(vector_item<T>* new_vector){    
    int k;
    int bucket_num = 0;

    if(metric == 1){ // use euclidean
        k = eu_table->get_k(); // get number of hash functions
        vector<int>* hvalues = new vector<int>;
        array<T, D>& vec = new_vector->get_points();

        for(int i = 0; i < k; i++){
            /* Get hash function value */
            int fi = eu_table->get_val_hf(vec, i);

            hvalues->push_back(fi);

            /* Get 1 or 0 value from map */
            fi = check_map(fi, i);

            /* Find bucket */
            bucket_num += fi * pow(2, k - i - 1);
        }

        eu_table->add_vector(new_vector, hvalues, bucket_num);
    }

    else{
        cs_table->add_vector(new_vector);
    }
}

template <class T>
void hypercube<T>::assign_clusters(cl_management<T>& cl_manage){
    vector<cluster<T>*>& clusters = cl_manage.get_clusters();
    int num_of_centroids = clusters.size(); // get number of centroids to be checked
    vector<unordered_set<string>*> checked_set; // set that holds items that were checked

    vector<vector<vector_check*>*> vectors_to_check; // avoid finding vectors and recalculating same distances 
    vector<cluster_info*> vectors_info = cl_manage.get_vectors_info();

    int vectors_left = vectors_info.size();

    for(int i = 0; i < num_of_centroids; i++){
        checked_set.push_back(new unordered_set<string>);
        vectors_to_check.push_back(new vector<vector_check*>);

        vectors_left -= clusters[i]->get_centroid_type();
    }

    dist_func dist = cl_manage.get_dist_func();
    double r = get_starting_r(clusters, dist);

    /* Euclidean will be used in HC */
    if(eu_table != NULL){
        for(int i = 0; i < num_of_centroids; i++){
            eu_table->first_assign(clusters[i], r, *this, *(vectors_to_check[i]), vectors_info, vectors_left);
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
    else if(cs_table != NULL){
        for(int i = 0; i < num_of_centroids; i++){
            cs_table->first_assign(clusters[i], r, *this, *(vectors_to_check[i]), vectors_info, vectors_left);
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

    /* Delete containter used */
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
void hypercube<T>::findANN(vector_item<T>& query, float radius, float& min_dist, string& ANN_name, ofstream& output){   
    
    if(metric == 1){ // euclidean will be used
        int k = eu_table->get_k(); // get number of hash functions

        array<T, D>& vec = query.get_points();
        
        /* Find bucket num of query */
        int bucket_num = 0;
        for(int i = 0; i < k; i++){
            /* Get hash function value */
            int fi = eu_table->get_val_hf(vec, i);

            /* Get 1 or 0 value from map */
            fi = check_map(fi, i);

            /* Find bucket */
           bucket_num += fi * pow(2, k - i - 1);
        }

        /* Find number of neighbours that need to be checked, according to probes */
        vector<int>* neighbours = find_neighbours(bucket_num, probes);

        int vector_sz = this->eu_table->get_k() + 1; // size of vector
        
        int remaining_probes = probes; // number of neighbours left to check
        int remaining_items = M; // number of items left to check
        int flag = 0; // if flag = 1, reached probes of M

        /* Check neighbours found */
        for(int i = 0; i < vector_sz; i++){
            if(flag == 1)
                break;

            /* Get neighbours */
            for(unsigned int j = 0; j < neighbours[i].size(); j++){
                
                remaining_probes--; // about to check another neighbour
                if(remaining_probes < 0){ // no more neighbours to check
                    flag = 1;    
                    break;
                } 

                /* Get bucket from euclidean table */
                int f = neighbours[i][j];
                vector<euclidean_vec<T>*>& buck = eu_table->get_bucket(f);

                /* If radius was given as 0, find only the nearest neighbour */
                if(radius == 0){
                    for(unsigned int i = 0; i < buck.size(); i++){
                        remaining_items--; // about to check another vector
                        if(remaining_items < 0){ // no more items to check
                            flag = 1;    
                            break;
                        } 

                        euclidean_vec<T>* cur_vec = buck[i]; // get current vector
                        vector_item<T>& item = cur_vec->get_vec();

                        string& item_id = item.get_id(); // get item id

                        float dist = eucl_distance(query, item);

                        /* Check if nearest neighbour */
                        if(dist <= min_dist || min_dist == -1.0){
                            min_dist = dist;
                            ANN_name.assign(item_id);
                        }
                    } // end for all items in buckets 
                } // end if radius > 0


                /* Else check for items in radius */
                else{
                    for(unsigned int i = 0; i < buck.size(); i++){
                        remaining_items--; // about to check another vector
                        if(remaining_items < 0){ // no more items to check
                            flag = 1;    
                            break;
                        } 
                        
                        euclidean_vec<T>* cur_vec = buck[i]; // get current vector
                        vector_item<T>& item = cur_vec->get_vec();
                        string& item_id = item.get_id(); // get item id

                        float dist = eucl_distance(query, item);

                        /* Print item in radius of query */
                        if(dist <= radius){
                            output << "\t" << item_id << endl;
                        }
                        
                        /* Check if nearest neighbour */
                        if(dist <= min_dist || min_dist == -1.0){
                            min_dist = dist;
                            ANN_name.assign(item_id);
                        }
                    } // end for all items in buckets
                } // end else radius > 0
            } // end for neighbours in current distance
        }// end for neighbours in probe
        delete [] neighbours;
    } // end if metric == euclidean

    else if(metric == 2){
        int k = cs_table->get_k(); // get number of hash functions
        int f = 0; // f(p) function <-> bucket index 
	
        /* Get vector points */
        array<T, D>* vec_points = &(query.get_points());

        /* Get all hash functions values */
        for(int i = 0; i < k; i++){
            int temp = cs_table->get_val_hf(*vec_points, i);
            f += temp * pow(2, k - i - 1); // compute binary value
        }

        vector<int>* neighbours = find_neighbours(f, probes);

        int vector_sz = this->cs_table->get_k() + 1; // size of vector
        
        int remaining_probes = probes;
        int remaining_items = M;
        int flag = 0;

        /* Check neighbours found */
        for(int i = 0; i < vector_sz; i++){
            if(flag == 1)
                break;

            for(unsigned int j = 0; j < neighbours[i].size(); j++){
                
                remaining_probes--; // about to check another neighbour
                if(remaining_probes < 0){ // no more neighbours to check
                    flag = 1;    
                    break;
                } 

                /* Get bucket from cosine table */
                int f = neighbours[i][j];
                vector<vector_item<T>*>& buck = cs_table->get_bucket(f);

                /* If radius was given as 0, find only the nearest neighbour */
                if(radius == 0){
                    for(unsigned int i = 0; i < buck.size(); i++){
                        remaining_items--; // about to check another vector
                        if(remaining_items < 0){ // no more neighbours to check
                            flag = 1;    
                            break;
                        } 

                        vector_item<T>* item = buck[i]; // get current vector
                        string& item_id = item->get_id(); // get item id

                        float dist = cs_distance(query, *item);

                        /* Check if nearest neighbour */
                        if(dist <= min_dist || min_dist == -1.0){
                            min_dist = dist;
                            ANN_name.assign(item_id);
                        }
                    } // end for all items in buckets 
                } // end if radius > 0


                /* Else check for items in radius */
                else{
                    for(unsigned int i = 0; i < buck.size(); i++){
                        remaining_items--; // about to check another vector
                        if(remaining_items < 0){ // no more items to check
                            flag = 1;    
                            break;
                        } 
                        
                        vector_item<T>* item = buck[i]; // get current vector
                        string& item_id = item->get_id(); // get item id

                        float dist = cs_distance(query, *item);

                        /* Print item in radius of query */
                        if(dist <= radius){
                            output << "\t" << item_id << endl;
                        }
                        
                        /* Check if nearest neighbour */
                        if(dist <= min_dist || min_dist == -1.0){
                            min_dist = dist;
                            ANN_name.assign(item_id);
                        }
                    } // end for all items in buckets
                } // end else radius > 0
            } // end for neighbours in current distance
        } // end for neighbours in probe
        delete [] neighbours;
    }
}



template <class T>
vector<int>* hypercube<T>::find_neighbours(int num, int probes){
    vector<int> count; // each cell has number of neighbours in corresponding distance
    int num_of_bits = -1;
    if(metric == 1)
        num_of_bits = this->eu_table->get_k();
    else if(metric == 2)
        num_of_bits = this->cs_table->get_k();

    int max_distance = num_of_bits + 1;

    /* neihbours[x] = neighbours in hamming distance x, x : {0, 1, 2, 3, ..., max_distance} */
    vector<int>* neighbours = new vector<int>[max_distance]; // neighbours buckets that will be returned

    int remaining_n = probes - 1; // remaining neigbours that need to be found
    neighbours[0].push_back(num); // self bucket must still be checked
    count.push_back(0);

    /* Find how many neihbours must be found in each distance, until probes are reached */
    for(int i = 1; i < max_distance; i++){

        /* Find number of neighbours in i hamming distance */
        int combs = get_combinations(num_of_bits, i);
        if(combs >= remaining_n){ // last neighbours number is found
            count.push_back(remaining_n);
            for(int j = i + 1; j < max_distance; j++)
                count.push_back(0);
            break;
        } 
        else{
            count.push_back(combs); // keep number of neighbours in distance i and keep searching
            remaining_n -= combs;
        }
    }

    remaining_n = probes - 1;
    int max_num = pow(2, num_of_bits); // maximum possible number that can be represented by num_of_bits
    for(int i = 0; i < max_num && remaining_n > 0; i++ ){
        if(i == num) // if same number, pass(already placed)
            continue;
        
        /* Compute hamming dist */
        int dist = hamming_dist(i, num);

        /* If neighbour is needed, then save in vector */
        if(count[dist] > 0){
            neighbours[dist].push_back(i);
            count[dist]--;
            remaining_n--;
        }
    }

    return neighbours;
}

template <class T>
long int hypercube<T>::get_total_size(){
    long int total_size = 0;
    int num_of_maps = 0;

    /* Get size of struct */
    total_size += sizeof(*this);

    /* If exists, get size of eu_table */
    if(eu_table != NULL){
        total_size += eu_table->get_size();
        num_of_maps = eu_table->get_k();
    }
        
    /* If exists, get size of cs_table */
    if(cs_table != NULL){
        total_size += cs_table->get_size();
        num_of_maps = cs_table->get_k();
    }

    /* Get size of maps */
    for(int i = 0; i < num_of_maps; i++){
        /* get size of each map + (2*int) * records */
        total_size += sizeof(fs[i]) + (fs[i].size() * (2 * sizeof(int)));
    }
    
    return total_size;
}

template <class T>
int hypercube<T>::get_probes(){
    return this->probes;
}

template <class T>
int hypercube<T>::get_M(){
    return this->M;
}