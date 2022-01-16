# Inverted Search Engine

[![C/C++ CI Actions Status](https://github.com/NefeliTav/Software-Development-for-Information-Systems/workflows/C/C++%20CI/badge.svg)](https://github.com/NefeliTav/Software-Development-for-Information-Systems/actions)

### Authors
- [Georgios Nikolaou - sdi1800134](https://github.com/GiorgosNik)
- [Nefeli Tavoulari - sdi1800190](https://github.com/NefeliTav)

## Introduction
The current project is a modified version of The SIGMOD 2013 Programming Contest.
The application tackles the task of receiving a stream of documents and queries and matching them up, utilizing different metrics. The goal is to minimize the system response time.

### Compile & Run
```
$ make && make run
```
### Memcheck
```
$ make valgrind
```
### Detect data races with helgrind
```
$ make helgrind
```
### Detect data races with drd
```
$ make drd
```
### Clean
```
$ make clean
```
### Test
```
$ make test && make run_test
```
### Memcheck on Tests
```
$ make valgrind_test
```
### Run 100 times to test threads
```
$ ./run_100_times.sh
```