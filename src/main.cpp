#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "tweet.h"
using namespace std;

int main(void){
    ifstream cc("files/coins_queries.csv");
    string line;
    int i = 0;
    vector<cryptocurrency*> cryptos;
    unordered_map<string, cryptocurrency*> map;
    while(getline(cc, line)){
        //cout << line << endl;
        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        size_t pos, prev = 0;

        int j = 0;
        string tag;
        while ((pos = line.find_first_of("\t,", prev)) != string::npos)
        {
            if (pos > prev){
                tag = line.substr(prev, pos - prev);
                if(j == 0){
                    cryptos.push_back(new cryptocurrency(tag, i));
                }
                else if(j == 4){
                    cryptos[i]->set_name(tag);
                }
                map[tag] = cryptos[i];
            }
            prev = pos + 1;
            j++;
        }
        
        if (prev < line.length()){
            tag = line.substr(prev, string::npos);
            if(j == 0){
                cryptos.push_back(new cryptocurrency(tag, i));
            }
            else if(j == 4){
                cryptos[i]->set_name(tag);
            }
            map[tag] = cryptos[i];
        }
        i++;
    }
    cout << "h" << endl;
}