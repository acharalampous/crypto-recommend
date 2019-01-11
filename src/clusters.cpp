/*******************************/
/* clusters.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <fstream>

#include "utils.h"
#include "clusters.h"

using namespace std;

template class cluster<double>;
template class cl_management<double>;



/*  Implementation of all functions of cluster_info
 *  Definitions found in clusters.h.
 */


//////////////////////
//** CLUSTER_INFO **//
//////////////////////
cluster_info::cluster_info(int index){ 
    is_centroid = 0;
    cluster_num = -1;
    this->index = index;
    distance = -1.0;
}

/* Returns is_centroid */
int cluster_info::get_is_centroid(){
    return this->is_centroid;
}

/* Returns cluster_num */
int cluster_info::get_cluster_num(){
    return this->cluster_num;
}

/* Returns index of vector in dataset */
int cluster_info::get_index(){
    return this->index;
}

/* Returns the distance of vector to centroid */
double cluster_info::get_distance(){
    return this->distance;
}

/* Set is_centroid as 1 */
void cluster_info::set_centroid(){
    this->is_centroid = 1;
}

/* Set cluster_num as the given int */
void cluster_info::set_cluster(int new_cluster){
    this->cluster_num = new_cluster;
}

/* Set index as the given int */
void cluster_info::set_index(int new_index){
    this->index = 1;
}

/* Set distance of vector to centroid */
void cluster_info::set_distance(double new_dist){
    this->distance = new_dist;
}


/* Unassign vector from cluster, setting cluster_num to -1 */
void cluster_info::reset_info(){
    this->is_centroid = 0;
    this->cluster_num = -1;
    this->distance = -1.0;
}



/////////////
// CLUSTER //
/////////////
template <class T>
cluster<T>::cluster(int cluster_num){
    this->cluster_num = cluster_num;
    this->centroid_type = -1;
    this->centroid = NULL;
    this->silhouette = -1.0;
}

template <class T>
cluster<T>::~cluster(){
    /* If centroid not in dataset, destroy it from here */
    if(centroid_type == 0)
        delete centroid;
    
    vectors.clear();
}

/* Add new vector to cluster(non-centroid) */
template <class T>
void cluster<T>::add_vector(vector_item<T>* new_vector){
    this->vectors.push_back(new_vector);
}


template <class T>
double cluster<T>::evaluation(vector<cluster<T>*>& clusters, dist_func& dist){
    int num_of_vectors = vectors.size(); // Get number of vectors in cluster
    int ct = -1; // centroid_type -> 1: if there is centroid from dataset, else 0
    double* s_values;
    double s_of_cluster = 0.0; // Average s(i) of cluster

    if(num_of_vectors == 0){
        this->silhouette = 0.0;
        return 0.0;
    }
    /* Must not perform silhouette for centroid if it is not in dataset */
    if(this->get_centroid_type() == 0){
        ct = 0;
        s_values = new double[num_of_vectors];

        /* Initialize Silhouettes Values */
        for(int i = 0; i < num_of_vectors; i++)
            s_values[i] = 0.0;
    }
    else{ // centroid is in dataset, so calculate its silhouette
        ct = 1;
        vector_item<T>* old_centroid = centroid;
        s_values = new double[num_of_vectors + 1];
        s_values[0] = 0.0; // Initialize centroids value

        /* Calculate a(i) of centroid */
        for(int i = 0; i < num_of_vectors; i++){
            
            /* Get current vector from cluster */
            vector_item<T>* curr_vector = vectors[i];

            /* Calculate distance and add to distances */
            double distance = dist(*old_centroid, *curr_vector);
            s_values[0] += distance;
            s_values[i + 1] = distance; // initialize and keep distance of i for later
        }

        /* No vectors in cluster, avoid division by 0 */
        if(num_of_vectors == 0){
            delete [] s_values;
            this->silhouette = 0.0;
            return 0.0;
        }
        /* Calculate average a(i) */
        s_values[0] = s_values[0] / (double)num_of_vectors; // except itself

        /* Find nearest (or 2nd nearest) cluster */
        int sec_best = get_second_best(*old_centroid, this->cluster_num, clusters, dist);
        double b_value = calculate_b(*old_centroid, clusters[sec_best], dist);
    
        /* Check for max{a(i), b(i)} */
        if(s_values[0] >= b_value && s_values[0] != 0.0){
            s_values[0] = (b_value - s_values[0]) / s_values[0];
        }
        else if(b_value != 0.0){
            s_values[0] = (b_value - s_values[0]) / b_value;
        }
        else{
            delete [] s_values;
            this->silhouette = 0.0;
            return 0.0;
        }

        s_of_cluster += s_values[0]; // increase total s(i) of cluster
    }


    for(int i = 0; i < num_of_vectors; i++){
        int ind = i + ct; // index on s_values

        /* Get current vector from cluster */
        vector_item<T>* curr_vector = vectors[i];

        /* Calculate the distance with the remaining vectors */
        for(int j = i + 1; j < num_of_vectors; j++){

            vector_item<T>* next_vector = vectors[j];

            /* Calculate distance and add to distances */
            double distance = dist(*curr_vector, *next_vector);
            s_values[ind] += distance;
            s_values[j + ct] += distance;  

        }

        /* There are no other vectors in cluster */
        if((num_of_vectors + ct - 1) == 0){
            break;
        }

        s_values[ind] /= double(num_of_vectors + ct - 1); // except itself

        int sec_best = get_second_best(*curr_vector, this->cluster_num, clusters, dist);
        double b_value = calculate_b(*curr_vector, clusters[sec_best], dist);
    
        if(s_values[ind] >= b_value && s_values[ind] != 0.0){
            s_values[ind] = (b_value - s_values[ind]) / s_values[ind];
        }
        else if(b_value != 0.0){
            s_values[ind] = (b_value - s_values[ind]) / b_value;
        }
        else{
            s_values[ind] = 0.0;
        }

        s_of_cluster += s_values[ind];
    }

    double s_total = s_of_cluster;
    s_of_cluster /= (double)(num_of_vectors + ct);

    /*** Output ***/
    this->silhouette = s_of_cluster;

    delete [] s_values;

    return s_total;
}


