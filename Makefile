FILES = src/
TEST_FILES = tests/
OBJS = utilities.o structs.o index.o thread.o core.o libcore.so
TEST_OBJ = tests.o utilities.o structs.o index.o thread.o core.o
OUT	= main
CC	 = g++
FLAGS = -g -c -lpthread -Wall -O3 -fPIC -std=c++11 

all: $(OBJS) $(FILES)main.cpp 
	$(CC) -g -Wall -fPIC -I. -O3 -lpthread -std=c++11 -o main $(FILES)main.cpp ./libcore.so

utilities.o: $(FILES)utilities.cpp
	$(CC) $(FLAGS) $(FILES)utilities.cpp

structs.o: $(FILES)structs.cpp
	$(CC) $(FLAGS) $(FILES)structs.cpp

thread.o: $(FILES)thread.cpp
	$(CC) $(FLAGS) $(FILES)thread.cpp

index.o: $(FILES)index.cpp
	$(CC) $(FLAGS) $(FILES)index.cpp

libcore.so: $(FILES)core.o
	$(CC) -shared -fPIC -lpthread -O3 -o libcore.so core.o thread.o structs.o index.o utilities.o -lc

core.o: $(FILES)core.cpp
	$(CC) $(FLAGS) $(FILES)core.cpp

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
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT)
	
valgrind_test: test
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./test