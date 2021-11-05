FILES = src/
TEST_FILES = tests/
OBJS = main.o functions.o structs.o index.o 
TEST_OBJ = tests.o functions.o structs.o index.o 
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

tests.o: $(TEST_FILES)tests.cpp
	$(CC) $(FLAGS) $(TEST_FILES)tests.cpp

test: $(TEST_OBJ)
	$(CC) -g $(TEST_OBJ) -o test

clean:
	rm -f $(OBJS) $(OUT) tests.o test 

run: $(OUT)
	./$(OUT)

run_test: test
	./test

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT) $(ARGS)
	
valgrind_test: test
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./test $(ARGS)