/* Returns number(id) of cluster */
template <class T>
int cluster<T>::get_cluster_num(){
    return this->cluster_num;
}

/* Returns type of centroid */
template <class T>
int cluster<T>::get_centroid_type(){
    return this->centroid_type;
}

/* Returns pointer to centroid */
template <class T>
vector_item<T>* cluster<T>::get_centroid(){
    return this->centroid;
}

/* Returns pointer to vector in given index of vector */
template <class T>
vector_item<T>* cluster<T>::get_vector(int index){
    return vectors[index];
}

/* Returns all vectors' container */
template <class T>
vector<vector_item<T>*>& cluster<T>::get_vectors(){
    return vectors;
}

/* Returns number of vectors in cluster */
template <class T>
int cluster<T>::get_size(){
    return this->vectors.size();
}

/* Returns silhouette value */
template <class T>
double cluster<T>::get_silhouette(){
    return this->silhouette;
}

/* Sets new centroid */
template <class T>
void cluster<T>::set_centroid(vector_item<T>* new_centroid){
    this->centroid = new_centroid;
}

/* Set centroid type */
template <class T>
void cluster<T>::set_centroid_type(int type){
    this->centroid_type = type;
}

/* Set silhouette */
template <class T>
void cluster<T>::set_silhouette(double silhouette){
    this->silhouette = silhouette;
}

template <class T>
void cluster<T>::print(){
    cout << "Cluster #" << this->get_cluster_num() << " || Centroid: " << get_centroid()->get_id() << endl;

    for(unsigned int i = 0; i < vectors.size(); i++){
        cout << "\t" << i << ". " << vectors[i]->get_id() << endl;
    }
}

template <class T>
void cluster<T>::print_to_file(ofstream& out){
    /* Print cluster num + size + centroid */
    out << "CLUSTER-" << cluster_num << " {size: " << vectors.size() << " , centroid: ";

    /* Print only id of centroid */
    if(centroid_type == 1){
        out << centroid->get_id() << "}" << endl;
    }
    else{ // must print its points */
        std::array<double, D>& coordinates = centroid->get_points(); // points of vector
        out << coordinates[0]; 
        for(int i = 1; i < D; i++)
            out << ", " << coordinates[i];

        out << "}" << endl;
    }
}



