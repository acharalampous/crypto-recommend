/*******************************/
/* metrics.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <ctime>
#include <random>
#include <cmath>
#include <fstream>

#include "metrics.h"
#include "utils.h"

using namespace std;

// template class euclidean_vec<int>;
template class euclidean_vec<double>;

// template class euclideanHF<int>;
template class euclideanHF<double>;

// template class euclidean<int>;
template class euclidean<double>;

// template class csimilarity<int>;
template class csimilarity<double>;

/*  Implementation of all functions of the metrics
 *  that are used in LSH. Definitions found in
 *  metrics.h.
 */


/////////////////
/** EUCLIDEAN **/
/////////////////

/** euclidean_vec **/
template <class T>
euclidean_vec<T>::euclidean_vec(vector_item<T>* vec, vector<int>* g){
	this->vec = vec;
	this->g = g;
}

template <class T>
euclidean_vec<T>::~euclidean_vec(){
	/* this->vec will be deleted in dataset */
	delete this->g;
}

template <class T>
vector_item<T>& euclidean_vec<T>::get_vec(){
	return *(this->vec);
}

template <class T>
vector<int>& euclidean_vec<T>::get_g(){
	return *(this->g);
}

template <class T>
long int euclidean_vec<T>::get_size(){
	long int total_size = 0;

	/* Get struct size */
	total_size += sizeof(*this);

	/* Get g(p) size */
	total_size += (sizeof(int) * g->size());

	return total_size;
}

template <class T>
void euclidean_vec<T>::print(){
	cout << "I am item in euclidean vec with g: " << endl;
	for(unsigned int i = 0; i < this->g->size(); i++)
		cout << (*g)[i];
	cout << endl;

	this->vec->print();
}



/** euclideanHF **/
template <class T>
euclideanHF<T>::euclideanHF(){
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<float> nd_random(0.0, 1.0);

	/* Generate random vector */
	for(unsigned int i = 0; i < D; i++)
		v[i] = nd_random(gen);

	/* Get random real number t(displacement) */
	uniform_real_distribution<> real_random(0.0, W - 0.000001);

	t = real_random(gen);
}

/* Get value of hash function */
template <class T>
int euclideanHF<T>::getValue(array<T, D>& vec){
	/* Calculation of: floor([ ( p * v ) + t) ] / w) */

	float product = vector_product(v, vec);
	float result = (product + t) / (float)W;

	return (int)result;
}

template <class T>
long int euclideanHF<T>::get_size(){
	long int total_size = 0;
	total_size += sizeof(*this);

	return total_size;
}

template <class T>
void euclideanHF<T>::print(){

	for (unsigned int i = 0; i < D; i++)
		cout << i << ". " << v[i] << endl;

	cout << "T: " << t << endl;
}



/** Euclidean metric **/
template <class T>
euclidean<T>::euclidean(int k, int dataset_sz) : k(k){
	random_device rd;
	mt19937 gen(rd()); // for random vector r

	/* Set metric parameters */
	this->tableSize = dataset_sz / TS_DIVISOR;
	M = pow(2, 32) - 5;

	/* Create empty buckets */
	buckets = new vector<euclidean_vec<T>*>[tableSize];

	/* Create hash functions */
	for(int i = 0; i < k; i++)
		hfs.push_back(euclideanHF<T>());

	/* Generate random vector r */
	uniform_int_distribution<int> rand_Z(MIN_Ri,MAX_Ri); 
	for(int i = 0; i < k; i++)
		r.push_back(rand_Z(gen));
}

template <class T>
euclidean<T>::~euclidean(){
	/* this->vec will be deleted in dataset */
	hfs.clear();
	r.clear();
	for(int i = 0; i < tableSize; i++){
		for(unsigned int j = 0; j < buckets[i].size(); j++){
			delete buckets[i][j];
		}
		buckets[i].clear();
	}

	delete [] buckets;
}

template <class T>
int euclidean<T>::get_bucket_num(vector<int>& hvalues){
	long long int f = 0;

	/* Calculation of: [(r1*h1 + r2*h2 + ... + rk*hk)modM]mod TS */
	for(int i = 0; i < k; i++)
		f += my_mod(hvalues[i] * r[i] ,this->M);

	f = my_mod(f, this->M);
	
	f = f % this->tableSize;

	return f;
}

template <class T>
int euclidean<T>::get_val_hf(array<T, D>& vec, int index){
	return hfs[index].getValue(vec);
}

