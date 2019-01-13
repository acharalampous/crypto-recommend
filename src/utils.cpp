/*******************************/
/* utils.cpp */

/* Name:    Andreas Charalampous
 * A.M :    1115201500195
 * e-mail:  sdi1500195@di.uoa.gr
 */
/********************************/
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sys/stat.h>

#include "utils.h"
#include "metrics.h"

using namespace std;

template float vector_product(vector<float>&, vector<int>&);
template float vector_product(vector<float>&, vector<double>&);
template double eucl_distance(vector_item<double>&, vector_item<double>&);
template double cs_distance(vector_item<double>&, vector_item<double>&);
template int get_second_best(vector_item<double>&, int, vector<cluster<double>*>&, dist_func&);
template double calculate_b(vector_item<double>&, cluster<double>*, dist_func&);
template float exchausting_s(dataset<int>&, vector_item<int>&, int);


/*  All functions implementions that are defined in utils.h */

/* Initialize struct with default values */
exe_args::exe_args(){
    init = 1;
    assign = 1;
    upd = 1;
    metric = 1;
    k = -1;
    max_updates = 30;
    L = DEFAULT_L;
    hf = DEFAULT_K;
    hc_probes = HC_DEFAULT_PROBES;
    hc_M = HC_DEFAULT_M;
    input_file = "";
    config_file = CONF_FILE;
}

int get_parameters(int argc, char** argv, exe_args& pars){
    // for (int i = 1; i < argc; i += 2){ // get all parameters
            
    //     string par(argv[i]); // get parameter
    //     if(par.compare("-i") == 0){ // input file provided
    //         if(!pars.input_file.empty()){ // input file(dataset) is provided twice
    //             printf("Error in parameters! Input file [-i] is given more than once! Abort.\n");
    //             return -2;
    //         }

    //         pars.input_file = argv[i + 1];
        
    //     } // end if -i
    //     else if(par.compare("-c") == 0){ // configuration file parameter
    //         if(!pars.config_file.empty()){ // port given twice
    //             printf("Error in parameters! Configuration file [-c] is given more than once! Abort.\n");
    //             return -2;
    //         }
            
    //             pars.config_file = argv[i + 1];
    //     } // end if -c
    //     else if(par.compare("-o") == 0){ // output file parameter
    //         if(!pars.output_file.empty()){ // output file twice
    //             printf("Error in parameters! Output file [-o] is given more than once! Abort.\n");
    //             return -2;
    //         }

    //         pars.output_file = argv[i + 1];
    //     } // end if -o
    //     else if(par.compare("-d") == 0) { // metric parameter
    //         if(!isNumber(argv[i + 1])){ // metric parameter given is not a number
    //             printf("Error in parameters! Metric [-d] given is not a valid number. Should be 1 or 2. Abort\n");
    //             return -2;
    //         }

    //         pars.metric = atoi(argv[i + 1]);
    //         if(pars.metric != 1 && pars.metric != 2){ // invalid number
    //             printf("Error in parameters! Metric [-d] given is not a valid number. Should be 1 or 2. Abort\n");
    //             return -2;
    //         }
    //     } // end if -d
    //     else if(par.compare("-complete") == 0){ // complete parameter
    //         pars.complete = 1;
    //         i--;
    //     } // end if -complete
    //     else if(par.compare("-1c") == 0){ // only one combination
    //         pars.all_combinations = 0;
    //         i--;
    //     }
    //     else{
    //         printf("Error in parameters. Unknown parameter given [%s]. Abort.\n", argv[i]);
    //         return -2;
    //     } // end switch
    // } // end for
    return 0;
}

