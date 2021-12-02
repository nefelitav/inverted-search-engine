#include "../include/core.h"
#include "../include/utilities.hpp"



bool exactMatch(const word word1, const word word2) {
    if (word1 == NULL || word2 == NULL) {
        throw std::invalid_argument( "Got NULL pointer");
    }
    return (strcmp(word1,word2) == 0);
}

int hammingDistance(const word word1, const word word2) {
    if (word1 == NULL || word2 == NULL) {
        throw std::invalid_argument( "Got NULL pointer");
    }
    int i = 0;

    // If words have different length raise exception
    int diff = abs((int)strlen(word1) - (int)strlen(word2));
    if ( diff != 0 ){
        throw std::invalid_argument( "Words Have Different Length");
    }

    // Difference in the common part of the words
    while (word1[i] != '\0' && word2[i] != '\0' ) {
        if (word1[i] != word2[i]) {
            diff++;
        }
        i++;
    }

    // Return the Hamming Distance
    return diff;
}

int editDistance(const word word1,const word word2) {
    if (word1 == NULL || word2 == NULL) {
        throw std::invalid_argument( "Got NULL pointer");
    }
    int final_distance;
    int size1 = 0;
    int size2 = 0;
    int substitution;
    int **d;

    // if equal, just stop
    if (strcmp(word1, word2) == 0) {
        return 0;
    }
    
    // Get the size of each word to create the matrix
    size1 = strlen(word1);
    size2 = strlen(word2);

    // Allocate the matrix with dims Size1*Size2
    d = new int*[size1 + 1];
    for (int i = 0; i < size1 + 1; i++) {
        d[i] = new int[size2 + 1]();
    }

    // Initialize border elements
    for (int i = 0; i < size2; i++) {
        d[0][i] = i;
    }
    for (int i = 0; i < size1; i++) { 
        d[i][0] = i;
    }
    
    // Calculate the minimum number of differences with the full-matrix iterative method
    for (int j = 1; j < size2 + 1; j++) {
        for (int i = 1; i < size1 + 1; i++) {
            if (word1[i-1] == word2[j-1]) {
                substitution = 0;
            } else {
                substitution = 1;
            }
            d[i][j] = std::min(std::min(d[i-1][j] + 1, d[i][j-1] + 1), d[i-1][j-1] + substitution);
        }
    }
    final_distance = d[size1][size2];

    // Delete the matrix
    for (int i = 0; i < size1 + 1; i++) {
        delete[] d[i];
    }
    delete[] d;
   
    // Return the Edit Distance
    return final_distance;
}

////////////////////////////////////////////////////////////////////////////////////

