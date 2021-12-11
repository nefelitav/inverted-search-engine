#include "../include/core.h"
#include "../include/utilities.hpp"



bool exactMatch(const word word1, const word word2) 
{
    if (word1 == NULL || word2 == NULL) 
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    return (strcmp(word1,word2) == 0);
}

int hammingDistance(const word word1, const word word2) 
{
    if (word1 == NULL || word2 == NULL) 
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i = 0;

    // If words have different length raise exception
    int diff = abs((int)strlen(word1) - (int)strlen(word2));
    if (diff != 0) 
    {
        throw std::invalid_argument("Words Have Different Length");
    }

    // Difference in the common part of the words
    while (word1[i] != '\0' && word2[i] != '\0' ) 
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

int editDistance(const word word1, const word word2) 
{
    if (word1 == NULL || word2 == NULL) 
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int final_distance;
    int size1 = 0;
    int size2 = 0;
    int substitution;
    int **d;

    // if equal, just stop
    if (strcmp(word1, word2) == 0) 
    {
        return 0;
    }
    
    // Get the size of each word to create the matrix
    size1 = strlen(word1);
    size2 = strlen(word2);

    // Allocate the matrix with dims Size1*Size2
    d = new int*[size1 + 1];
    for (int i = 0; i < size1 + 1; i++) 
    {
        d[i] = new int[size2 + 1]();
    }

    // Initialize border elements
    for (int i = 0; i < size2; i++) 
    {
        d[0][i] = i;
    }
    for (int i = 0; i < size1; i++) 
    { 
        d[i][0] = i;
    }
    
    // Calculate the minimum number of differences with the full-matrix iterative method
    for (int j = 1; j < size2 + 1; j++) 
    {
        for (int i = 1; i < size1 + 1; i++)
        {
            if (word1[i-1] == word2[j-1]) 
            {
                substitution = 0;
            } else {
                substitution = 1;
            }
            d[i][j] = std::min(std::min(d[i-1][j] + 1, d[i][j-1] + 1), d[i-1][j-1] + substitution);
        }
    }
    final_distance = d[size1][size2];

    // Delete the matrix
    for (int i = 0; i < size1 + 1; i++) 
    {
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
        throw std::invalid_argument("Got NULL pointer");
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
        throw std::invalid_argument("Got NULL pointer");
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
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)            // each bucket
    {
        if (this->wordsPerBucket[bucket] > 0)
        {
            for (int word = 0; word < this->wordsPerBucket[bucket]; word++) 
            {
                //add words to index   // lookup
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

////////////////////////////////////////////////////////////////////////////////////

HashTable* DocumentDeduplication(Document* d, HashTable* HT)
{
    if (d == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    for (int i = 0; i < MAX_DOC_WORDS; i ++)                            // push each word to hash table
    {
        const word w = d->getWord(i);
        //cout << w << endl;
        if (w == NULL)
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
        throw std::invalid_argument("Got NULL pointer");
    }
    int mid, cmp;
    while (left < right) 
    {
        mid = left + (right - left) / 2;
        cmp = id - queries[mid]->getId();
        if (cmp == 0)
        {
            return -1;                                                      // query already in array -> continue
        }
        else if (cmp > 0)                                                   // query should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0)                                                   // query should go before mid
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
    if (q == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
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
    // std::cout << hash << " " << pos << std::endl;
    for (int i = 0; i < MAX_QUERY_WORDS; i++) // remove every entry of this query from index
    {
        if (this->buckets[hash][pos]->getWord(i) != NULL)
        {
            // std::cout << this->buckets[hash][pos]->getWord(i) <<  std::endl;
            removeFromIndex(this->buckets[hash][pos]->getWord(i), id, this->buckets[hash][pos]->getMatchingType());
        }
    }
    //ET->printTable();
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

/////////////////////////////////////////////////////////////////////////////

int FindEntry(entry** entries, int left, int right, const word w)
{
    if (entries == NULL || w == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
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
            return FindEntry(entries, left, mid - 1, w);
        }
        return FindEntry(entries, mid + 1, right, w);
    }
    // not in array
    return -1;

}


int entrybinarySearch(entry** entries, int left, int right, const word w)
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
            return mid;                                                      // entry already in array -> add queryid to payload
        }
        else if (cmp > 0)                                                   // entry should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0)                                                   // entry should go before mid
        {
            right = mid - 1;
        }
    }
    cmp = strcmp(w, entries[left]->getWord());
    if (cmp > 0)
    {
        return left + 1;
    }
    else 
    {
        return left;
    }
}

EntryTable :: EntryTable()
{
    this->buckets = new entry**[MAX_ENTRY_BUCKETS]();                               // pointers to buckets
    this->entriesPerBucket = new int[MAX_ENTRY_BUCKETS]();                         
    //std::cout << "Hash Table is created!" << std::endl;
}
unsigned long EntryTable :: addToBucket(unsigned long hash, entry* e)
{
    // std::cout << "----------hash = " << hash << std::endl;
    if (e == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new entry*[ENTRIES_PER_BUCKET]();                             // create bucket
        this->buckets[hash][0] = e;                                                           // first entry in bucket
        this->entriesPerBucket[hash]++;    
        // std::cout << "Bucket no " << hash << " is created" << std::endl;
        return hash;
    }

    if (this->entriesPerBucket[hash] % ENTRIES_PER_BUCKET == 0)                             // have reached limit of bucket
    {
        entry** resized = new entry*[this->entriesPerBucket[hash] + ENTRIES_PER_BUCKET]();      // create bigger bucket
        for (i = 0; i <  this->entriesPerBucket[hash]; i++)
        {
            resized[i] = this->buckets[hash][i];
        }
        delete[] this->buckets[hash];
        this->buckets[hash] = resized;                                                 // pointer to the new bigger bucket 
    }

    int pos = entrybinarySearch(this->buckets[hash], 0, this->entriesPerBucket[hash] - 1, e->getWord());
    
    if (pos == -1)
    {
        return hash;
    }
    //std::cout << e->getWord() << "pos = "<< pos << std::endl;

    if (strcmp(this->buckets[hash][pos]->getWord(), e->getWord()) == 0)
    {
        this->buckets[hash][pos]->addToPayload(e->getPayload()->getId(), e->getPayload()->getThreshold());
        return hash;
    }

    if (this->entriesPerBucket[hash] >= pos)                             // in the middle of the array
    {
        for (i = this->entriesPerBucket[hash]; i > pos; i--)
        {
            this->buckets[hash][i] = this->buckets[hash][i-1]; 
        }
    }
    this->buckets[hash][pos] = e;  // new node in array
    this->entriesPerBucket[hash]++; 
    return hash;
}

const int EntryTable :: getEntriesPerBucket(unsigned long hash) const
{
    return this->entriesPerBucket[hash];
}

entry** EntryTable :: getBucket(unsigned long hash) const
{
    return this->buckets[hash];
}
void EntryTable :: printBucket(unsigned long hash) const
{
    payloadNode *curr;
    std::cout << "Bucket no " << hash << std::endl;
    for (int i = 0; i < this->entriesPerBucket[hash]; i++)
    {
        std::cout << this->buckets[hash][i]->getWord() << std::endl;
        std::cout << "Query IDs :" << std::endl;
        curr = this->buckets[hash][i]->getPayload();
        while (curr != NULL) 
        {
            std::cout << " - "<< curr->getId() << std::endl;
            curr = curr->getNext();
        }
    }
    std::cout << "######################" << std::endl;
} 

void EntryTable :: printTable() const
{
    payloadNode *curr;
    for (int bucket = 0; bucket < MAX_ENTRY_BUCKETS; bucket++)
    {
        if (this->entriesPerBucket[bucket] > 0)
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int e = 0; e < this->entriesPerBucket[bucket]; e++) 
            {
                std::cout << this->buckets[bucket][e]->getWord() << std::endl;
                std::cout << "Query IDs :" << std::endl;
                curr = this->buckets[bucket][e]->getPayload();
                while (curr != NULL) 
                {
                    std::cout << " - "<< curr->getId() << std::endl;
                    curr = curr->getNext();
                }
            }
            std::cout << "######################" << std::endl;
        }
    }
}
void EntryTable :: deleteQueryId(word givenWord, const QueryID queryId)
{
    unsigned long hash = hashFunction(givenWord);  // find bucket
    // std::cout << givenWord << " " << hash << std::endl;
    int pos = FindEntry(this->buckets[hash], 0, this->entriesPerBucket[hash] - 1, givenWord); // find record
    // std::cout << "pos = " << pos << std::endl;
    this->buckets[hash][pos]->deletePayloadNode(queryId);  // delete queryid from payload
    // if (this->buckets[hash][pos]->EmptyPayload())
    // {
    //     // std::cout << "-------------" << std::endl;
    //     EntryList->removeEntry(this->buckets[hash][pos]);
    // //     this->buckets[hash][pos] = NULL;
    // //     if (this->entriesPerBucket[hash] >= pos)                             // in the middle of the array
    // //     {
    // //         for (int i = pos; i < this->entriesPerBucket[hash]; i++)
    // //         {
    // //             this->buckets[hash][i] = this->buckets[hash][i+1]; 
    // //         }
    // //     }
    // //     this->entriesPerBucket[hash]--;
    // }

}
EntryTable :: ~EntryTable()
{
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)
    {
        if (this->buckets[bucket] != NULL)
        {   
            delete[] this->buckets[bucket];                                       // delete buckets
            // std::cout << "Bucket no " << bucket << " is deleted!" << std::endl;
        }                             
    }
    delete[] this->entriesPerBucket;
    delete[] this->buckets;                                                 // delete hash table
    // std::cout << "Hash Table is deleted!" << std::endl;
}

