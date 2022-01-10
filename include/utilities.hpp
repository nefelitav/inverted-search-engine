#ifndef UTILITIES
#define UTILITIES
#define MIN(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#include "structs.hpp"
#include "jobscheduler.hpp"

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

void storeResult(int numRes, DocID document, QueryID *queries);


//////////////////////////////////////////////////////////////////////////

class matchedQuery {
    private:
        QueryID id;
        matchedQuery* next;
    public:
        matchedQuery(QueryID id);
        matchedQuery* getNext();
        QueryID getId();
        void setNext(matchedQuery* next);
};

class matchedQueryList                 // list of matched queries
{
    private:
        matchedQuery *head;

    public:
        matchedQueryList();
        void addToList(QueryID id);
        void printList();
        matchedQuery *getHead();
        ~matchedQueryList();
};

//////////////////////////////////////////////////////////////////////////

class DocTable                           // hash table where document words are stored after deduplication
{
    private:
        word **buckets;
        int wordsPerBucket[MAX_BUCKETS];
        DocID tableID;
    public:
        DocTable(DocID id);
        unsigned long addToBucket(unsigned long hash, const word w);
        ErrorCode wordLookup();
        const word *getBucket(unsigned long hash) const;
        void printBucket(unsigned long hash) const;
        void printTable() const;
        const int getWordsPerBucket(unsigned long hash) const;
        ~DocTable();
};

DocTable *DocumentDeduplication(Document *d, DocTable *HT);

//////////////////////////////////////////////////////////////////////////

class QueryTable     // hash table where all queries are stored, so that i can access them with ease
{
    private:
        Query*** buckets;
        int queriesPerBucket[MAX_QUERY_BUCKETS];

    public:
        QueryTable();
        QueryTable* cloneQueryTable();
        unsigned long addToBucket(unsigned long hash, Query *q);
        Query **getBucket(unsigned long hash) const;
        void printBucket(unsigned long hash) const;
        void printTable() const;
        Query *getQuery(QueryID id);
        void deleteQuery(QueryID id);
        const int getQueriesPerBucket(unsigned long hash) const;
        ~QueryTable();
};


//////////////////////////////////////////////////////////////////////////

class EntryTable {      // hash table where entries are stored, used for exact match
    private:
        entry*** buckets;
        int entriesPerBucket[MAX_BUCKETS];
    public:
        EntryTable();
        unsigned long addToBucket(unsigned long hash, entry* e);
        entry** getBucket(unsigned long hash) const;
        void printBucket(unsigned long hash) const;
        void printTable() const;
        const int getEntriesPerBucket(unsigned long hash) const;
        void deleteQueryId(word givenWord, const QueryID queryId);
        void wordLookup(const word w, entry_list* result);
        ~EntryTable();
};


//////////////////////////////////////////////////////////////////////////

class ResultTable {         // hash table where query ids are stored, deduplicated
    private:
        QueryID** buckets;
        int queriesPerBucket[MAX_QUERY_BUCKETS];
    public:
        ResultTable();
        unsigned long addToBucket(unsigned long hash, QueryID id);
        QueryID* getBucket(unsigned long hash) const;
        void printBucket(unsigned long hash) const;
        void printTable() const;
        Query* getQuery(QueryID id);
        matchedQueryList* checkMatch(QueryTable* tempTable);
        const int getQueriesPerBucket(unsigned long hash) const;
        ~ResultTable();
};


//////////////////////////////////////////////////////////////////////////

extern QueryTable* QT;
extern entry_list* EntryList; 
extern EntryTable* ET;
extern result* resultList;