template <class T>
void euclidean<T>::add_vector(vector_item<T>* new_vector){
	vector<int>* hvalues = new vector<int>; // values returned from hash functions
	int f; // f(p) function <-> bucket index 
	
	/* Get vector points */
	array<T, D>* vec_points = &new_vector->get_points();

	/* Get all hash functions values */
	for(int i = 0; i < k; i++)
		hvalues->push_back(hfs[i].getValue(*vec_points));

	/* Compute f(p) */
	f = get_bucket_num(*hvalues);

	// if(f < 0)
		// cout << "OVERFLOW" << endl;

	buckets[f].push_back(new euclidean_vec<T>(new_vector, hvalues));
}

template <class T>
void euclidean<T>::add_vector(vector_item<T>* new_vec, vector<int>* hvalues, int index){
	buckets[index].push_back(new euclidean_vec<T>(new_vec, hvalues));
}

template <class T>
vector<euclidean_vec<T>*>& euclidean<T>::get_bucket(int index){
	return buckets[index];
}


template <class T>
int euclidean<T>::first_assign(cluster<double>* cl, double& r, unordered_set<string>& checked_set, 
                               vector<vector_check*>& vectors_to_check, 
							   vector<cluster_info*>& vectors_info, int& vectors_left){
	
	int cluster_num = cl->get_cluster_num();
	vector_item<T>* query = cl->get_centroid();
	
	vector<int> hvalues; // values returned from hash functions
	int f; // f(p) function <-> bucket index

	/* First we must find the bucket that corresponds to query */
	/* Get vector points */
	array<T, D>* query_points = &(query->get_points());

	/* Get all hash functions values */
	for(int i = 0; i < k; i++)
		hvalues.push_back(hfs[i].getValue(*query_points));

	/* Compute f(p) */
	f = get_bucket_num(hvalues);

	vector<euclidean_vec<T>*>& buck = get_bucket(f);

	/* Check for items in radius */
	for(unsigned int i = 0; i < buck.size(); i++){
		euclidean_vec<T>* cur_vec = buck[i]; // get current vector

		vector_item<T>& item = cur_vec->get_vec();
		string& item_id = item.get_id(); // get item id
		
		/* Check if item was not checked already */
		if(!in_set(checked_set, item_id)){
			checked_set.insert(item_id);
			if(comp_gs(cur_vec->get_g(), hvalues)){ // check if same g(s)
				double dist = eucl_distance<double>(*query, item);
				
				int item_index = item.get_index();
				/* Check if distance is in range */
				if(dist <= r){

					/* Vector is in radius, must check if its already assigned */
					if(vectors_info[item_index]->get_cluster_num() == -1){ // not assigned
						vectors_info[item_index]->set_cluster(cluster_num);
						vectors_info[item_index]->set_distance(dist);

						vectors_left--;
					}
					else if(dist <= vectors_info[item_index]->get_distance()){ // assigned, check for smaller distance
							vectors_info[item_index]->set_cluster(cluster_num);
							vectors_info[item_index]->set_distance(dist);
					}
				}
				/* If not yet assigned, check for distance, so it can be checked later */ 
				else if(vectors_info[item_index]->get_cluster_num() == -1 || dist <= vectors_info[item_index]->get_distance()){
					vectors_to_check.push_back(new vector_check(&item, dist));
				}
			}
		}
	}

	/* Return vectors left to check */
	return vectors_to_check.size();
}

template <class T>
int euclidean<T>::first_assign(cluster<double>* cl, double& r, hypercube<T>& hc, 
                               vector<vector_check*>& vectors_to_check, 
							   vector<cluster_info*>& vectors_info, int& vectors_left){   
    
	int k = get_k(); // get number of hash functions
	int cluster_num = cl->get_cluster_num();
	vector_item<T>* query = cl->get_centroid();
	int probes = hc.get_probes();

	array<T, D>& vec = query->get_points();
        
	/* Find bucket num of query */
	int bucket_num = 0;
	for(int i = 0; i < k; i++){
		/* Get hash function value */
		int fi = get_val_hf(vec, i);

		/* Get 1 or 0 value from map */
		fi = hc.check_map(fi, i);

		/* Find bucket */
		bucket_num += fi * pow(2, k - i - 1);
	}

	/* Find number of neighbours that need to be checked, according to probes */
	vector<int>* neighbours = hc.find_neighbours(bucket_num, probes);

	int vector_sz = get_k() + 1; // size of vector
	
	int remaining_probes = probes; // number of neighbours left to check
	int remaining_items = hc.get_M(); // number of items left to check
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
			vector<euclidean_vec<T>*>& buck = get_bucket(f);

			for(unsigned int i = 0; i < buck.size(); i++){
				remaining_items--; // about to check another vector
				if(remaining_items < 0){ // no more items to check
					flag = 1;    
					break;
				} 
				
				euclidean_vec<T>* cur_vec = buck[i]; // get current vector
				vector_item<T>& item = cur_vec->get_vec();

				double dist = eucl_distance(*query, item);

				int item_index = item.get_index();
				/* Check if distance is in range */
				if(dist <= r){

					/* Vector is in radius, must check if its already assigned */
					if(vectors_info[item_index]->get_cluster_num() == -1){ // not assigned
						vectors_info[item_index]->set_cluster(cluster_num);
						vectors_info[item_index]->set_distance(dist);

						vectors_left--;
					}
					else if(dist <= vectors_info[item_index]->get_distance()){ // assigned, check for smaller distance
							vectors_info[item_index]->set_cluster(cluster_num);
							vectors_info[item_index]->set_distance(dist);
					}
				}

				/* If not yet assigned, check for distance, so it can be checked later */ 
				else if(vectors_info[item_index]->get_cluster_num() == -1 || dist <= vectors_info[item_index]->get_distance()){
					vectors_to_check.push_back(new vector_check(&item, dist));
				}
			} // end for all items in buckets
		} // end for neighbours in current distance
	}// end for neighbours in probe
	delete [] neighbours;

	return vectors_to_check.size();
}