int validate_parameters(exe_args& pars){
    
    /* Get configuration file */
    while(1){ // until correct file is given
            
        /* Check if conf file was provided by parameters */
        if(pars.config_file.empty()){
            cout << "Please provide path to config file, or .. to abort: ";
            fflush(stdout);
            getline(cin, pars.config_file);
            fflush(stdin);
        }

        /* Abort */
        if(!pars.config_file.compare("..")){ 
            cout << "No file was given. Abort." << endl;
            return -1;
        }

        /* Check if file exists */
        struct stat buffer;
        if(stat (pars.config_file.c_str(), &buffer) != 0){
            cout << "File " << pars.config_file << " does not exist. Try again." << endl;
            pars.config_file = "";
            continue;
        }
        
        ifstream conf;
        conf.open(pars.config_file); // open file provided 
        if(conf.is_open()){ // file was succesfully opened
            /* read configuration and get all parameters from it */
            int res = read_config_file(conf, pars);
            conf.close();
            /* Check if valid file */
            if(res == -1){
                cout << "Invalid Configuration file! Number of clusters was not provided. Abort" << endl;
                return -3;
            }
            break;
        }
    }

    /* Get input file */
    while(1){ // until correct input file is given

        /* Check if input file was provided by parameters */
        /* Also in case of re-execution, check if user wants another file to be used */
        if(pars.input_file.empty()){
            cout << "Please provide path to input file(dataset), or .. to abort: ";
            fflush(stdout);
            getline(cin, pars.input_file);
            fflush(stdin);
        }

        /* Abort */
        if(!pars.input_file.compare("..")){ 
            cout << "No file was given. Abort." << endl;
            return -1;
        }

        /* Check if file exists */
        struct stat buffer;
        if(stat (pars.input_file.c_str(), &buffer) != 0){
            cout << "File " << pars.input_file << " does not exist. Try again." << endl;
            pars.input_file = "";
            continue;
        }
        else{
            break;
        }
    }


    /* Validity check */

    /* Check init algorithm */
    if(pars.init < 1 && pars.init > 2){
        cout << "Invalid initialization algorithm provided! Abort." << endl;
        return -3;
    }
    
    /* Check assign algorithm */
    if(pars.assign < 1 && pars.assign > 3){
        cout << "Invalid assign algorithm provided! Abort." << endl;
        return -3;
    }
    
    /* Check upd algorithm */
    if(pars.upd < 1 && pars.upd > 2){
        cout << "Invalid update algorithm provided! Abort." << endl;
        return -3;
    }
    

    /* Check metric */
    if(pars.metric != 1 && pars.metric != 2){
        cout << "Invalid metric provided! Abort." << endl;
        return -3;
    }

    /* Check number of cluster */
    if(pars.k < 2){
        cout << "Invalid number of clusters provided. Abort." << endl;
        return -3;
    }

    /* Check number of hash function */
    if(pars.hf < 1){
        cout << "Invalid number of hash functions provided. Abort." << endl;
        return -3;
    }

    /* Check number of hash Tables */
    if(pars.L < 1){
        cout << "Invalid number of hash tables provided. Abort." << endl;
        return -3;
    }

    /* Check number of max updates */
    if(pars.max_updates < 1){
        cout << "Invalid number of max updates provided. Abort." << endl;
        return -3;
    }

    /* Check number of probes */
    if(pars.hc_probes < 1){
        cout << "Invalid number of hypercube probes provided. Abort." << endl;
        return -3;
    }

    /* Check number for M for hc */
    if(pars.hc_M < 1){
        cout << "Invalid number of hypercube M provided. Abort." << endl;
        return -3;
    }

    return 0;

}

int read_config_file(ifstream& conf_file, exe_args& pars){
    string line;
    int flag = -1; // check if neccessary(no defaults) were provided
    while(getline(conf_file, line)){
        if(line.compare(0, 19, "number_of_clusters:") == 0){ // k
            string par = line.substr(19, line.length() - 19);
            pars.k = stoi(par);
            flag = 1;        
        }
        else if(line.compare(0, 25, "number_of_hash_functions:") == 0){ // hf
            string par = line.substr(25, line.length() - 25);
            pars.hf = stoi(par);
        }
        else if(line.compare(0, 22, "number_of_hash_tables:") == 0){ // L
            string par = line.substr(22, line.length() - 22);
            pars.L = stoi(par);
        }
        else if(line.compare(0, 12, "max_updates:") == 0){ // max_updates
            string par = line.substr(12, line.length() - 12);
            pars.max_updates = stoi(par);

        }
        else if(line.compare(0, 10, "hc_probes:") == 0){ // probes
            string par = line.substr(10, line.length() - 10);
            pars.hc_probes = stoi(par);
        }
        else if(line.compare(0, 5, "hc_M:") == 0){ // M
            string par = line.substr(5, line.length() - 5);
            pars.hc_M = stoi(par);
        }
        else if(line.compare(0, 7,"metric:") == 0){ // metric
            string par = line.substr(7, line.length() - 7);
            pars.metric = stoi(par);
        }
        else if(line.compare(0, 5,"init:") == 0){ // init algorithm
            string par = line.substr(5, line.length() - 5);
            pars.init = stoi(par);
        }
        else if(line.compare(0, 7,"assign:") == 0){ // init algorithm
            string par = line.substr(7, line.length() - 7);
            pars.assign = stoi(par);
        }
        else if(line.compare(0, 4,"upd:") == 0){ // init algorithm
            string par = line.substr(4, line.length() - 4);
            pars.upd = stoi(par);
        }
        else if(line.compare(0, 6, "input:") == 0){ // input file
            pars.input_file = line.substr(6, line.length() - 6);
        }
    }

    return flag;
}

