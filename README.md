# Inverted Search Engine

[![C/C++ CI Actions Status](https://github.com/nefelitav/inverted-search-engine/workflows/C/C++%20CI/badge.svg)](https://github.com/nefelitav/inverted-search-engine/actions)

### Authors
- [Georgios Nikolaou](https://github.com/GiorgosNik)
- [Nefeli Tavoulari](https://github.com/nefelitav)

## Introduction
The current project is a modified version of The SIGMOD 2013 Programming Contest.
The application tackles the task of receiving a stream of documents and queries and matching them up, utilizing different metrics. The goal is to minimize the system response time.

### Compile & Run
```
$ make && make run
```
### Check for memory leaks
```
$ make valgrind
```
### Detect data races
```
$ make helgrind
```
### Test
```
$ make test && make run_test
```
