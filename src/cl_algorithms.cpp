/*******************************/
/* cl_algorithms.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <random>
#include <array>

#include "cl_algorithms.h"
#include "lsh.h"
#include "hypercube.h"

using namespace std;

template class cl_init_random<double>;
template class cl_init_kmeans<double>;

template class cl_assign_lloyd<double>;
template class cl_assign_lsh<double>;
template class cl_assign_hc<double>;

template class cl_update_kmeans<double>;
template class cl_update_pam<double>;


/*  Implementation of all functions of clustering algorithms
 *  Definitions found in cl_algorithms.h.
 */



////////////////////
// CL_INIT_RANDOM //
////////////////////
template <class T>
void cl_init_random<T>::init_clusters(cl_management<T>& cl_manage){
    random_device rd;
    mt19937 gen(rd()); // choose random vector as centroids
        
    dataset<T>* all_vectors = cl_manage.get_dataset();
    vector<cluster_info*>& vectors_info = cl_manage.get_vectors_info();
    vector<cluster<T>*>& clusters = cl_manage.get_clusters();


    int k = cl_manage.get_k(); // get number of total clusters

    int num_of_vectors = all_vectors->get_counter(); // get num of vectors

    uniform_int_distribution<int> rand_Z(0, num_of_vectors - 1);

    for(int i = 0; i < k; i++){
        int sel = rand_Z(gen); // select random vector from dataset
        
        if(vectors_info[sel]->get_is_centroid() == 1){ // vector already a centroid, so choose another random
            i--;
            continue;
        }
    
        /* Set point as centroid */
        /* Update cluster info */
        clusters[i]->set_centroid(all_vectors->get_item(sel));
        clusters[i]->set_centroid_type(1);
        
        /* Update vector info */
        vectors_info[sel]->set_centroid();
        vectors_info[sel]->set_cluster(i);
    }
}


template <class T>
int cl_init_random<T>::get_alg_id(){
    return 1;
}



////////////////////
// CL_INIT_KMEANS //
////////////////////
template <class T>
void cl_init_kmeans<T>::init_clusters(cl_management<T>& cl_manage){
    random_device rd;
    mt19937 gen(rd()); // choose random vector as centroids
        
    dataset<T>* all_vectors = cl_manage.get_dataset();
    vector<cluster_info*>& vectors_info = cl_manage.get_vectors_info();
    vector<cluster<T>*>& clusters = cl_manage.get_clusters();
    
    dist_func dist_function;
    dist_function = cl_manage.get_dist_func();

    int k = cl_manage.get_k(); // get number of total clusters
    int num_of_vectors = all_vectors->get_counter(); // get num of vectors

    /* The first centroid is chosen randomly */
    uniform_int_distribution<int> rand_Z(0, num_of_vectors - 1);
    int sel = rand_Z(gen); // select random first centroid

    /* Set point as centroid */
    clusters[0]->set_centroid(all_vectors->get_item(sel));
    clusters[0]->set_centroid_type(1);
    
    vectors_info[sel]->set_centroid();
    vectors_info[sel]->set_cluster(0);

    /* Array that maps the minimum distance of each vector to all centroids */
    vector<T> min_distances;
    for(int i = 0; i < num_of_vectors; i++) // Initialize array
        min_distances.push_back(0.0);
    
    double total_distance = 0.0; // distance of the furthest vector

    vector<dist_mapping> probs_array; // array that holds probabilities

    /* Find the rest k - 1 centroids */
    for(int i = 1; i < k; i++){
        /* In each iteration we need to check the distance from the last found centroid */
        /* as we already have the minimum distance of the rest, from previous iterations*/
        vector_item<T>* last_centroid = clusters[i - 1]->get_centroid();
        if(last_centroid == NULL)
            return;


        /* Clear probabilities array and push the starting 0.0 */
        probs_array.clear();
        probs_array.push_back(dist_mapping(-1, 0.0));

        /* Reset total distance */
        total_distance = 0.0;

        /* First iteration, so wont check for min_distance(minimize checks) */
        if(i == 1){
            
            /* Find minimum distance to centroids. Must check only with the last centroid */
            for(int j = 0; j < num_of_vectors; j++){

                /* Check if not centroid */
                if(vectors_info[j]->get_is_centroid() == 1){
                    continue;
                }

                vector_item<T>* curr_item = all_vectors->get_item(j);

                /* Set minimum distance */
                min_distances[j] = dist_function(*curr_item, *last_centroid);

                double prob = pow(min_distances[j], 2); // calculate probability of vector
                total_distance += prob; // increase total distance

                probs_array.push_back(dist_mapping(curr_item->get_index(), prob));

                // probs_array.push_back(dist_mapping(curr_item->get_index(), min_distances[j]));

            }
        } // end if i == 1
        else{
            /* Find minimum distance to centroids. Must check only with the last centroid */
            for(int j = 0; j < num_of_vectors; j++){
                /* Check if not centroid */
                if(vectors_info[j]->get_is_centroid() == 1){
                    continue;
                }

                vector_item<T>* curr_item = all_vectors->get_item(j);

                /* Find distance with last centroid and check if less than the minimum distance from all previous centroids */
                double dist = dist_function(*curr_item, *last_centroid);
                if(dist < min_distances[j]){
                    min_distances[j] = dist;
                }

                double prob = pow(min_distances[j], 2); // calculate probability of vector
                total_distance += prob; // increase total distance

                probs_array.push_back(dist_mapping(curr_item->get_index(), prob));
            
            }
        } // end else i != 1

        /* Normalize distances */
        unsigned int array_sz = probs_array.size();
        for(unsigned int j = 1; j < array_sz; j++){
            probs_array[j].distance = (double)(probs_array[j].distance / total_distance) + probs_array[j - 1].distance;
        }

        /* Get index of vector and set it as centroid in next cluster */
         sel = get_next_centroid(probs_array);
        
        clusters[i]->set_centroid(all_vectors->get_item(sel));
        clusters[i]->set_centroid_type(1);
        
        vectors_info[sel]->set_centroid();
        vectors_info[sel]->set_cluster(i);
    } // end finding centroids
}

