#include <iostream>
#include "r.h"

using namespace std;

int D = 203;

int print1(){
	cout << "1 " << D << endl;
}

int print2(){
	cout << "1 " << D << endl;
}

void rdf(){
	#undef D
	#define D 10
}
