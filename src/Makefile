CC = g++
CFLAGS = -g3 -Wall -std=c++11
OBJ = main.o tweet.o rService.o utils.o dataset.o clusters.o cl_algorithms.o lsh.o hypercube.o metrics.o
EXE = recommendation

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c $^

tweet.o: tweet.cpp
	$(CC) $(CFLAGS) -c $^

rService.o: rService.cpp
	$(CC) $(CFLAGS) -c $^

utils.o: utils.cpp
	$(CC) $(CFLAGS) -c $^

dataset.o: dataset.cpp
	$(CC) $(CFLAGS) -c $^

clusters.o: clusters.cpp
	$(CC) $(CFLAGS) -c $^

cl_algorithms.o: cl_algorithms.cpp
	$(CC) $(CFLAGS) -c $^

lsh.o: lsh.cpp
	$(CC) $(CFLAGS) -c $^

hypercube.o: hypercube.cpp
	$(CC) $(CFLAGS) -c $^

metrics.o: metrics.cpp
	$(CC) $(CFLAGS) -c $^

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ) 