template <class T>
int cl_init_kmeans<T>::get_next_centroid(vector<dist_mapping>& probs_array){
    random_device rd;
    mt19937 gen(rd());
    
    int start = 0;
    int end = probs_array.size() - 1;

    double min_val = 0.0;
    double max_val = probs_array[end].distance;

    uniform_real_distribution<> rand_R(min_val, max_val);

    double val = rand_R(gen);

    /* perform binary search for given val */
    while(1){
        int pivot = (end + start) / 2;
        
        if(val == probs_array[pivot].distance) // found value, return current pivot index
            return probs_array[pivot].index;
        else if(val >= probs_array[pivot + 1].distance){ // larger than end
            start = pivot;
            continue;
        }
        else if(val < probs_array[pivot].distance){ // less than start
            end = pivot;
            continue;
        }
        else if(val > probs_array[pivot].distance && val < probs_array[pivot + 1].distance) // found value field, return pivot + 1 index
            return probs_array[pivot + 1].index;

        /* Error checking */
        cout << "[BINARY SEARCH]I SHOULD NOT BE HERE" << endl;
    }
}

template <class T>
int cl_init_kmeans<T>::get_alg_id(){
    return 2;
}



/////////////////////
// CL_ASSIGN_LLOYD //
/////////////////////
template <class T>
void cl_assign_lloyd<T>::assign_clusters(cl_management<T>& cl_manage){
    dataset<T>* all_vectors = cl_manage.get_dataset();
    vector<cluster_info*>& vectors_info = cl_manage.get_vectors_info();
    vector<cluster<T>*>& clusters = cl_manage.get_clusters();
    
    dist_func dist_function = cl_manage.get_dist_func();

    int k = cl_manage.get_k(); // get number of total clusters
    int num_of_vectors = all_vectors->get_counter(); // get num of vectors

    /* Assign all non-centroids in dataset, to nearest centroid's cluster */ 
    for(int i = 0; i < num_of_vectors; i++){
        /* Check if not centroid */
        if(vectors_info[i]->get_is_centroid() == 1){
            continue;
        }

        /* Get current vector */
        vector_item<T>* curr_item = all_vectors->get_item(i);
        double min_distance = 0.0;
        int cluster_num = -1;
        
        /* Check for all centroids and find the nearest */
        for(int j = 0; j < k; j++){
            vector_item<T>* curr_centroid = clusters[j]->get_centroid();

            /* Get distance and check if minimum */
            double dist = dist_function(*curr_item, *curr_centroid);
            if(j == 0 || dist <= min_distance){
                min_distance = dist;
                cluster_num = j;
            }
        }

        clusters[cluster_num]->add_vector(curr_item);
        
        vectors_info[i]->set_cluster(cluster_num);
        vectors_info[i]->set_distance(min_distance);
    }
}

