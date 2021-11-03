OBJS	= main.o functions.o structs.o index.o
OUT	= main
CC	 = g++
FLAGS = -g -c -Wall -std=c++11 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

functions.o: functions.cpp
	$(CC) $(FLAGS) functions.cpp

structs.o: structs.cpp
	$(CC) $(FLAGS) structs.cpp

index.o: index.cpp
	$(CC) $(FLAGS) index.cpp

clean:
	rm -f $(OBJS) $(OUT) *.o

run: $(OUT)
	./$(OUT)

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT) $(ARGS)
	