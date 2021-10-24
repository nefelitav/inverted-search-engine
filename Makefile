OBJS	= main.o match.o structs.o
SOURCE	= main.cpp structs.cpp match.cpp
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

structs.o: structs.cpp
	$(CC) $(FLAGS) structs.cpp

clean:
	rm -f $(OBJS) $(OUT) match structs *.o

run: $(OUT)
	./$(OUT)

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT) $(ARGS)
	