void printValidParameters(){
    cout << "\n*Execute again providing (optionally) the following parameters:" << endl;
    cout << "\t-i inputFile" << endl;
    cout << "\t-c configFile" << endl;
    cout << "\t-o outputFile" << endl;
    cout << "\t-d metric" << endl;
    cout << "\t-complete" << endl;
    cout << "-inputFile: Path to the dataset file" << endl;
    cout << "-configFile: Path to the config file" << endl;
    cout << "-outputFile: Path to the output file" << endl;
    cout << "-metric: 1 for euclidean or 2 for cosine" << endl;
    cout << "-complete: Print full info for clusters" << endl;
}

void printValidConfig(){
    cout << "\n*Execute again providing a config file with the following options. * are mandatory:" << endl;
    cout << "*number_of_clusters:<int>[>= 2]" << endl;
    cout << "number_of_hash_functions:<int>[>=1" << endl;
    cout << "number_of_hash_tables:<int>[>=1]" << endl;
    cout << "max_updates:<int>[>=1]" << endl;
    cout << "hc_probes:<int>[>=1]" << endl;
    cout << "hc_M:<int>[>=1]" << endl;
}

int read_combination(int& init, int& assign, int& update){
    string choice;

    /* Assignment algorithm */
    while(1){
        cout << "\nPlease choose Initialization Algorithm: " << endl;
        cout << "\t 1 Random selection of k points from dataset" << endl;
        cout << "\t 2 K-means++" << endl;
        cout << "\t A Run all combinations" << endl;
        cout << "\t X Abort program" << endl;
        cout << "\nMy choice: ";
        fflush(stdout);
        
        getline(cin, choice);
        fflush(stdin);

        if(!choice.compare("1")){ // Random Selection
            init = 1;
            break;
        }
        else if(!choice.compare("2")){ // K-means++
            init = 2;
            break;
        }
        else if(!choice.compare("A")){ // All Combinations
            return 1;
        }
        else if(!choice.compare("X")){ // Exit
            return 2;
        }
        else
            cout << "Invalid input given. Try again." << endl;
    }
    while(1){
        cout << "\nPlease choose Assignment Algorithm: " << endl;
        cout << "\t 1 Lloyd's Assignment" << endl;
        cout << "\t 2 LSH Range Search Assignment" << endl;
        cout << "\t 3 Hypercube Range Search Assignment" << endl;
        cout << "\t A Run all combinations" << endl;
        cout << "\t X Abort program" << endl;
        cout << "\nMy choice: ";        
        fflush(stdout);
        
        getline(cin, choice);
        fflush(stdin);

        if(!choice.compare("1")){ // LLoyd's Assignment
            assign = 1;
            break;
        }
        else if(!choice.compare("2")){ // LSH
            assign = 2;
            break;
        }
        else if(!choice.compare("3")){ // Hypercube
            assign = 3;
            break;
        }
        else if(!choice.compare("A")){ // All Combinations
            return 1;
        }
        else if(!choice.compare("X")){ // Exit
            return 2;
        }
        else
            cout << "Invalid input given. Try again." << endl;
    }
    while(1){
        cout << "\nPlease choose Update Algorithm: " << endl;
        cout << "\t 1 K-means" << endl;
        cout << "\t 2 Partinioning Around Medoids(PAM)" << endl;
        cout << "\t A Run all combinations" << endl;
        cout << "\t X Abort program" << endl;
        cout << "\nMy choice: ";
        fflush(stdout);
        
        getline(cin, choice);
        fflush(stdin);

        if(!choice.compare("1")){ // Random Selection
            update = 1;
            break;
        }
        else if(!choice.compare("2")){ // K-means++
            update = 2;
            break;
        }
        else if(!choice.compare("A")){ // All Combinations
            return 1;
        }
        else if(!choice.compare("X")){ // Exit
            return 2;
        }
        else
            cout << "Invalid input given. Try again." << endl;
    }

    return 0;
}

