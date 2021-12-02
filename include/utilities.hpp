#ifndef UTILITIES
#define UTILITIES

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
        void addWordsToIndex();
        const word* getBucket(unsigned long hash) const;
        void printBucket(unsigned long hash) const;
        void printTable() const;
        const int getWordsPerBucket(unsigned long hash) const;
        ~HashTable();
};

HashTable* DocumentDeduplication(Document* d, HashTable* HT);
HashTable* QueryDeduplication(Query* q, HashTable* HT);
int binarySearch(word* words, int left, int right, const word w);
unsigned long hashFunction(const word str);

////////////////////////////////////////////////////////////////////


unsigned long hashFunctionById(QueryID id);
int QuerybinarySearch(Query** queries, int left, int right, const QueryID id);
class QueryTable {
    private:
        Query*** buckets;
        int* QueriesPerBucket;
    public:
        QueryTable();
        unsigned long addToBucket(unsigned long hash, Query* q);
        Query** getBucket(unsigned long hash) const;
        void printBucket(unsigned long hash) const;
        void printTable() const;
        const Query* getQuery(QueryID id) const;
        void deleteQuery(QueryID id) const;
        const int getQueriesPerBucket(unsigned long hash) const;
        ~QueryTable();
};

extern QueryTable* QT;
//extern HashTable* QHT;
extern entry_list* EntryList; 

#endif