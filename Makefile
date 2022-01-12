FILES = src/
TEST_FILES = tests/
OBJS = utilities.o structs.o index.o jobscheduler.o core.o libcore.so
TEST_OBJ = utilities.o structs.o index.o jobscheduler.o core.o tests.o 
OUT	= main
CC	 = g++
FLAGS = -g -c -pthread -Wall -O3 -fPIC -std=c++11 
TEST_FLAGS = -g -c -pthread -Wall -fPIC -std=c++11 


all: $(OBJS) $(FILES)main.cpp 
	$(CC) -g -Wall -fPIC -I. -O3 -pthread -std=c++11 -o main $(FILES)main.cpp ./libcore.so

utilities.o: $(FILES)utilities.cpp
	$(CC) $(FLAGS) $(FILES)utilities.cpp

structs.o: $(FILES)structs.cpp
	$(CC) $(FLAGS) $(FILES)structs.cpp

jobscheduler.o: $(FILES)jobscheduler.cpp
	$(CC) $(FLAGS) $(FILES)jobscheduler.cpp

index.o: $(FILES)index.cpp
	$(CC) $(FLAGS) $(FILES)index.cpp

libcore.so: $(FILES)core.cpp $(FILES)jobscheduler.cpp $(FILES)structs.cpp $(FILES)utilities.cpp $(FILES)index.cpp
	$(CC) -shared -fPIC -pthread -O3 -o libcore.so core.o jobscheduler.o structs.o index.o utilities.o -lc

core.o: $(FILES)core.cpp
	$(CC) $(FLAGS) $(FILES)core.cpp

tests.o: $(TEST_FILES)tests.cpp
	$(CC) $(TEST_FLAGS) $(TEST_FILES)tests.cpp

test: $(TEST_OBJ)
	$(CC) -Wall -fPIC -I. -pthread -std=c++11 -o -g $(TEST_OBJ) -o test

clean:
	rm -f $(OBJS) $(OUT) $(FILES)core.o tests.o test 

run: $(OUT)
	./$(OUT)

run_test: test
	./test 

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./$(OUT)
	
valgrind_test: test
	valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes ./test

helgrind: $(OUT)
	valgrind --tool=helgrind ./$(OUT)