///////////////////
// CL_MANAGEMENT //
///////////////////
template <class T>
cl_management<T>::cl_management(exe_args& pars, int init_alg, int assign_alg, int update_alg){
    all_vectors = NULL;

    this->metric = pars.metric;
    this->k = pars.k;
    this->max_updates = pars.max_updates;
    this->L = pars.L;
    this->hf_num = pars.hf;
    this->hc_probes = pars.hc_probes;
    this->hc_M = pars.hc_M;
    this->avg_silhouette = -1.0;
    this->complete = pars.complete;

    /* Create empty clusters */
    for(int i = 0; i < k; i++)
        clusters.push_back(new cluster<T>(i));

    /* Set initiliaze algorithm */
    if(init_alg == 1)
        init_algorithm = new cl_init_random<T>;
    else if(init_alg == 2)
        init_algorithm = new cl_init_kmeans<T>;

    /* Set assign algorithm */
    if(assign_alg == 1)
        assign_algorithm = new cl_assign_lloyd<T>;
    else if(assign_alg == 2)
        assign_algorithm = new cl_assign_lsh<T>;
    else if(assign_alg == 3)
        assign_algorithm = new cl_assign_hc<T>;

    /* Set update algorithm */
    if(update_alg == 1)
        update_algorithm = new cl_update_kmeans<T>;
    else if(update_alg == 2)
        update_algorithm = new cl_update_pam<T>;

    /* Set metric for distance */
    if(metric == 1)
        dist_function = &eucl_distance<T>;
    else if(metric == 2)
        dist_function = &cs_distance<T>;
}

template <class T>
cl_management<T>::~cl_management(){
    /* Destroy dataset */
    if(all_vectors != NULL)
        delete this->all_vectors;

    /* Destroy cluster_info for each vector */
    for(unsigned int i = 0; i < vectors_info.size(); i++)
        delete vectors_info[i];
    vectors_info.clear();

    /* Destroy clusters */
    for(unsigned int i = 0; i < clusters.size(); i++)
        delete clusters[i];
    clusters.clear();

    /* Destroy algorithms */
    if(init_algorithm != NULL)
        delete this->init_algorithm;
    if(assign_algorithm != NULL)
        delete this->assign_algorithm;
    if(update_algorithm != NULL)
        delete this->update_algorithm;
}

template <class T>
void cl_management<T>::clustering(exe_args& parameters, int init, int assign, int upd){
    int i = 0;

    /* Open input file */
    ifstream input(parameters.input_file);
    this->fill_dataset(input);
    cout << "Dataset is created." << endl;

    this->tick();
    cout << "Initializing Clusters..." << endl;
    this->init_clusters();
    cout << "Clusters were Initialized." << endl;

    cout << "Assigning to Clusters for the first time..." << endl;
    this->assign_clusters();
    cout << "Done assigning to Clusters." << endl;

    cout << "Updating - Assigning until convergence..." << endl;
    while(1){
        int made_changes = this->update_clusters();
        this->assign_clusters();
        // cout << "Iteration #" << i++ << ":" << endl;
        if(made_changes == 0 || i >= this->max_updates)
            break;
    }
    this->tock();
    
    cout << "Calculating Silhouette..." << endl;
    this->silhouette();
    cout << "Done Calculating Silhouette." << endl;

    cout << "Printing resutls to file..." << endl;
    cout << "Done Printing to file." << endl;

    cout << "Finished Combination." << endl;
    cout << "-------------------------------------------------------------------------\n" << endl;
}

template <class T>
void cl_management<T>::fill_dataset(ifstream& input){
    string line;
    
    if(this->all_vectors != NULL){
        cout << "Error. Dataset already created" << endl;
        exit(-1);
    }

    if(this->vectors_info.size() != 0){
        cout << "Error. Vectors info already created" << endl;
        exit(-1);
    }


    /* Create new dataset */
    this->all_vectors = new dataset<T>;

    int i = 0;
    /* Scan all vectors in input file, line by line */
    while(getline(input, line)){
        this->all_vectors->add_vector(line);
        this->vectors_info.push_back(new cluster_info(i++));
    }


    /* Check if lsh or hypercube in order to insert all vectors in buckets */
    int assign_alg = assign_algorithm->get_alg_id();
    if(assign_alg > 1){ // lsh or hc
        int n = all_vectors->get_counter();

        /* Initialize lsh */
        if(assign_alg == 2)
            this->assign_algorithm->init_lsh(metric, L, hf_num, n);
        else if(assign_alg == 3)
            this->assign_algorithm->init_hc(metric, hf_num, hc_probes, hc_M);
        
        /* Add to lsh/hc */
        for(int i = 0; i < n; i++)
            assign_algorithm->add_vector(all_vectors->get_item(i));
    }


}