template <class T>
int cl_assign_lloyd<T>::get_alg_id(){
    return 1;
}



///////////////////
// CL_ASSIGN_LSH //
///////////////////
template <class T>
cl_assign_lsh<T>::~cl_assign_lsh(){
    if(this->lsh != NULL)
        delete this->lsh;
}

template <class T>
void cl_assign_lsh<T>::init_lsh(int metric, int L, int hfs_num, int n){
    this->lsh = new LSH<T>(metric, L, hfs_num, n);
}

template <class T>
int cl_assign_lsh<T>::get_alg_id(){
    return 2;
}

template <class T>
void cl_assign_lsh<T>::add_vector(vector_item<T>* new_vector){
    this->lsh->add_vector(new_vector);
}

template <class T>
void cl_assign_lsh<T>::assign_clusters(cl_management<T>& cl){
    lsh->assign_clusters(cl);
} 




//////////////////
// CL_ASSIGN_HC //
//////////////////
template <class T>
cl_assign_hc<T>::~cl_assign_hc(){
    if(this->hc != NULL)
        delete this->hc;
}

template <class T>
void cl_assign_hc<T>::init_hc(int metric, int hfs_num, int probes, int hc_M){
    this->hc = new hypercube<T>(metric, hfs_num, probes, hc_M);
}

template <class T>
void cl_assign_hc<T>::add_vector(vector_item<T>* new_vector){
    this->hc->add_vector(new_vector);
}

template <class T>
int cl_assign_hc<T>::get_alg_id(){
    return 3;
}

template <class T>
void cl_assign_hc<T>::assign_clusters(cl_management<T>& cl_manage){
    this->hc->assign_clusters(cl_manage);
} 




//////////////////////
// CL_UPDATE_KMEANS //
//////////////////////
template <class T>
int cl_update_kmeans<T>::update_clusters(cl_management<T>& cl_manage){
    int made_changes = 0; // 1: at least one centroid changed, 0: no changes

    vector<cluster_info*>& vectors_info = cl_manage.get_vectors_info();
    vector<cluster<T>*>& clusters = cl_manage.get_clusters();

    int k = cl_manage.get_k(); // get number of total clusters

    /* Update centroids in all clusters */ 
    for(int i = 0; i < k; i++){

        /* Get new centroid for current cluster */
        vector_item<T>* new_centroid = get_new_centroid(*clusters[i]);

        vector_item<T>* old_centroid = clusters[i]->get_centroid();

        /* If not differences found yet, check for different centroid */
        if(made_changes == 0){
            int equal = new_centroid->is_equal(*old_centroid);
            made_changes = (equal != 1) ? 1 : 0;
        }

        /* Centroid not in dataset, so it must be destroyed */
        if(clusters[i]->get_centroid_type() == 0)
            delete old_centroid;
        else{
            /* Get index of old centroid to reset it(stop it from being centroid) */
            int old_index = old_centroid->get_index();
            vectors_info[old_index]->reset_info();
        }

        int cl_size = clusters[i]->get_size();
        
        /* Reset cluster, remove every vector from it */
        for(int j = 0; j < cl_size; j++){
            vector_item<T>* curr_vector = clusters[i]->get_vector(j);
            int item_index = curr_vector->get_index();

            vectors_info[item_index]->reset_info();
        }

        (clusters[i]->get_vectors()).clear();

        /* Set new_centroid */
        clusters[i]->set_centroid(new_centroid);
        clusters[i]->set_centroid_type(0);

    }   

    return made_changes;
}

template <class T>
vector_item<T>* cl_update_kmeans<T>::get_new_centroid(cluster<T>& cl){
    /* New centroid will not be in dataset, so create a new one */
    vector_item<T>* new_centroid = new vector_item<T>;
    array<T, D>& coordinates = new_centroid->get_points();

    /* Get number of vectors in cluster + centroid */
    int num_of_vectors = cl.get_size() + 1;
    
    /* Get old centroid and add to new centroid */
    array<T, D>& old_coord = (cl.get_centroid())->get_points();
    for(int j = 0; j < D; j++)
        coordinates[j] += old_coord[j];


    /* Get all vectors in cluster and add its points to new centroid */
    for(int i = 0; i < num_of_vectors - 1; i++){

        /* Get current vector from cluster */
        array<T, D>& curr_coord = (cl.get_vector(i))->get_points();
        for(int j = 0; j < D; j++){
            coordinates[j] += curr_coord[j];
        }

        /* Calculate median for current dimension */
    }

    for(int j = 0; j < D; j++)
        coordinates[j] /= (double)num_of_vectors;
    
    return new_centroid;
}

