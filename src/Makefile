CC = g++
CFLAGS = -g3 -Wall
OBJ = main.o tweet.o
EXE = recommendation

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c $^

tweet.o: tweet.cpp
	$(CC) $(CFLAGS) -c $^

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ) 