/* Initialize clusters' centroids */
template <class T>
void cl_management<T>::init_clusters(){
    this->init_algorithm->init_clusters(*this);
}

/* Assign vectors to existing clusters */
template <class T>
void cl_management<T>::assign_clusters(){
    this->assign_algorithm->assign_clusters(*this);
}

/* Update centroids of clusters */
template <class T>
int cl_management<T>::update_clusters(){
    return this->update_algorithm->update_clusters(*this);
}

template <class T>
void cl_management<T>::silhouette(){
    double s_total = 0.0; // total silhouette of all vectors */
    int total_vectors = all_vectors->get_counter();

    /* Calculate silhouette of each cluster */
    for(int i = 0; i < k; i++){
        cluster<T>* curr_cluster = clusters[i];
        
        s_total += curr_cluster->evaluation(clusters, dist_function);
    }


    s_total /= (double)total_vectors;

    this->avg_silhouette = s_total;
}

/* Get clustering start time */
template <class T>
void cl_management<T>::tick(){
    cl_start = clock();
}

/* Get clustering end time */
template <class T>
void cl_management<T>::tock(){
    cl_end = clock();
}

/* Get time elapsed during clustering */
template <class T>
double cl_management<T>::get_time_elapsed(){
    double time_elapsed = double(this->cl_end - this->cl_start) / CLOCKS_PER_SEC;
    return time_elapsed;
}

/* Returns pointer to dataset */
template <class T>
dataset<T>* cl_management<T>::get_dataset(){
    return this->all_vectors;
}

/* Returns cluster_info of all vectors container */
template <class T>
vector<cluster_info*>& cl_management<T>::get_vectors_info(){
    return this->vectors_info;
}

/* Returns all clusters */
template <class T>
vector<cluster<T>*>& cl_management<T>::get_clusters(){
    return this->clusters;
}

/* Returns number of k(total clusters) */
template <class T>
int cl_management<T>::get_k(){
    return this->k;
}

/* Returns number of max updates to be done */
template <class T>
int cl_management<T>::get_max_updates(){
    return this->max_updates;;
}

/* Returns the distance function(metric) used */
template <class T>
dist_func cl_management<T>::get_dist_func(){
    return this->dist_function;
}

template <class T>
void cl_management<T>::print(){
    for(unsigned int i = 0; i < clusters.size(); i++)
        clusters[i]->print();
}

template <class T>
void cl_management<T>::print_to_file(ofstream& out){
    int init_num = init_algorithm->get_alg_id();
    int assign_num = assign_algorithm->get_alg_id();
    int upd_num = update_algorithm->get_alg_id();
    out << "------------------------------------------------------------------------------------" << endl;
    out << "Algorithm: I" << init_num << "A" << assign_num << "U" << upd_num << endl;

    if(metric == 1)
        out << "Metric: Euclidean" << endl;
    else
        out << "Metric: Cosine" << endl;
    
    out << endl;

    int num_of_clusters = clusters.size();

    for(int i = 0; i < num_of_clusters; i++){
        clusters[i]->print_to_file(out);
        out << endl;
    }

    out << endl;

    out << "Clustering_time: " << get_time_elapsed() << " seconds" << endl;

    double sil = clusters[0]->get_silhouette();
    out << "Silhouette: [" << sil;
    for(double i = 1; i < num_of_clusters; i++){
        sil = clusters[i]->get_silhouette();
        out << ", " << sil;
    }

    out << ", >> " << this->avg_silhouette << " << ]" << endl;

    out << endl;

    /* Print every vector's id in each cluster */
    if(this->complete == 1){
        for(int i = 0; i < num_of_clusters; i++){
            vector<vector_item<T>*>& vectors = clusters[i]->get_vectors();
            int num_of_vectors = vectors.size();
            out << "CLUSTER-" << clusters[i]->get_cluster_num() << " {";
            
            if(clusters[i]->get_centroid_type() == 1){
                out << (clusters[i]->get_centroid())->get_id();
                for(int j = 0; j < num_of_vectors; j++){
                    out << ", " << vectors[j]->get_id();
                }
            }
            else{
                out << vectors[0]->get_id();
                for(int j = 1; j < num_of_vectors; j++){
                    out << ", " << vectors[j]->get_id();
                }
            }
            
            out << "}" << endl;
            out << endl;
        }
    }
    out << "------------------------------------------------------------------------------------" << endl;
    out << endl;

}

