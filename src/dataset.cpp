/*******************************/
/* dataset.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "dataset.h"

using namespace std;

template class vector_item<int>;
template class vector_item<double>;

template class dataset<int>;
template class dataset<double>;


/*  Implementation of all functions of dataset
 *  and vector_item. Definitions found in
 *  dataset.h.
 */

/////////////////////
//** VECTOR_ITEM **//
/////////////////////
template <class T>
vector_item<T>::vector_item(string& new_vector, int index){
    size_t str_start = 0; // start of each dimension's point(char of string holding point)
    size_t str_end; // end of each dimension's point(char of string holding point) 
    int i = 0; // index for point to be inserted in array

    /* Assign index(position in container) of vector */
    this->index = index;

    /* Reading id of vector */
    str_end = new_vector.find_first_of("\t,", str_start);
    this->item_id = new_vector.substr(str_start, str_end - str_start);
    
    str_start = str_end + 1;

    try {
        /* Extract all points from string and insert in array */ 
        while ((str_end = new_vector.find_first_of("\t ,", str_start)) != string::npos)
        {
            if (str_end > str_start){
                if(i >= D){
                    throw out_of_range("Out_of_range");
                }
                coordinates[i++] = stod(new_vector.substr(str_start, str_end - str_start));
            }

            str_start = str_end + 1; // move to next point
            
        }
        
        if (str_start < new_vector.length())
            coordinates[i++] = stod(new_vector.substr(str_start, string::npos));
    }catch(out_of_range& e2){
        cout << "[Creation of vector_item][line of file: " << index + 1 << " || Id of vector: " << this->item_id << " ] Larger number of dimensions in vector given!" << endl;
        cout << "Aborting..." << endl;
        exit(-1);
    }catch(invalid_argument& e1){
        cout << "[ERROR][line of file: " << index + 1 << " || Id of vector: " << this->item_id << " ] Invalid vector was given. Probably characters were provided. Check your file." << endl;
        cout << "Aborting..." << endl; 
        exit(-1);
    }

    if(i < D){ // check if valid points were given
        cout << "[Creation of vector_item][line of file: " << index + 1 << " || Id of vector: " << this->item_id << " ] Invalid dimensions in vector given! Fewer dimensions were provided" << endl;
        cout << "Aborting..." << endl;
        exit(-1);
    } 
}

/* Create empty vector_item */
template <class T>
vector_item<T>::vector_item(){
    index = -1; // vector is not placed in dataset
    item_id.clear();

    /* Initialize points in all dimensions*/
    for(int i = 0; i < D; i++)
        coordinates[i] = 0;
}

/* Returns 1 if same, else 0 */
template <class T>
int vector_item<T>::is_equal(vector_item<T>& vec){
    /* Check all dimensions */
    for(int i = 0; i < D; i++){
        if(this->coordinates[i] != vec.coordinates[i]) // Different dimension
            return 0; // Not equal vectors
    }

    return 1; // Equal vectors
}


/* Returns item name */
template <class T>
string& vector_item<T>::get_id(){
    return item_id;
}

/* Return points array */
template <class T>
array<T, D>& vector_item<T>::get_points(){
    return coordinates;
}

/* Returns size of vector(number of dimensions) */
template <class T>
int vector_item<T>::get_size(){
    return coordinates.size();
}

/* Returns size of vector(number of dimensions) */
template <class T>
int vector_item<T>::get_index(){
    return this->index;
}

/* Print info about vector_item(Debugging) */
template <class T>
void vector_item<T>::print(){
    cout << "\tI am " << item_id << endl;
    for(int i = 0; i < D; i++)
        cout << "\t" << i << ". " << coordinates[i] << endl;

    cout << "\n" << endl;
}




/////////////
// DATASET //
/////////////

/* Destroy all vector_items */
template <class T>
dataset<T>::~dataset(){
    for(unsigned int i = 0; i < vectors.size(); i++)
        delete vectors[i];
    vectors.clear();
}

/* Push new vector in dataset, increase counter */
template <class T>
void dataset<T>::add_vector(string& new_vector){
    
    /* Push new vector */
    vectors.push_back(new vector_item<T>(new_vector, counter));
    
    /* Increase num of vectors */
    counter++;
}

template <class T>
int dataset<T>::get_counter(){
    return counter;
}

template <class T>
vector_item<T>* dataset<T>::get_item(int index){
    return vectors[index];
}

/* Print info about dataset(Debugging) */
template <class T>
void dataset<T>::print(){
    cout << "[Dataset]: " << counter << endl;
    for(unsigned int i = 0; i < vectors.size(); i++)
        vectors[i]->print();
}
