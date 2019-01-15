#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

int main(void){
    unordered_map<int, vector<int>> mp;

    unordered_map<int, vector<int>>::iterator f = mp.find(3);

    if(f == mp.end())
        mp[3] = {1};
    else
        f->second.push_back(1);
    
    f = mp.find(3);

    if(f == mp.end())
        mp[3] = {6};
    else
        f->second.push_back(6);

    
    




    unordered_map<int, vector<int>>::iterator it = mp.begin();
    for(it = mp.begin(); it != mp.end(); it++){
        cout << it->first  << endl << "\t";
        for(int i = 0; i < it->second.size(); i++)
            cout << it->second[i] << " ";
        cout << endl;
    }


    // vector<int> vec;

    // // vec.push_back(1);
    // // vec.push_back(2);
    // // vec.push_back(3);
    // // vec.insert(vec.begin() + 1, 4);
    // vec.insert(vec.begin() + 0, 15);
    // vec.insert(vec.begin() + 1, 12);

    // for(unsigned int i = 0; i < vec.size(); i++)
    //     cout << vec[i] << " ";


    // cout << endl;
}