int binarySearch(word* words, int left, int right, const word w)
{
    if (words == NULL || w == NULL)
    {
        throw std::invalid_argument( "Got NULL pointer");
    }
    int mid, cmp;
    while (left < right) 
    {
        mid = left + (right - left) / 2;
        cmp = strcmp(w, words[mid]);
        
        if (cmp == 0)
        {
            return -1;                                                      // word already in array -> continue
        }
        else if (cmp > 0)                                                   // word should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0)                                                   // word should go before mid
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
    return -1;                                                              // word already in array -> continue
}

HashTable :: HashTable()
{
    this->buckets = new word*[MAX_BUCKETS]();                               // pointers to buckets
    this->wordsPerBucket = new int[MAX_BUCKETS]();                         
    //std::cout << "Hash Table is created!" << std::endl;
}
unsigned long HashTable :: addToBucket(unsigned long hash, const word w)
{
    //cout << "hash = " << hash << endl;
    if (w == NULL)
    {
        throw std::invalid_argument( "Got NULL pointer");
    }
    int i;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new word[WORDS_PER_BUCKET]();                             // create bucket
        for (i = 0; i < WORDS_PER_BUCKET; i++)
        {
            this->buckets[hash][i] = new char[MAX_WORD_LENGTH];
        }
        strcpy(this->buckets[hash][0], w);                                              // first word in bucket
        this->wordsPerBucket[hash]++;    
        //cout << "Bucket no " << hash << " is created" << endl;
        return hash;
    }
    if (this->wordsPerBucket[hash] % WORDS_PER_BUCKET == 0)                             // have reached limit of bucket
    {
        word* resized = new word[this->wordsPerBucket[hash] + WORDS_PER_BUCKET]();      // create bigger bucket
        for (i = 0; i <  this->wordsPerBucket[hash] + WORDS_PER_BUCKET; i++)
        {
            resized[i] = new char[MAX_WORD_LENGTH];
            if (i < this->wordsPerBucket[hash])
            {
                strcpy(resized[i], this->buckets[hash][i]);                            // copy words accumulated until now
            }
        }
        delete [] this->buckets[hash];

        this->buckets[hash] = resized;                                                 // pointer to the new bigger bucket 
    }

    int pos = binarySearch(this->buckets[hash], 0, this->wordsPerBucket[hash] - 1, w);
    
    if (pos == -1)
    {
        return hash;
    }
    //cout << pos << endl;

    if (this->wordsPerBucket[hash] >= pos)                             // in the middle of the array
    {
        for (i = this->wordsPerBucket[hash]; i > pos; i--)
        {
            strcpy(this->buckets[hash][i], this->buckets[hash][i-1]); 
        }
    }
    strcpy(this->buckets[hash][pos], w);                              // new node in array
    this->wordsPerBucket[hash]++; 
    return hash;
}
void HashTable :: addWordsToIndex()
{
    entry* e = NULL;
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)            // each bucket
    {
        if (this->wordsPerBucket[bucket] > 0)
        {
            for (int word = 0; word < this->wordsPerBucket[bucket]; word++) 
            {
                create_entry(&this->buckets[bucket][word], &e);
                add_entry(EntryList, e);
                //add entry to index   // build or lookup
            }
        }
    }
    //EntryList->printList();

}
const int HashTable :: getWordsPerBucket(unsigned long hash) const
{
    return this->wordsPerBucket[hash];
}

const word* HashTable :: getBucket(unsigned long hash) const
{
    return this->buckets[hash];
}
void HashTable :: printBucket(unsigned long hash) const
{
    std::cout << "Bucket no " << hash << std::endl;
    for (int i = 0; i < this->wordsPerBucket[hash]; i++)
    {
        std::cout << this->buckets[hash][i] << std::endl;
    }
    std::cout << "######################" << std::endl;
} 

void HashTable :: printTable() const
{
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)
    {
        if (this->wordsPerBucket[bucket] > 0)
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int word = 0; word < this->wordsPerBucket[bucket]; word++) 
            {
                std::cout << this->buckets[bucket][word] << std::endl;
            }
            std::cout << "######################" << std::endl;
        }
    }
}
HashTable :: ~HashTable()
{
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)
    {
        if (this->buckets[bucket] != NULL)
        {
            int wordsNum = WORDS_PER_BUCKET + (this->wordsPerBucket[bucket] / WORDS_PER_BUCKET) * WORDS_PER_BUCKET; // figure out how many words should be deleted
            if (this->wordsPerBucket[bucket] % WORDS_PER_BUCKET == 0)
            {
                wordsNum = (this->wordsPerBucket[bucket] / WORDS_PER_BUCKET) * WORDS_PER_BUCKET;
            }
            for (int w = 0; w < wordsNum; w++)
            {
                delete [] this->buckets[bucket][w];                         // delete words
            }
            delete [] this->buckets[bucket];                                // delete buckets
        }
        //cout << "Bucket no " << bucket << " is deleted !" << endl;
    }
    delete[] this->wordsPerBucket;
    delete[] this->buckets;                                                 // delete hash table
    //std::cout << "Hash Table is deleted!" << std::endl;
}

