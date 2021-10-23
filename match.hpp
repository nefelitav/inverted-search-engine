#include <iostream>
#include <algorithm>
#include "core.h"
using namespace std;

#ifndef MATCH
#define MATCH

bool exactMatch(const char* word1,const char* word2);
int hammingDistance(const char* word1,const char* word2);
int editDistance(const char* word1,const char* word2);

#endif