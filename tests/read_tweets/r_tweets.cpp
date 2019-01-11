#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

int main(void){
    cout << "some " << endl;
    ifstream new_file("files/tweets_dataset_small.csv");
    string line;
    string id;
    string tweet_id;
    string tweet;

    int total = 0;
    while(getline(new_file, line))
        total++;

    cout << total << endl;
    sleep(3);
    new_file.clear();
    new_file.seekg(0, ios::beg);

    
    while(getline(new_file, line)){
        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        size_t prev = 0, pos;
        int i = 0;

        /* reading user id */
        pos = line.find_first_of("\t,", prev);
        
        id = line.substr(prev, pos - prev);
        prev = pos + 1;

        /* reading tweet id */
        pos = line.find_first_of("\t,", prev);
        
        /* keep tweet */
        tweet_id = line.substr(prev, pos - prev);
        prev = pos + 1;

        cout << "USER#: " << id << " TWEET#: " << tweet_id << endl;
        cout << "TWEET: " << tweet << endl;

        tweet = line.substr(prev, line.size() - prev);
    }

}