void print_exe_details(exe_args& pars){
    cout << "\nExecution details:" << endl;
    cout << "------------------" << endl;
    
    /* Print clusters */
    cout << "  *Number of Clusters to be created: " << pars.k << endl;

    /* Print metric */
    if(pars.metric == 1)
        cout << "  *Metric: Euclidean" << endl;
    else if(pars.metric == 2)
        cout << "  *Metric: Cosine" << endl;

    cout << "  *Maximum number of updates: " << pars.max_updates << endl;    

    /* Print init algorithm */
    if(pars.init == 1)
        cout << "  *Initialization Algorithm: (1)Random selection of k points from dataset." << endl;
    else if(pars.init == 2)
        cout << "  *Initialization Algorithm: (2)K-means++." << endl;
    
    /* Print assign algorithm */
    if(pars.assign == 1)
        cout << "  *Assignment Algorithm: (1)Lloyd's Assignment." << endl;
    if(pars.assign == 2){
        cout << "  *Assignment Algorithm: (2)LSH Range Search." << endl;
        cout << "    **Number of Hash Tables: " << pars.L << endl;
        cout << "    **Number of Hash Functions: " << pars.hf << endl;
    }
    if(pars.assign == 3){
        cout << "  *Assignment Algorithm: (1)Hypercube Range Search." << endl;
        cout << "    **Number of Hash Functions: " << pars.hf << endl;
        cout << "    **Number of probes: " << pars.hc_probes << endl;
        cout << "    **Number of M: " << pars.hc_M << endl;
    }

    /* Print Update algorithm */
    if(pars.upd == 1)
        cout << "  *Update Algorithm: (1)K-means." << endl;
    else if(pars.upd == 2)
        cout << "  *Update Algorithm: (2)Partioning around medoids(PAM)." << endl;
    
    cout << "  *Input file: " << pars.input_file << endl;
    cout << "  *Config file: " << pars.config_file << endl;
         
    cout << endl;
}


int isNumber(char* str){
    char* temp = str;
    char ch = *temp;
    if(ch == '-') // negative number is provided
        ch = *(++temp);
    while(ch != '\0'){
        if(ch >= 48 && ch <= 57){
            ch = *(++temp);
        }
        else{
            return 0;
        }
    }
    return 1;
}

int get_int_len(int num){
    int num_digits = 1; // digits of number
    while(num >= 0){	
		num_digits++;
       	num = num >> 1;
	}

    return num_digits;
}

int get_combinations(int a, int b){
    /* Computes comb(a,b) = a! / (b! * (a - b)!) */
    int diff = a - b;

    return ( get_factorial(a) / ( get_factorial(b) * get_factorial(diff)) );


}

int get_factorial(int num){
    int total = 1;
    for(int i = 2; i <= num; i++){
        total *= i;
    }

    return total;
}

int hamming_dist(int x, int y){
    int x_or = x ^ y; // keep different bits, they will be represented as 1

    int dist = 0; // hamming_distance
    while(x_or > 0){ // check all bits
        if((x_or | 1) == x_or) // if true, means the leftmost bit is 1, hence different
            dist++;
        x_or = x_or >> 1;
    }
    return dist;
}

/* Compute inner product of two vectors */
template <class T>
float vector_product(vector<float>& vec1, vector<T>& vec2){
    float product = 0.0;

    for(int i = 0; i < D; i++)
        product += vec1[i] * vec2[i];

    return product;
}

long long int h_concantenate(vector<int>& hs){
    /* Creates a string with the concatenated ints, and then it is */
    /* returned converted back to int.                             */

    string res_str("");
    for(unsigned int i = 0; i < hs.size(); i++)
        res_str += to_string(abs(hs[i]));


    return stoll(res_str);
}

