#ifndef FUNCTIONS
#define FUNCTIONS

#include "core.h"
#include "structs.hpp"
#include <iostream>
#include "cstring"
#include <algorithm>
#include <cmath>
#include <cstdlib>

bool exactMatch(const word word1,const word word2);
int hammingDistance(const word word1,const word word2);
int editDistance(const word word1,const word word2);

int binarySearch(word* words, int left, int right, const word w);
unsigned long hashFunction(const word str);
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