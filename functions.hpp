#include <iostream>
#include <algorithm>
#include "core.h"
#include "structs.hpp"

using namespace std;

#ifndef FUNCTIONS
#define FUNCTIONS

bool exactMatch(const char* word1,const char* word2);
int hammingDistance(const char* word1,const char* word2);
int editDistance(const char* word1,const char* word2);

int binarySearch(word* words, int left, int right, const word w);
unsigned long hashFunction(char *str);
void Deduplication(Document* d);
 
class HashTable {
    private:
        word** buckets;
        int* wordsPerBucket;
    public:
        HashTable();
        void addToBucket(int hash, word w);
        void printTable();
        ~HashTable();
};
#endif