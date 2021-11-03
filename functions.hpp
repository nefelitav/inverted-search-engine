#ifndef FUNCTIONS
#define FUNCTIONS

#include "./include/core.h"
#include "structs.hpp"
#include <iostream>
#include "cstring"
#include <algorithm>
#include <cmath>
#include <cstdlib>

int exactMatch(const char* word1,const char* word2);
int hammingDistance(const char* word1,const char* word2);
int editDistance(const char* word1,const char* word2);

int binarySearch(word* words, int left, int right, const word w);
unsigned long hashFunction(word str);
void Deduplication(Document* d);
 
class HashTable {
    private:
        word** buckets;
        int* wordsPerBucket;
    public:
        HashTable();
        void addToBucket(int hash, const word w);
        void printTable();
        ~HashTable();
};
#endif