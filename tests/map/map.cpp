#include <iostream>
#include <map>
#include <string>
using namespace std;

int main(void){
	pair<double, string> x;
	multimap<double, string> mp;
	
	
	x.first = 1.5;
	x.second = "1os";
	mp.insert(x);

	x.first = 7.5;
	x.second = "2os";
	mp.insert(x);

	x.first = 2.3;
	x.second = "3os";
	mp.insert(x);

	x.first = 10.1;
	x.second = "4os";
	mp.insert(x);


	x.first = -1.2;
	x.second = "5os";
	mp.insert(x);

	for(multimap<double, string>::iterator it = mp.begin(); it != mp.end(); it++){
		cout << it->first << " - " << it->second << endl;

	} 
}