/* Computes and returns the modulo of the given values a mod b */
long long int my_mod(int a, long int b){
	long long int res = a - ((floor( (long double)a / (long double)b) ) * b); 
    return res;
}

int get_metrics(string& metrics){
    if(metrics.empty()) // no definition, use euclidean
        return 0;

    if(metrics.find("euclidean") != string::npos) // euclidean is defined
        return 1;
    
    if(metrics.find("cosine") != string::npos) // cosine is defined
        return 2;

    return 0; // nothing was defined, use euclidean
}

double get_radius(std::string& radius){
    if(radius.compare(0, 9, "Radius: <")) // check if radius is given
        return -1;

    /* Extract radius */
    int pos1 = radius.find('<');
    int pos2 = radius.find('>');

    /* Keep the part enclosed in <> */
    string temp = radius.substr(pos1 + 1, pos2 - pos1 - 1);
    
    double result = 0.0;
    try{
        result = stod(temp);
    }catch(invalid_argument){
        cout << "Invalid radius was given. Radius won't be used at all!" << endl;
        return 0.0;
    }

    return result;
}

template <class T>
double eucl_distance(vector_item<T>& vec1, vector_item<T>& vec2){
	/* Compute sqrt((Sum((ai - bi)^2))), for all points */

    if(vec1.get_size() != vec2.get_size()){
		cout << "Invalid dimensions of vectors" << endl;
		exit(0);
	}

	double dist = 0.0;

	vector<T>& arr1 = vec1.get_points();
	vector<T>& arr2 = vec2.get_points();


	for(unsigned int i = 0; i < arr1.size(); i++)
		dist += pow(arr1[i] - arr2[i], 2);

	dist = sqrt(dist);

	return dist;
}

template <class T>
double cs_distance(vector_item<T>& vec1, vector_item<T>& vec2){
	/* Compute (1 - ((vec1 * vec2) / (||vec1||*||vec2||))) */
 	if(vec1.get_size() != vec2.get_size()){
		cout << "Invalid dimensions" << endl;
		exit(0);
	}

	double dist;
	double euc_dist = 0.0;
	double arr1_norm = 0.0;
	double arr2_norm = 0.0;

	vector<T>& arr1 = vec1.get_points();
	vector<T>& arr2 = vec2.get_points();


	for(unsigned int i = 0; i < arr1.size(); i++){
		euc_dist += (arr1[i] * arr2[i]); // vec1 & vec2
		arr1_norm += arr1[i] * arr1[i]; // ||vec1||
		arr2_norm += arr2[i] * arr2[i]; // ||vec2||
	}

	arr1_norm = sqrt(arr1_norm);
	arr2_norm = sqrt(arr2_norm);

	dist = euc_dist / (arr1_norm * arr2_norm);
	dist = 1 - dist;

	return dist;
}

double cs_similarity(user& usr1, user& usr2){
	/* Compute ( (vec1 * vec2) / (||vec1||*||vec2||) ) */
 	vector<double>& arr1 = usr1.get_sentiments();
    vector<double>& arr2 = usr2.get_sentiments();
     
    if(arr1.size() != arr2.size()){
		cout << "Invalid dimensions" << endl;
		exit(0);
	}

	double similarity;
	double euc_dist = 0.0;
	double arr1_norm = 0.0;
	double arr2_norm = 0.0;

	for(unsigned int i = 0; i < arr1.size(); i++){
		euc_dist += (arr1[i] * arr2[i]); // vec1 & vec2
		arr1_norm += arr1[i] * arr1[i]; // ||vec1||
		arr2_norm += arr2[i] * arr2[i]; // ||vec2||
	}

	arr1_norm = sqrt(arr1_norm);
	arr2_norm = sqrt(arr2_norm);

	similarity = euc_dist / (arr1_norm * arr2_norm);

	return similarity;
}

