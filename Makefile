FILES = source/
OBJS	= main.o functions.o structs.o index.o
OUT	= main
CC	 = g++
FLAGS = -g -c -Wall -std=c++11 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 

main.o: $(FILES)main.cpp
	$(CC) $(FLAGS) $(FILES)main.cpp 

functions.o: $(FILES)functions.cpp
	$(CC) $(FLAGS) $(FILES)functions.cpp

structs.o: $(FILES)structs.cpp
	$(CC) $(FLAGS) $(FILES)structs.cpp

index.o: $(FILES)index.cpp
	$(CC) $(FLAGS) $(FILES)index.cpp

clean:
	rm -f $(OBJS) $(OUT) *.o

run: $(OUT)
	./$(OUT)

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT) $(ARGS)
	