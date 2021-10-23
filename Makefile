OBJS	= main.o match.o
SOURCE	= main.cpp match.cpp
HEADER	= structs.hpp match.hpp core.h
OUT	= main
CC	 = g++
FLAGS = -g -c -Wall

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

match.o: match.cpp
	$(CC) $(FLAGS) match.cpp

clean:
	rm -f $(OBJS) $(OUT) match *.o

run: $(OUT)
	./$(OUT) $(ARGS)

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT) $(ARGS)
	