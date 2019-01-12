#include <iostream>
#include <array>
#define D 5

using namespace std;

int main(void){
	//cout << D << endl;
	array<int, D> arr;
	cout << arr.size() << endl;
	#undef D
	

	#define D 10

	cout << arr.size() << endl;

	cout << D << endl;

}
