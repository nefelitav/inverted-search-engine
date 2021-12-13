#ifndef UTILITIES
#define UTILITIES

#include "core.h"
#include "structs.hpp"
#include <iostream>
#include "cstring"
#include <algorithm>
#include <cmath>
#include <cstdlib>

bool exactMatch(const word word1, const word word2);
int hammingDistance(const word word1, const word word2);
int editDistance(const word word1, const word word2);

class result
{
private:
    int num_res;
    DocID doc_id;
    QueryID *query_ids;
    result *next;

public:
    result(int numRes, DocID document, QueryID *queries);
    void addResult(result *toAdd);
    result *getNext();
    int getNumRes();
    DocID getDocID();
    QueryID *getQueries();
};

class matchedQuery
{
private:
    QueryID id;
    matchedQuery *next;

public:
    matchedQuery(QueryID id);
    matchedQuery *getNext();
    QueryID getId();
    void setNext(matchedQuery *next);
};

class matchedQueryList
{
private:
    matchedQuery *head;

public:
    matchedQueryList();
    void addToList(QueryID id);
    void printList();
    matchedQuery *getHead();
    void removeQuery(matchedQuery *q);
    ~matchedQueryList();
};

class DocTable
{
private:
    word **buckets;
    int *wordsPerBucket;

public:
    DocTable();
    unsigned long addToBucket(unsigned long hash, const word w);
    void wordLookup();
    const word *getBucket(unsigned long hash) const;
    void printBucket(unsigned long hash) const;
    void printTable() const;
    const int getWordsPerBucket(unsigned long hash) const;
    ~DocTable();
};

DocTable *DocumentDeduplication(Document *d, DocTable *HT);
int binarySearch(word *words, int left, int right, const word w);
unsigned long hashFunction(const word str);

////////////////////////////////////////////////////////////////////

class QueryTable
{
private:
    Query ***buckets;
    int *QueriesPerBucket;

public:
    QueryTable();
    unsigned long addToBucket(unsigned long hash, Query *q);
    Query **getBucket(unsigned long hash) const;
    void printBucket(unsigned long hash) const;
    void printTable() const;
    Query *getQuery(QueryID id);
    void deleteQuery(QueryID id) const;
    const int getQueriesPerBucket(unsigned long hash) const;
    ~QueryTable();
};

int findQuery(Query **queries, int left, int right, const QueryID id);
unsigned long hashFunctionById(QueryID id);
int QuerybinarySearch(Query **queries, int left, int right, const QueryID id);

class EntryTable
{
private:
    entry ***buckets;
    int *entriesPerBucket;

public:
    EntryTable();
    unsigned long addToBucket(unsigned long hash, entry *e);
    entry **getBucket(unsigned long hash) const;
    void printBucket(unsigned long hash) const;
    void printTable() const;
    const int getEntriesPerBucket(unsigned long hash) const;
    void deleteQueryId(word givenWord, const QueryID queryId);
    void wordLookup(const word w, entry_list *result);
    ~EntryTable();
};
int findEntry(entry **entries, int left, int right, const word w);
int entrybinarySearch(entry **entries, int left, int right, const word w);
void storeResult(int numRes, DocID document, QueryID *queries);

extern QueryTable *QT;
extern entry_list *EntryList;
extern EntryTable *ET;
extern result *resultList;

#endif