template <class T>
void euclidean<T>::findANN(vector_item<T>& query, float radius, float& min_dist, string& NN_name, ofstream& output, unordered_set<string>& checked_set){
	vector<int> hvalues; // values returned from hash functions
	int f; // f(p) function <-> bucket index

	/* First we must find the bucket that corresponds to query */
	/* Get vector points */
	array<T, D>* query_points = &(query.get_points());

	/* Get all hash functions values */
	for(int i = 0; i < k; i++)
		hvalues.push_back(hfs[i].getValue(*query_points));

	/* Compute f(p) */
	f = get_bucket_num(hvalues);

	vector<euclidean_vec<T>*>& buck = get_bucket(f);

	/* If radius was given as 0, find only the nearest neighbour */
	if(radius == 0){
		for(unsigned int i = 0; i < buck.size(); i++){
			euclidean_vec<T>* cur_vec = buck[i]; // get current vector

			vector_item<T>& item = cur_vec->get_vec();

			string& item_id = item.get_id(); // get item id

			/* Check if item was not checked already */
			if(!in_set(checked_set, item_id)){
				checked_set.insert(item_id);
				if(comp_gs(cur_vec->get_g(), hvalues)){ // check if same g 				
					float dist = eucl_distance(query, item);

					/* Check if nearest neighbour */
					if(dist <= min_dist || min_dist == 0.0){
						min_dist = dist;
						NN_name.assign(item_id);
					}
				}
			}
		}
	}


	/* Else check for items in radius */
	else{
		for(unsigned int i = 0; i < buck.size(); i++){
			euclidean_vec<T>* cur_vec = buck[i]; // get current vector

			vector_item<T>& item = cur_vec->get_vec();
			string& item_id = item.get_id(); // get item id

			/* Check if item was not checked already */
			if(!in_set(checked_set, item_id)){
				checked_set.insert(item_id);
				if(comp_gs(cur_vec->get_g(), hvalues)){ // check if same 
					float dist = eucl_distance(query, item);

					/* Print item in radius of query */
					if(dist <= radius){
						output << "\t" << item_id << endl;
					}

					/* Check if nearest neighbour */
					if(dist <= min_dist || min_dist == 0.0){
						min_dist = dist;
						NN_name.assign(item_id);
					}
				}
			}
		}
	}

}

template <class T>
int euclidean<T>::comp_gs(vector<int>& g1, vector<int>& g2){
	/* Check if same dimensions */
	if(g1.size() != g2.size())
		return 0;

	/* Check all values */
	for(unsigned int i = 0; i < g1.size(); i++){
		if(g1[i] != g2[i])
			return 0;
	}

	/* Reaching this point means that vectors(gs) are the same */
	return 1;
}

template <class T>
int euclidean<T>::get_k(){
	return this->k;
}

template <class T>
long int euclidean<T>::get_size(){
	long int total_size = 0;
	
	/* Get struct size */
	total_size += sizeof(*this);

	/* Get hash functions structs size */
	for(int i = 0; i < k; i++){
		total_size += hfs[i].get_size();
	}

	/* Calculate buckets size */
	for(int i = 0; i < tableSize; i++){
		total_size += sizeof(buckets[i]); 
		for(unsigned int j = 0; j < buckets[i].size(); j++){
			total_size += buckets[i][j]->get_size();
		}
	}

	/* Get r(random vector) size */
	total_size += sizeof(int) * r.size();

	return total_size;
}