unsigned long hashFunction(word str)                                        // a typical hash function
{
    if (str == NULL || strcmp(str, " ") == 0)
    {
        throw std::invalid_argument( "Got NULL pointer");
    }
    unsigned long hash = 5381;
    int character;

    while ((character = *str++))
    {
        hash = ((hash << 5) + hash) + character; 
    }
    return hash % MAX_BUCKETS;
}

////////////////////////////////////////////////////////////////////////////////////

HashTable* DocumentDeduplication(Document* d, HashTable* HT)
{
    if (d == NULL)
    {
        throw std::invalid_argument( "Got NULL pointer");
    }
    for (int i = 0; i < MAX_DOC_WORDS; i ++)                            // push each word to hash table
    {
        const word w = d->getWord(i);
        //cout << w << endl;
        if ( w == NULL)
        {
            break;
        }
        HT->addToBucket(hashFunction(w), w);
    }
    return HT;
    //HT.printTable();
}

HashTable* QueryDeduplication(Query* q, HashTable* HT)
{

    if (q == NULL)
    {
        throw std::invalid_argument( "Got NULL pointer");
    }
    for (int i = 0; i < MAX_QUERY_WORDS; i ++)                            // push each word to hash table
    {
        const word w = q->getWord(i);
        //cout << w << endl;
        if ( w == NULL)
        {
            break;
        }
        HT->addToBucket(hashFunction(w), w);
    }
    return HT;
    //HT.printTable();
}

//////////////////////////////////////////////////////////////////////////

int FindQuery(Query** queries, int left, int right, const QueryID id)
{
    if (queries == NULL)
    {
        throw std::invalid_argument( "Got NULL pointer");
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
            return FindQuery(queries, left, mid - 1, id);
        }
        return FindQuery(queries, mid + 1, right, id);
    }
    // not in array
    return -1;

}

int QuerybinarySearch(Query** queries, int left, int right, const QueryID id)
{
    if (queries == NULL)
    {
        throw std::invalid_argument( "Got NULL pointer");
    }
    int mid;
    while (left < right) 
    {
        mid = left + (right - left) / 2;
        if (id == queries[mid]->getId())
        {
            return -1;                                                      // query already in array -> continue
        }
        else if (id > queries[mid]->getId())                                                   // query should go after mid
        {
            left = mid + 1;
        }
        else if (id < queries[mid]->getId())                                                   // query should go before mid
        {
            right = mid - 1;
        }
    }
    if (id == queries[left]->getId())
    {
        return -1;
    }
    if (id > queries[left]->getId())
    {
        return left + 1;
    }
    else if (id < queries[left]->getId())
    {
        return left;
    }
    return -1;                                                              // query already in array -> continue
}

QueryTable :: QueryTable()
{
    this->buckets = new Query**[MAX_QUERY_BUCKETS]();                               // pointers to buckets / arrays of query pointers -> ***
    this->QueriesPerBucket = new int[MAX_QUERY_BUCKETS]();                         
    //std::cout << "Query Table is created!" << std::endl;
}

