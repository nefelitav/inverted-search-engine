OBJS	= main.o functions.o structs.o
SOURCE	= main.cpp structs.cpp functions.cpp
HEADER	= structs.hpp functions.hpp core.h
OUT	= main
CC	 = g++
FLAGS = -g -c -Wall

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

functions.o: functions.cpp
	$(CC) $(FLAGS) functions.cpp

structs.o: structs.cpp
	$(CC) $(FLAGS) structs.cpp

clean:
	rm -f $(OBJS) $(OUT) functions structs *.o

run: $(OUT)
	./$(OUT)

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT) $(ARGS)
	