/////////////////////////
/** COSINE SIMILARITY **/
/////////////////////////
template <class T>
csimilarityHF<T>::csimilarityHF(){
	random_device rd;

	mt19937 gen(rd());

	/* Get random vector */
	for(unsigned int i = 0; i < D; i++){
		normal_distribution<float> nd_random(0.0, 1.0);
		
		r[i] = nd_random(gen);
	}


}
/* Get value of hash function */
template <class T>
int csimilarityHF<T>::getValue(array<T, D>& vec){
	
	float product = vector_product(r, vec);
	
	/* if product > 0 return 1, else return 0 */
	return (product >= 0)? 1 : 0;

}

template <class T>
long int csimilarityHF<T>::get_size(){
	long int total_size = 0;
	total_size += sizeof(*this);

	return total_size;
}

template <class T>
void csimilarityHF<T>::print(){
	for (unsigned int i = 0; i < D; i++)
		cout << i << ". " << r[i] << endl;
}



/* csimilarity */
template <class T>
csimilarity<T>::csimilarity(int k){
	this->k = k;
	this->tableSize = pow(2, k);

	buckets = new vector<vector_item<T>*>[tableSize];

	for(int i = 0; i < k; i++)
		hfs.push_back(csimilarityHF<T>());
}

template <class T>
csimilarity<T>::~csimilarity(){
	/* this->vec will be deleted in dataset */
	hfs.clear();

	delete [] buckets;
}

template <class T>
void csimilarity<T>::add_vector(vector_item<T>* new_vector){
	int f = 0; // f(p) function <-> bucket index 
	
	/* Get vector points */
	array<T, D>* vec_points = &new_vector->get_points();

	/* Get all hash functions values */
	for(int i = 0; i < k; i++){
		int temp = hfs[i].getValue(*vec_points);
		f += temp * pow(2, k - i - 1); // compute binary value
	}

	buckets[f].push_back(new_vector);
}

template <class T>
vector<vector_item<T>*>& csimilarity<T>::get_bucket(int index){
	return buckets[index];
}

template <class T>
int csimilarity<T>::first_assign(cluster<double>* cl, double& r, unordered_set<string>& checked_set, 
                               vector<vector_check*>& vectors_to_check, 
							   vector<cluster_info*>& vectors_info, int& vectors_left){
	
	int f = 0; // f(p) function <-> bucket index
	int cluster_num = cl->get_cluster_num();
	vector_item<T>* query = cl->get_centroid();
	
	/* First we must find the bucket that corresponds to query */
	/* Get vector points */
	array<T, D>* query_points = &(query->get_points());

	for(int i = 0; i < k; i++){
		int temp = hfs[i].getValue(*query_points);
		f += temp * pow(2, k - i - 1); // compute binary value
	}

	vector<vector_item<T>*>& buck = get_bucket(f);

	/* Check for items in radius */
	for(unsigned int i = 0; i < buck.size(); i++){
		vector_item<T>& item = *buck[i];
		string& item_id = item.get_id(); // get item id
		
		/* Check if item was not checked already */
		if(!in_set(checked_set, item_id)){
			checked_set.insert(item_id);
			double dist = cs_distance<double>(*query, item);
			
			int item_index = item.get_index();
			/* Check if distance is in range */
			if(dist <= r){

				/* Vector is in radius, must check if its already assigned */
				if(vectors_info[item_index]->get_cluster_num() == -1){ // not assigned
					vectors_info[item_index]->set_cluster(cluster_num);
					vectors_info[item_index]->set_distance(dist);

					vectors_left--;
				}
				else if(dist <= vectors_info[item_index]->get_distance()){ // assigned, check for smaller distance
						vectors_info[item_index]->set_cluster(cluster_num);
						vectors_info[item_index]->set_distance(dist);
				}
			}
			/* If not yet assigned, check for distance, so it can be checked later */ 
			else if(vectors_info[item_index]->get_cluster_num() == -1 || dist <= vectors_info[item_index]->get_distance()){
				vectors_to_check.push_back(new vector_check(&item, dist));
			}
		}
	}

	/* Return vectors left to check */
	return vectors_to_check.size();
}

