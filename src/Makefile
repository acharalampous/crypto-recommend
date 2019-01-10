CC = g++
CFLAGS = -g3 -Wall -std=c++11
OBJ = main.o tweet.o rService.o
EXE = recommendation

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c $^

tweet.o: tweet.cpp
	$(CC) $(CFLAGS) -c $^

rService.o: rService.cpp
	$(CC) $(CFLAGS) -c $^

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ) 