inline bool exactMatch(const word word1, const word word2)
{
    if (word1 == NULL || word2 == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    return (strcmp(word1, word2) == 0);
}

inline int hammingDistance(const word word1, const word word2)
{
    int i = 0;
    int diff = 0;

    // If words have different length raise exception
    if (strlen(word1) != strlen(word2))
    {
        throw std::invalid_argument("Words Have Different Length");
    }

    // Difference in the common part of the words
    while (word1[i] != '\0')
    {
        if (word1[i] != word2[i])
        {
            diff++;
        }
        i++;
    }
    // Return the Hamming Distance
    return diff;
}

inline int editDistance(const word word1, const word word2)
{
    unsigned int len1, len2, x, y, lastDiag, oldDiag;

    len1 = strlen(word1);
    len2 = strlen(word2);

    unsigned int col[len1 + 1];      // re-write the same colum
    for (y = 1; y <= len1; y++)
    {
        col[y] = y;
    }

    for (x = 1; x <= len2; x++) 
    {
        col[0] = x;
        for (y = 1, lastDiag = x - 1; y <= len1; y++) 
        {
            oldDiag = col[y];
            col[y] = MIN(col[y] + 1, col[y - 1] + 1, lastDiag + (word1[y - 1] == word2[x - 1] ? 0 : 1));
            lastDiag = oldDiag;
        }
    }
    return col[len1];
}


inline unsigned long hashFunction(word str) // a typical hash function
{
    if (str == NULL || strcmp(str, " ") == 0)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    unsigned long hash = 5381;
    int character;
    while ((character = *str++))
    {
        hash = ((hash << 5) + hash) + character;
    }
    return hash % MAX_BUCKETS;
}

inline unsigned long hashFunctionById(QueryID id) // a typical hash function
{
    unsigned long hash = ((id >> 16) ^ id) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = (hash >> 16) ^ hash;
    return hash % MAX_QUERY_BUCKETS;
}

inline int binarySearch(word *words, int left, int right, const word w)
{
    if (words == NULL || w == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int mid, cmp;
    while (left < right)
    {
        mid = left + (right - left) / 2;
        cmp = strcmp(w, words[mid]);

        if (cmp == 0)
        {
            return -1; // word already in array -> continue
        }
        else if (cmp > 0) // word should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0) // word should go before mid
        {
            right = mid - 1;
        }
    }
    cmp = strcmp(w, words[left]);
    if (cmp > 0)
    {
        return left + 1;
    }
    else if (cmp < 0)
    {
        return left;
    }
    return -1; // word already in array -> continue
}


inline int queryBinarySearch(Query **queries, int left, int right, const QueryID id)
{
    if (queries == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int mid, cmp;
    while (left < right)
    {
        mid = left + (right - left) / 2;
        cmp = id - queries[mid]->getId();
        if (cmp == 0)
        {
            return -1; // query already in array -> continue
        }
        else if (cmp > 0) // query should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0) // query should go before mid
        {
            right = mid - 1;
        }
    }
    cmp = id - queries[left]->getId();
    if (cmp > 0)
    {
        return left + 1;
    }
    else if (cmp < 0)
    {
        return left;
    }
    return -1; // query already in array -> continue
}



inline int entryBinarySearch(entry **entries, int left, int right, const word w)
{
    if (entries == NULL || w == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int mid, cmp;
    while (left < right)
    {
        mid = left + (right - left) / 2;
        cmp = strcmp(w, entries[mid]->getWord());
        if (cmp == 0)
        {
            return mid; // entry already in array -> add queryid to payload
        }
        else if (cmp > 0) // entry should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0) // entry should go before mid
        {
            right = mid - 1;
        }
    }
    cmp = strcmp(w, entries[left]->getWord());
    if (cmp > 0)
    {
        return left + 1;
    }
    return left;
}


inline int resultBinarySearch(QueryID *ids, int left, int right, const QueryID id)
{
    if (ids == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int mid, cmp;
    while (left < right)
    {
        mid = left + (right - left) / 2;
        cmp = id - ids[mid];
        if (cmp == 0)
        {
            return -1; // query already in array -> continue
        }
        else if (cmp > 0) // query should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0) // query should go before mid
        {
            right = mid - 1;
        }
    }
    cmp = id - ids[left];
    if (cmp > 0)
    {
        return left + 1;
    }
    else if (cmp < 0)
    {
        return left;
    }
    return -1; // query already in array -> continue
}


inline int findEntry(entry **entries, int left, int right, const word w)
{
    if (w == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    if (entries == NULL)
    {
        return -1;
    }
    if (right >= left)
    {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(w, entries[mid]->getWord());

        if (cmp == 0)
        {
            return mid;
        }
        if (cmp > 0)
        {
            return findEntry(entries, left, mid - 1, w);
        }
        return findEntry(entries, mid + 1, right, w);
    }
    // not in array
    return -1;
}

inline int findQuery(Query **queries, int left, int right, const QueryID id) // binary search
{
    if (queries == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    if (right >= left)
    {
        int mid = left + (right - left) / 2;
        if (id == queries[mid]->getId())
        {
            return mid;
        }
        if (queries[mid]->getId() > id)
        {
            return findQuery(queries, left, mid - 1, id);
        }
        return findQuery(queries, mid + 1, right, id);
    }
    // not in array
    return -1;
}

inline void merge(QueryID* queryIds, int left, int mid, int right) 
{
    int len1 = mid - left + 1;
    int len2 = right - mid;
    int leftArr[len1], rightArr[len2];
    int i = 0, j = 0, k = left;

    for (int i = 0; i < len1; i++)            // split array in two
    {
        leftArr[i] = queryIds[left + i];
    }
    for (int j = 0; j < len2; j++)
    {
        rightArr[j] = queryIds[mid + 1 + j];
    }
    
    while (i < len1 && j < len2)              // compare subarrays 
    {
        if (leftArr[i] <= rightArr[j]) 
        {
          queryIds[k] = leftArr[i];
          i++;
        } 
        else 
        {
          queryIds[k] = rightArr[j];
          j++;
        }
        k++;
    }
    while (i < len1) 
    {
        queryIds[k] = leftArr[i];
        i++;
        k++;
    }
    while (j < len2)
    {
        queryIds[k] = rightArr[j];
        j++;
        k++;
    }
}

inline void mergeSort(QueryID* queryIds, int left, int right) 
{
    if (left < right) 
    {
        int mid = left + (right - left) / 2;
        mergeSort(queryIds, left, mid);
        mergeSort(queryIds, mid + 1, right);
        merge(queryIds, left, mid, right);
    }
}
#endif