template <class T>
int csimilarity<T>::first_assign(cluster<double>* cl, double& r, hypercube<T>& hc, 
                               vector<vector_check*>& vectors_to_check, 
							   vector<cluster_info*>& vectors_info, int& vectors_left){   
    
	int k = get_k(); // get number of hash functions
	int cluster_num = cl->get_cluster_num();
	vector_item<T>* query = cl->get_centroid();
	int probes = hc.get_probes();
	int f = 0;

	array<T, D>& vec = query->get_points();
        
	/* Get all hash functions values */
	for(int i = 0; i < k; i++){
		int temp = this->get_val_hf(vec, i);
		f += temp * pow(2, k - i - 1); // compute binary value
	}

	/* Find number of neighbours that need to be checked, according to probes */
	vector<int>* neighbours = hc.find_neighbours(f, probes);

	int vector_sz = get_k() + 1; // size of vector
	
	int remaining_probes = probes; // number of neighbours left to check
	int remaining_items = hc.get_M(); // number of items left to check
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

			/* Get bucket from cosine table */
			int f = neighbours[i][j];
			vector<vector_item<T>*>& buck = get_bucket(f);

			for(unsigned int k = 0; k < buck.size(); k++){
				remaining_items--; // about to check another vector
				if(remaining_items < 0){ // no more items to check
					flag = 1;    
					break;
				} 
				
				vector_item<T>* item = buck[k]; // get current vector

				double dist = cs_distance(*query, *item);

				int item_index = item->get_index();
				
				/* Check if distance is in range */
				if(dist <= r){

					/* Vector is in radius, must check if its already assigned */
					if(vectors_info[item_index]->get_cluster_num() == -1){ // not assigned
						vectors_info[item_index]->set_cluster(cluster_num);
						vectors_info[item_index]->set_distance(dist);

						vectors_left--;
					}
					else if(dist <= vectors_info[item_index]->get_distance()){ // assigned, check for smaller distance
							vectors_info[item_index]->set_cluster(cluster_num);
							vectors_info[item_index]->set_distance(dist);
					}
				}

				/* If not yet assigned, check for distance, so it can be checked later */ 
				else if(vectors_info[item_index]->get_cluster_num() == -1 || dist <= vectors_info[item_index]->get_distance()){
					vectors_to_check.push_back(new vector_check(item, dist));
				}
			} // end for all items in buckets
		} // end for neighbours in current distance
	}// end for neighbours in probe
	delete [] neighbours;

	return vectors_to_check.size();
}

template <class T>
void csimilarity<T>::findANN(vector_item<T>& query, float radius, float& min_dist, string& NN_name, ofstream& output, unordered_set<string>& checked_set){
	int f = 0; // f(p) function <-> bucket index

	/* First we must find the bucket that corresponds to query */
	/* Get vector points */
	array<T, D>* query_points = &(query.get_points());

	for(int i = 0; i < k; i++){
		int temp = hfs[i].getValue(*query_points);
		f += temp * pow(2, k - i - 1); // compute binary value
	}

	vector<vector_item<T>*>& buck = get_bucket(f);

	/* If radius was given as 0, find only the nearest neighbour */
	if(radius == 0){
		for(unsigned int i = 0; i < buck.size(); i++){
			vector_item<T>* cur_vec = buck[i]; // get current vector
			string& item_id = cur_vec->get_id(); // get item id

			if(!in_set(checked_set, item_id)){
				checked_set.insert(item_id);

				float dist = cs_distance(query, *cur_vec);

				/* Check if nearest neighbour */
				if(dist <= min_dist || min_dist == 0.0){
					min_dist = dist;
					NN_name.assign(item_id);
				}
			}
		}
	}


	/* Else check for items in radius */
	else{
		for(unsigned int i = 0; i < buck.size(); i++){
			vector_item<T>* cur_vec = buck[i]; // get current vector
			string& item_id = cur_vec->get_id();

			if(!in_set(checked_set, item_id)){
				checked_set.insert(item_id);
				float dist = cs_distance(query, *cur_vec);

				if(dist <= radius)
					output << "\t" << item_id << endl;

				/* Check if nearest neighbour */
				if(dist <= min_dist || min_dist == 0.0){
					min_dist = dist;
					NN_name.assign(item_id);
				}
			}
		}
	}
}


template <class T>
int csimilarity<T>::get_val_hf(array<T, D>& vec, int index){
	return hfs[index].getValue(vec);
}

template <class T>
int csimilarity<T>::get_k(){
	return this->k;
}

template <class T>
long int csimilarity<T>::get_size(){
	long int total_size = 0;
	
	/* Get struct size */
	total_size += sizeof(*this);

	/* Get hash functions structs size */
	for(int i = 0; i < k; i++){
		total_size += hfs[i].get_size();
	}

	/* Calculate buckets size */
	for(int i = 0; i < tableSize; i++){
		total_size += sizeof(buckets[i]);
		for(unsigned int j = 0; j < buckets[i].size(); j++){
			total_size += sizeof(vector_item<T>*);
		}
	}

	return total_size;
}