template <class T>
int cl_update_kmeans<T>::get_alg_id(){
    return 1;
}


///////////////////
// CL_UPDATE_PAM //
///////////////////
template <class T>
int cl_update_pam<T>::update_clusters(cl_management<T>& cl_manage){
    int made_changes = 0; // 1: at least one centroid changed, 0: no changes
    
    vector<cluster_info*>& vectors_info = cl_manage.get_vectors_info();
    vector<cluster<T>*>& clusters = cl_manage.get_clusters();
    
    dist_func dist_function;
    dist_function = cl_manage.get_dist_func();

    int k = cl_manage.get_k(); // get number of total clusters

    /* Find medoids and update centroids in all clusters */ 
    for(int i = 0; i < k; i++){

        /* Get new centroid for current cluster */
        vector_item<T>* new_centroid = get_new_centroid(*(clusters[i]), dist_function);

        vector_item<T>* old_centroid = clusters[i]->get_centroid();

        /* If not differences found yet, check for different centroid */
        if(made_changes == 0){
            int equal = new_centroid->is_equal(*old_centroid);
            made_changes = (equal != 1) ? 1 : 0;
        }

        /* Get index of old centroid to reset it(stop it from being centroid) */
        int old_index = old_centroid->get_index();
        vectors_info[old_index]->reset_info();

        int cl_size = clusters[i]->get_size();
        
        /* Reset cluster, remove every vector from it */
        for(int j = 0; j < cl_size; j++){
            vector_item<T>* curr_vector = clusters[i]->get_vector(j);
            int item_index = curr_vector->get_index();

            vectors_info[item_index]->reset_info();
        }

        (clusters[i]->get_vectors()).clear();

        /* Set new_centroid */
        clusters[i]->set_centroid(new_centroid);
        clusters[i]->set_centroid_type(1);

        int centroid_index = new_centroid->get_index();
        vectors_info[centroid_index]->set_centroid();
        vectors_info[centroid_index]->set_cluster(i);

    }

    return made_changes;
}

template <class T>
vector_item<T>* cl_update_pam<T>::get_new_centroid(cluster<T>& cl, dist_func& dist){
    /* Get number of vectors in cluster + centroid */
    const int num_of_vectors = cl.get_size() + 1;
    
    /* Array that will hold the distances of each vector with the rest vectors. */
    /* At index 0 is the distance for the current centroid                      */
    double* distances = new double[num_of_vectors];
    distances[0] = 0.0; // initialize distance of centroid
    
    /* Get old centroid and calculate distance with all vectors */
    vector_item<T>* old_centroid = cl.get_centroid();
    for(int j = 0; j < num_of_vectors - 1; j++){
        /* Get current vector from cluster */
        vector_item<T>* curr_vector = cl.get_vector(j);

        /* Calculate distance and add to distances */
        double distance = dist(*old_centroid, *curr_vector);
        distances[0] += distance;
        distances[j + 1] = distance; // initiliaze distance for current vector(no need to recalculate)

    }

    /* Get all vectors in cluster and add its points to new centroid */
    for(int i = 0; i < num_of_vectors - 1; i++){

        /* Get current vector from cluster */
        vector_item<T>* curr_vector = cl.get_vector(i);

        /* Calculate the distance with the remaining vectors */
        for(int j = i + 1; j < num_of_vectors - 1; j++){

            vector_item<T>* next_vector = cl.get_vector(j);

            /* Calculate distance and add to distances */
            double distance = dist(*curr_vector, *next_vector);
            distances[i + 1] += distance;
            distances[j + 1] += distance;  

        }
    }

    /* Calculate final average distance for each vector */
    for(int j = 0; j < num_of_vectors; j++)
        distances[j] /= (double)(num_of_vectors - 1); // except itself
    

    /* Find vector with smallest distance */
    double min_distance = distances[0];
    int new_centroid = -1;
    for(int j = 1; j < num_of_vectors; j++){
        if(distances[j] <= min_distance){
            min_distance = distances[j];
            new_centroid = j - 1;
        }     
    }

    delete [] distances;
    if(new_centroid == -1)
        return cl.get_centroid();
    else
        return cl.get_vector(new_centroid);
}

template <class T>
int cl_update_pam<T>::get_alg_id(){
    return 2;
}