unsigned long QueryTable :: addToBucket(unsigned long hash, Query* q)
{
    int i;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new Query*[QUERIES_PER_BUCKET]();                               // create bucket
        this->buckets[hash][0] = q;                                                           // first query in bucket
        this->QueriesPerBucket[hash]++;   
        //std::cout << "Bucket no " << hash << " is created" << std::endl;
        //std::cout << "Query no " << this->buckets[hash][0]->getId() << " is created" << std::endl;
        return hash;
    }
    if (this->QueriesPerBucket[hash] % QUERIES_PER_BUCKET == 0)                             // have reached limit of bucket
    {
        Query** resized = new Query*[this->QueriesPerBucket[hash] + QUERIES_PER_BUCKET]();      // create bigger bucket
        for (i = 0; i < this->QueriesPerBucket[hash]; i++)
        {
            resized[i] = this->buckets[hash][i];                            // copy queries accumulated until now
        }
        delete[] this->buckets[hash];                                       // delete bucket
        this->buckets[hash] = resized;                                                 // pointer to the new bigger bucket 

    }
    int pos = QuerybinarySearch(this->buckets[hash], 0, this->QueriesPerBucket[hash] - 1, q->getId());
    //std::cout << "pos = " << std::endl;
    if (pos == -1)
    {
        delete q;
        return hash;
    }
    if (this->QueriesPerBucket[hash] >= pos)                             // in the middle of the array
    {
        for (i = this->QueriesPerBucket[hash]; i > pos; i--)
        {
            this->buckets[hash][i] = this->buckets[hash][i-1]; 
        }
    }
    this->buckets[hash][pos] = q;                              // new node in array
    this->QueriesPerBucket[hash]++; 
    //std::cout << "Query no " << this->buckets[hash][0]->getId() << " is created" << std::endl;
    return hash;
}

const int QueryTable :: getQueriesPerBucket(unsigned long hash) const
{
    return this->QueriesPerBucket[hash];
}

Query** QueryTable :: getBucket(unsigned long hash) const
{
    return this->buckets[hash];
}

const Query* QueryTable :: getQuery(QueryID id) const
{
    unsigned long hash = hashFunctionById(id); 
    int pos = FindQuery(this->buckets[hash], 0, this->QueriesPerBucket[hash] - 1, id);
    if (pos == -1)
        return NULL;
    return this->buckets[hash][pos];
}

void QueryTable :: deleteQuery(QueryID id) const
{
    unsigned long hash = hashFunctionById(id); 
    int pos = FindQuery(this->buckets[hash], 0, this->QueriesPerBucket[hash] - 1, id);
    delete this->buckets[hash][pos];
    if (this->QueriesPerBucket[hash] >= pos)                             // in the middle of the array
    {
        for (int i = pos; i < this->QueriesPerBucket[hash]; i++)
        {
            this->buckets[hash][i] = this->buckets[hash][i+1]; 
        }
    }
    this->QueriesPerBucket[hash]--;

}
void QueryTable :: printBucket(unsigned long hash) const
{
    std::cout << "Bucket no " << hash << std::endl;
    for (int i = 0; i < this->QueriesPerBucket[hash]; i++)
    {
        std::cout << this->buckets[hash][i]->getWord(0) << std::endl;
    }
    std::cout << "######################" << std::endl;
} 

void QueryTable :: printTable() const
{
    for (int bucket = 0; bucket < MAX_QUERY_BUCKETS; bucket++)
    {
        if (this->QueriesPerBucket[bucket] > 0)
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int q = 0; q < this->QueriesPerBucket[bucket]; q++) 
            {
                std::cout << this->buckets[bucket][q]->getWord(0) << std::endl;
            }
            std::cout << "######################" << std::endl;
        }
    }
}
QueryTable :: ~QueryTable()
{
    for (int bucket = 0; bucket < MAX_QUERY_BUCKETS; bucket++)
    {
        for (int q = 0; q < this->QueriesPerBucket[bucket]; q++)
        {
            //std::cout << "Query no " << this->buckets[bucket][q]->getId() << " is deleted!" << std::endl;
            delete this->buckets[bucket][q];                                // delete queries
        }
        if (this->buckets[bucket] != NULL)
        {   
            delete[] this->buckets[bucket];                                       // delete buckets
            //std::cout << "Bucket no " << bucket << " is deleted!" << std::endl;
        }
    }
    delete[] this->QueriesPerBucket;
    delete[] this->buckets;                                                 // delete hash table
    //std::cout << "Query Table is deleted!" << std::endl;
}

unsigned long hashFunctionById(QueryID id)                                        // a typical hash function
{
    unsigned long hash = ((id >> 16) ^ id) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = (hash >> 16) ^ hash;
    return hash % MAX_QUERY_BUCKETS;
}