template <class T>
int get_second_best(vector_item<T>& query, int cl_num, vector<cluster<T>*>& clusters, dist_func& dist){
    int sec_best = cl_num; // index of the second best cluster for query to be placed
    double min_distance; // distance to second best
    int k = clusters.size();

    int flag = 0; // min_distance was initialized

    /* Check for all clusters */
    for(int i = 0; i < k; i++){
        if(i == cl_num) // if same cluster, skip
            continue;
        
        double curr_dist = dist(query, *(clusters[i]->get_centroid()));
        if(flag == 0){ // no minimum distance yet, so keep current distance
            sec_best = i;
            min_distance = curr_dist;
            flag = 1;
        }
        else{
            if(curr_dist <= min_distance){ // keep if less
                sec_best = i;
                min_distance = curr_dist;
            }
        }
    } // end for all clusters

    return sec_best;
}

template <class T>
double calculate_b(vector_item<T>& query, cluster<T>* cl, dist_func& dist){
    int num_of_vectors = cl->get_size();
    double b_value = 0.0;

    /* Calculate sum of distances of all vectors in cluster given */
    for(int i = 0; i < num_of_vectors; i++)
        b_value += dist(query, *(cl->get_vector(i)));

    if(cl->get_centroid_type() == 1){
        num_of_vectors++;

        b_value += dist(query, *(cl->get_centroid()));
    }

    if(num_of_vectors == 0)
        return 0.0;

    /* Get average distance and return */
    b_value = b_value / (double)num_of_vectors;
    
    return b_value;
}

double get_starting_r(vector<cluster<double>*>& clusters, dist_func& dist){
    int num_of_clusters = clusters.size(); // get number of clusters
    double min_distance = 0.0;
    vector<vector_item<double>*> centroids;

    /* Get all clusters from the begining, to save additional fetchs */ 
    for(int i = 0; i < num_of_clusters; i++)
        centroids.push_back(clusters[i]->get_centroid());

    /* Initialize min distance */
    min_distance = dist(*centroids[0], *centroids[1]);
    for(int i = 2; i < num_of_clusters; i++){
        double curr_dist = dist(*centroids[0], *centroids[i]);
        if(curr_dist <= min_distance && curr_dist != 0.0){
            min_distance = curr_dist;
        }
    }

    /* Check for all clusters the distance between centroids */ 
    for(int i = 1; i < num_of_clusters; i++){
        for(int j = i + 1; j < num_of_clusters; j++){
            double curr_dist = dist(*centroids[i], *centroids[j]);
            if(curr_dist <= min_distance && curr_dist != 0.0){
                min_distance = curr_dist;
            }
        }
    }


    return min_distance / 2.0;
}

int add_to_clusters(cluster<double>* cl, double& r, vector<vector_check*>& vectors_to_check, vector<cluster_info*>& vectors_info, int& vectors_left){
	int cluster_num = cl->get_cluster_num();
	int changes = 0;

	int num_of_vectors = vectors_to_check.size();

	/* Check all vectors that were collected before */
	for(int i = 0; i < num_of_vectors; i++){
		vector_item<double>& item = *(vectors_to_check[i]->item);

		/* Check if item was not checked already */
		double dist = vectors_to_check[i]->distance;
		if(dist <= r){
			int item_index = item.get_index();

			/* Vector is in radius, must check if its already assigned */
			if(vectors_info[item_index]->get_cluster_num() == -1){ // not assigned
				vectors_info[item_index]->set_cluster(cluster_num);
				vectors_info[item_index]->set_distance(dist);
				vectors_left--;
				changes++; // changes were made
			}
			else if(dist <= vectors_info[item_index]->get_distance()){ // vector is assigned, check if less distance
					vectors_info[item_index]->set_cluster(cluster_num);
					vectors_info[item_index]->set_distance(dist);
			}

			delete vectors_to_check[i];
			
			num_of_vectors--;
			i--;
			vectors_to_check.erase(vectors_to_check.begin() + i + 1); 	
		}
	}
	return changes;
}

