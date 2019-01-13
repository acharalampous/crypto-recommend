#include <iostream>
#include <array>
#include "r.h"

extern int D;

using namespace std;

int main(void){
	cout << "0 " << D << endl;
	print1();	
	
	D = 100;

	cout << "0 " << D << endl;
	print2();
}
