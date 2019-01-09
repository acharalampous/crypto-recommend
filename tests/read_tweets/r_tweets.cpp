#include <iostream>
#include <fstream>

using namespace std;

int main(void){
    cout << "some " << endl;
    ifstream new_file("files/tweets_dataset_small.csv");
    string line;
    string id;
    string tweet_id;
    string tweet;
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

        // while ((pos = line.find_first_of("\t,", prev)) != string::npos)
        // {
        //     // sleep(1);
        //     if (pos > prev)
        //         cout << i << ". " << line.substr(prev, pos - prev) << endl;
        //     prev = pos+1;
        //     i++;
        // }
        
        // if (prev < line.length())
        //     cout << i << ". " << line.substr(prev, string::npos) << endl;
    }

}