void final_assign(cl_management<double>& cl_manage){
    dataset<double>* all_vectors = cl_manage.get_dataset();
    vector<cluster_info*>& vectors_info = cl_manage.get_vectors_info();
    vector<cluster<double>*>& clusters = cl_manage.get_clusters();
    
    dist_func dist_function = cl_manage.get_dist_func();

    int k = cl_manage.get_k(); // get number of total clusters
    int num_of_vectors = all_vectors->get_counter(); // get num of vectors

    /* Assign all non-centroids in dataset, to nearest centroid's cluster */ 
    for(int i = 0; i < num_of_vectors; i++){
        /* Check if not centroid */
        if(vectors_info[i]->get_is_centroid() == 1){
            continue;
        }

        vector_item<double>* curr_item = all_vectors->get_item(i);

        int cluster_num = vectors_info[i]->get_cluster_num();
        /* Check if cluster for current vector was found */
        if(cluster_num != -1){
            clusters[cluster_num]->add_vector(curr_item);
            continue;
        }

        /* Get current vector */
        double min_distance = 0.0;
        
        /* Check for all centroids and find the nearest */
        for(int j = 0; j < k; j++){
            vector_item<double>* curr_centroid = clusters[j]->get_centroid();

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



int new_execution(ifstream& input, ifstream& query, ofstream& output){
    string choice;

    while(1){
        cout << "Would you like to continue(y or n): ";
        fflush(stdout);
        
        getline(cin, choice);
        fflush(stdin);

        if(!choice.compare("y") || !choice.compare("Y") || !choice.compare("yes") ){
            cout << "Please choose one of the following(select number):" << endl;
            cout << "  0 Same files as before" << endl;
            cout << "  1 New Dataset" << endl;
            cout << "  2 New Query File" << endl;
            cout << "  3 New Output File" << endl;
            cout << "  4 New Dataset and Query File" << endl;
            cout << "  5 New Dataset and Output File" << endl;
            cout << "  6 New Query and Output File" << endl;
            cout << "  7 Entirely New Files" << endl;
            cout << "  8 I Changed My Mind. I Want To Exit\n" << endl;
            cout << "Note: If you use the same output file name, the previous one will be replaced!\n" << endl;
            cout << "My choice: ";

            fflush(stdout);
            getline(cin, choice);
            fflush(stdin);

            if(!choice.compare("0")) // Use same files
                return 0;
            else if(!choice.compare("1")){ // diff input{
                input.close();
                return 1;
            }
            else if(!choice.compare("2")){ // diff query
                query.close();
                return 2;
            }
            else if(!choice.compare("3")){ // diff out
                output.close();
                return 3;
            }
            else if(!choice.compare("4")){ // diff in+q
                input.close();
                query.close();
                return 4;
            }
            else if(!choice.compare("5")){ // diff in+out
                input.close();
                output.close();
                return 5;
            }
            else if(!choice.compare("6")){ // diff q+out
                query.close();
                output.close();
                return 6;
            }
            else if(!choice.compare("7")){ // diff ALL
                input.close();
                query.close();
                output.close();
                return 7;
            }
            else if(!choice.compare("8")) // EXIT
                return 8;
            else
                cout << "Invalid input given. Try again." << endl;
        }
        if(!choice.compare("n") || !choice.compare("N") || !choice.compare("no") )
            return 8; // EXIT
        else
            cout << "Invalid input given. Try again." << endl;
    }
}

int in_set(unordered_set<string>& check_set, string& id){
    unordered_set<string>::iterator it;

    it = check_set.find(id);
    if(it == check_set.end()) // id does not exist
        return 0;
    else // id is in set
        return 1; 
}

int in_set(unordered_set<int>& check_set, int id){
    unordered_set<int>::iterator it;

    it = check_set.find(id);
    if(it == check_set.end()) // id does not exist
        return 0;
    else // id is in set
        return 1; 
}

template <class T>
float exchausting_s(dataset<T>& data_set, vector_item<T>& item, int metric){
    float min_dist = -1.0;
    float cur_dist = 0.0;

    int data_sz = data_set.get_counter();
    if(metric == 1){ // euclidean distance

        /* Check all items in dataset */
        for(int i = 0; i < data_sz; i++){
            vector_item<T>* current_item = data_set.get_item(i);
            cur_dist = eucl_distance(*current_item, item);

            /* keep distance, if smaller than the minimum */
            if(cur_dist <= min_dist || i == 0)
                min_dist = cur_dist;
        }
    }
    else if(metric == 2){ // cosine distance

        /* Check all items in dataset */
        for(int i = 0; i < data_sz; i++){
            vector_item<T>* current_item = data_set.get_item(i);
            cur_dist = cs_distance(*current_item, item);

            /* keep distance, if smaller than the minimum */
            if(cur_dist <= min_dist || i == 0)
                min_dist = cur_dist;
        }
    }
    return min_dist;
}