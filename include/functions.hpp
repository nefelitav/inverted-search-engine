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
 
class HashTable {
    private:
        word** buckets;
        int* wordsPerBucket;
    public:
        HashTable();
        unsigned long addToBucket(unsigned long hash, const word w);
        const word* getBucket(unsigned long hash) const;
        void printBucket(unsigned long hash) const;
        void printTable() const;
        const int getWordsPerBucket(unsigned long hash) const;
        ~HashTable();
};

HashTable* Deduplication(Document* d, HashTable * HT);
int binarySearch(word* words, int left, int right, const word w);
unsigned long hashFunction(const word str);

#endif