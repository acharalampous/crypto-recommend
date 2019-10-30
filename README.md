# CryptoCurrencies Recommendation System
<p align="center">
  <img width = 400 height = 300 src="img.png">
</p>
<br />Implementation of CryptoCurrency Recommendation algorithms for users using their tweets. Each user U = {u1, u2, .., uN} has a set of tweets t containing references and comments to specific cryptocurrencies. Using a Grammar A, a sentiment analysis is made for every tweet. Another Grammar K contains the tags (aliases) for the cryptocurrencies, so based on the sentiment analysis for each user, a vector V is created, in which each point represents the total sentiment for each cryptocurrency. Based on the Vi vector, and the user's tweets, two different approaches are used in order to find the best cryptocurrencies to recommend. Finally, a 10-fold validation is done in order to calculate the average absolute error. The recommendation algorithms used are:

1. Cosine LSH Recommendation: Using the implementation here: www.github.com/AndCharalampous/lsh
2. Clustering Recommendation: Using the implemetation here: www.github.com/AndCharalampous/vectors-clustering
## Deployment

The code was developed and tested on **Linux Ubuntu 18.04 LTS** using **C++11** and **g++ v7.3.0**.

## Prerequisites

A Linux distribution is required with a g++ compiler order to compile and execute. _g++ v7.3.0_ is recommended.
<br />You can check your gcc compiler version using:
<br />``` $ g++ --version ```

You can install the latest gcc compiler on Ubuntu based systems using:
<br />``` $ sudo apt install build-essential ``` 

## Getting Started
Download Repository Locally using:
<br /> ```$ git clone https://github.com/AndCharalampous/crypto-recommend.git ```

## Compilation
Compile code using the Makefile provided:
<br /> ```$ cd src```
<br /> ```$ make```
<br /> Executable file recommendation is created.

### Configuration Files
Two conf files must be provided (cluster1.conf, cluster2.conf), for tweet and stentiment clustering respectively. Their convention is the same, the only difference is that cluster1.conf can take the path to cluster2.conf (tag in bold).The format of the configuration for the Clustering approach must follow the convention of <tag>:<int> (without space). The available tags are:
* **input**: input file for cluster2.conf, tag only in cluster1.conf
* number_of_clusters
* number_of_hash_functions
* number_of_hash_tables
* max_updates
* hc_probes
* hc_M
<br />_The tags can be given in any order. The number of clusters must be given, all the other tags can be omitted, using the default values_.

### Tweets File
The file containing the tweets must have this convention:
<br />P: [Integer]
<br />1 915 bitcoin profitable ..
<br />1 952 ethereum losses ..
<br />2 93 tether bear ..
<br />..
<br />1143  43  ethereum bull ..
<br />On the first line the number of nn used is given.
<br />Each line contains the UserId, TweetId and Tweet Content separated with tabs (\t).

### Execution
Execute using:
<br /> ```$ ./recommendation [-d input_file] [-l lex_file] [-o output_file] [-c coin_file] [-validate]```
* input_file: Path to file that contains tweets
* lex_file: Path to the lexicon file
* output_file: Path to results file
* coin_file: Path to file with the crypto's tags
* validate: Perform 10-fold validation
* 1c: If given only **1c**ombination of algorithms will be used, else all 12 
_All parameters are optional during executing. If not provided, the four files will be requested from user. If validate parameter is not given, no 10-fold will be performed._

### Output
In output file (First Column: User Id, Second Column: Recommended Cryptos):
<br />1. LSB-COSINE Recommendation
<br />  Problem A
<br />8 Not enough information about user
<br />11 "bitcoing" "ethereum" ..
<br />24 "power ledger" "gas" ..
<br />....
<br />  Problem B
<br />...
<br />2. Clustering Recommendation
<br />  Problem A
<br />...
<br />  Problem B
<br />...
<br />If 10-fold Validation was performed then the output should contain at the end
<br />Cosine LSH Recommendation MAE: 0.17542
<br />Clustering Recommendation MAE: 0.18863

## Author
**Andreas Charalampous** - https://github.com/AndCharalampous

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
