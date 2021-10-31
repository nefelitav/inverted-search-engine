OBJS	= main.o match.o structs.o index.o treeNodeList.o childQueue.o
SOURCE	= main.cpp structs.cpp match.cpp index.cpp treeNodeList.cpp childQueue.cpp
HEADER	= structs.hpp match.hpp core.h index.h treeNodeList.h childQueue.hpp
OUT	= main
CC	 = g++
FLAGS = -g -c -Wall -std=c++11

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

match.o: match.cpp
	$(CC) $(FLAGS) match.cpp

structs.o: structs.cpp
	$(CC) $(FLAGS) structs.cpp

index.o: index.cpp
	$(CC) $(FLAGS) index.cpp

treeNodeList.o: treeNodeList.cpp
	$(CC) $(FLAGS) treeNodeList.cpp

childQueue.o: childQueue.cpp
	$(CC) $(FLAGS) childQueue.cpp

clean:
	rm -f $(OBJS) $(OUT) match structs *.o

run: $(OUT)
	./$(OUT)

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT) $(ARGS)
	