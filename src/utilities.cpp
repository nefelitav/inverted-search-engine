#include "../include/core.h"
#include "../include/utilities.hpp"
#include "../include/jobscheduler.hpp"

DocTable ::DocTable(DocID id)
{
    this->buckets = new word *[MAX_BUCKETS](); // pointers to buckets
    this->tableID = id;
    memset(this->wordsPerBucket, 0, MAX_BUCKETS * (sizeof(int)));
}

unsigned long DocTable ::addToBucket(unsigned long hash, const word w)
{
    if (w == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new word[WORDS_PER_BUCKET](); // create bucket
        for (i = 0; i < WORDS_PER_BUCKET; ++i)
        {
            this->buckets[hash][i] = new char[MAX_WORD_LENGTH];
        }
        strcpy(this->buckets[hash][0], w); // first word in bucket
        ++this->wordsPerBucket[hash];
        return hash;
    }
    if (this->wordsPerBucket[hash] % WORDS_PER_BUCKET == 0) // have reached limit of bucket
    {
        word *resized = new word[this->wordsPerBucket[hash] + WORDS_PER_BUCKET](); // create bigger bucket
        for (i = 0; i < this->wordsPerBucket[hash] + WORDS_PER_BUCKET; ++i)
        {
            resized[i] = new char[MAX_WORD_LENGTH];
            if (i < this->wordsPerBucket[hash])
            {
                strcpy(resized[i], this->buckets[hash][i]); // copy words accumulated until now
            }
        }

        delete[] this->buckets[hash];
        this->buckets[hash] = resized; // pointer to the new bigger bucket
    }

    int pos = binarySearch(this->buckets[hash], 0, this->wordsPerBucket[hash] - 1, w); // find where word should be added, so that the bucket is sorted

    if (pos == -1)
    {
        return hash;
    }

    if (this->wordsPerBucket[hash] >= pos) // in the middle of the array
    {
        for (i = this->wordsPerBucket[hash]; i > pos; i--)
        {
            strcpy(this->buckets[hash][i], this->buckets[hash][i - 1]); // shift
        }
    }
    strcpy(this->buckets[hash][pos], w); // new node in array
    ++this->wordsPerBucket[hash];
    return hash;
}

ErrorCode DocTable ::wordLookup() // search for every word of this doc, if they match with any query entries
{

    int size = 0;
    QueryID *matchedQueries;
    matchedQuery *curr;
    entry_list *result = NULL;
    create_entry_list(&result);
#if PARALLEL == 2 || PARALLEL == 4 || PARALLEL == 5 || PARALLEL == 7
    QueryTable *localTable = scheduler->getQueryTable(scheduler->convertId(pthread_self()));
#else
    QueryTable *localTable = QT;
#endif
    for (int bucket = 0; bucket < MAX_BUCKETS; ++bucket) // each bucket
    {
        if (this->wordsPerBucket[bucket])
        {
            for (int word = 0; word < this->wordsPerBucket[bucket]; ++word) // for each word -> search in all indices
            {
                ET->wordLookup(this->buckets[bucket][word], result); // exact match
                lookup_entry_index(this->buckets[bucket][word], result, MT_EDIT_DIST);
                lookup_entry_index(this->buckets[bucket][word], result, MT_HAMMING_DIST);
            }
        }
    }

    entry *curr_entry = result->getHead();
    payloadNode *curr_payloadnode = NULL;
    ResultTable *RT = new ResultTable();

    while (curr_entry) // get every entry of result list
    {
        curr_payloadnode = curr_entry->getPayload(); // get payload of every entry
        while (curr_payloadnode)
        {
            localTable->getQuery(curr_payloadnode->getId())->setTrue(curr_entry->getWord());         // get every query corresponding to queryid of payload and set to true
            RT->addToBucket(hashFunctionById(curr_payloadnode->getId()), curr_payloadnode->getId()); // add queryid to bucket -> deduplicate
            curr_payloadnode = curr_payloadnode->getNext();
        }
        curr_entry = curr_entry->getNext();
    }
    matchedQueryList *results = RT->checkMatch(localTable); // check which queries had all their entries matched, otherwise set to false, to continue process

    curr = results->getHead();
    while (curr)
    {
        ++size;
        curr = curr->getNext();
    }

    matchedQueries = new QueryID[size];
    curr = results->getHead();

    for (int i = 0; i < size; ++i)
    {
        matchedQueries[i] = curr->getId();
        curr = curr->getNext();
    }

    mergeSort(matchedQueries, 0, size - 1);
#if PARALLEL == 2 || PARALLEL == 4 || PARALLEL == 5 || PARALLEL == 7
    pthread_mutex_lock(&resultLock); // protect resultList, which is a global variable
    storeResult(size, this->tableID, matchedQueries);
    pthread_cond_signal(&resEmptyCond); // resultList is not empty and we can move on to getNextAvailRes
    pthread_mutex_unlock(&resultLock);
#else
    storeResult(size, this->tableID, matchedQueries);
#endif

    delete results;
    delete RT;
    destroy_entry_list(result);

    return EC_SUCCESS;
}
const int DocTable ::getWordsPerBucket(unsigned long hash) const
{
    return this->wordsPerBucket[hash];
}

const word *DocTable ::getBucket(unsigned long hash) const
{
    return this->buckets[hash];
}
void DocTable ::printBucket(unsigned long hash) const
{
    std::cout << "Bucket no " << hash << std::endl;
    for (int i = 0; i < this->wordsPerBucket[hash]; ++i)
    {
        std::cout << this->buckets[hash][i] << std::endl;
    }
    std::cout << "######################" << std::endl;
}

void DocTable ::printTable() const
{
    for (int bucket = 0; bucket < MAX_BUCKETS; ++bucket)
    {
        if (this->wordsPerBucket[bucket])
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int word = 0; word < this->wordsPerBucket[bucket]; ++word)
            {
                std::cout << this->buckets[bucket][word] << std::endl;
            }
            std::cout << "######################" << std::endl;
        }
    }
}
DocTable ::~DocTable()
{
    int bucket;
    int wordsNum;
    for (bucket = 0; bucket < MAX_BUCKETS; ++bucket)
    {
        if (this->wordsPerBucket[bucket])
        {
            wordsNum = WORDS_PER_BUCKET + (this->wordsPerBucket[bucket] / WORDS_PER_BUCKET) * WORDS_PER_BUCKET; // figure out how many words should be deleted
            if (this->wordsPerBucket[bucket] % WORDS_PER_BUCKET == 0)
            {
                wordsNum = (this->wordsPerBucket[bucket] / WORDS_PER_BUCKET) * WORDS_PER_BUCKET;
            }
            for (int w = 0; w < wordsNum; w++)
            {
                delete[] this->buckets[bucket][w]; // delete words
            }
            delete[] this->buckets[bucket]; // delete buckets
        }
    }
    delete[] this->buckets; // delete hash table
}

////////////////////////////////////////////////////////////////////////////////////

DocTable *DocumentDeduplication(Document *d, DocTable *HT)
{
    if (d == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    word w = d->getWord(0);
    int i = 0;
    while (w != NULL) // push each word to hash table
    {                 //std::cout << w << std::endl;
        HT->addToBucket(hashFunction(w), w);
        i++;
        w = d->getWord(i);
    }
    return HT; //HT.printTable();
}

//////////////////////////////////////////////////////////////////////////

QueryTable ::QueryTable()
{
    this->buckets = new Query **[MAX_QUERY_BUCKETS](); // pointers to buckets / arrays of query pointers -> ***
    memset(this->queriesPerBucket, 0, MAX_QUERY_BUCKETS * (sizeof(int)));
    for (int i = 0; i < MAX_QUERY_BUCKETS; i++)
    {
        this->bucketLock[i] = PTHREAD_MUTEX_INITIALIZER;
    }
}

QueryTable *QueryTable ::cloneQueryTable() // copy query table in local variable
{
    QueryTable *newQT = new QueryTable();
    for (int bucket = 0; bucket < MAX_QUERY_BUCKETS; ++bucket)
    {
        newQT->queriesPerBucket[bucket] = this->queriesPerBucket[bucket];

        if (this->queriesPerBucket[bucket])
        {
            newQT->buckets[bucket] = new Query *[QUERIES_PER_BUCKET]();
            for (int q = 0; q < this->queriesPerBucket[bucket]; q++)
            {
                newQT->buckets[bucket][q] = new Query(*this->buckets[bucket][q]);
            }
        }
    }
    return newQT;
}

unsigned long QueryTable ::addToBucket(unsigned long hash, Query *q)
{
    if (q == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i;
    pthread_mutex_lock(&(this->bucketLock[hash])); // lock bucket
    if (this->buckets[hash] == NULL)
    {

        this->buckets[hash] = new Query *[QUERIES_PER_BUCKET](); // create bucket
        memset(this->buckets[hash], 0, QUERIES_PER_BUCKET);
        this->buckets[hash][0] = q; // first query in bucket
        this->queriesPerBucket[hash]++;
        pthread_mutex_unlock(&(this->bucketLock[hash]));
        return hash;
    }
    if (this->queriesPerBucket[hash] % QUERIES_PER_BUCKET == 0) // have reached limit of bucket
    {
        Query **resized = new Query *[this->queriesPerBucket[hash] + QUERIES_PER_BUCKET](); // create bigger bucket
        for (i = 0; i < this->queriesPerBucket[hash]; ++i)
        {
            resized[i] = this->buckets[hash][i]; // copy queries accumulated until now
        }
        delete[] this->buckets[hash];  // delete bucket
        this->buckets[hash] = resized; // pointer to the new bigger bucket
    }
    int pos = queryBinarySearch(this->buckets[hash], 0, this->queriesPerBucket[hash] - 1, q->getId());
    if (pos == -1)
    {
        delete q;
        pthread_mutex_unlock(&(this->bucketLock[hash]));
        return hash;
    }

    if (this->queriesPerBucket[hash] > pos) // in the middle of the array
    {
        for (i = this->queriesPerBucket[hash]; i > pos; i--)
        {
            this->buckets[hash][i] = this->buckets[hash][i - 1];
            this->buckets[hash][i - 1] = NULL;
        }
    }

    this->buckets[hash][pos] = q; // new node in array
    this->queriesPerBucket[hash]++;
    pthread_mutex_unlock(&(this->bucketLock[hash]));
    return hash;
}

const int QueryTable ::getQueriesPerBucket(unsigned long hash) const
{
    return this->queriesPerBucket[hash];
}

Query **QueryTable ::getBucket(unsigned long hash) const
{
    return this->buckets[hash];
}

Query *QueryTable ::getQuery(QueryID id)
{
    unsigned long hash = hashFunctionById(id);
    int pos = findQuery(this->buckets[hash], 0, this->queriesPerBucket[hash] - 1, id);
    if (pos == -1)
        return NULL;
    return this->buckets[hash][pos];
}

void QueryTable ::deleteQuery(QueryID id)
{
    unsigned long hash = hashFunctionById(id);
    pthread_mutex_lock(&(this->bucketLock[hash]));                                     // lock bucket
    int pos = findQuery(this->buckets[hash], 0, this->queriesPerBucket[hash] - 1, id); // find query position
    for (int i = 0; i < MAX_QUERY_WORDS; ++i)                                          // remove every entry of this query from index
    {
        if (this->buckets[hash][pos]->getWord(i))
        {
            if (this->buckets[hash][pos]->getMatchingType() == MT_EDIT_DIST)
            {
                pthread_mutex_lock(&editLock);
                removeFromIndex(this->buckets[hash][pos]->getWord(i), id, this->buckets[hash][pos]->getMatchingType());
                pthread_mutex_unlock(&editLock);
            }
            else if (this->buckets[hash][pos]->getMatchingType() == MT_HAMMING_DIST)
            {
                pthread_mutex_lock(&hammingLock[strlen(this->buckets[hash][pos]->getWord(i)) - 4]);
                removeFromIndex(this->buckets[hash][pos]->getWord(i), id, this->buckets[hash][pos]->getMatchingType());
                pthread_mutex_unlock(&hammingLock[strlen(this->buckets[hash][pos]->getWord(i)) - 4]);
            }
            else if (this->buckets[hash][pos]->getMatchingType() == MT_EXACT_MATCH)
            {
                pthread_mutex_lock(&exactLock);
                removeFromIndex(this->buckets[hash][pos]->getWord(i), id, this->buckets[hash][pos]->getMatchingType());
                pthread_mutex_unlock(&exactLock);
            }
        }
    }
    delete this->buckets[hash][pos];
    this->buckets[hash][pos] = NULL;
    if (this->queriesPerBucket[hash] > pos) // in the middle of the array
    {
        for (int i = pos; i < this->queriesPerBucket[hash] - 1; ++i)
        {
            this->buckets[hash][i] = this->buckets[hash][i + 1]; // shift
            this->buckets[hash][i + 1] = NULL;
        }
    }
    this->queriesPerBucket[hash]--;
    pthread_mutex_unlock(&(this->bucketLock[hash]));
}
void QueryTable ::printBucket(unsigned long hash) const
{
    std::cout << "Bucket no " << hash << std::endl;
    for (int i = 0; i < this->queriesPerBucket[hash]; ++i)
    {
        std::cout << this->buckets[hash][i]->getWord(0) << std::endl;
    }
    std::cout << "######################" << std::endl;
}

void QueryTable ::printTable() const
{
    for (int bucket = 0; bucket < MAX_QUERY_BUCKETS; ++bucket)
    {
        if (this->queriesPerBucket[bucket])
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int q = 0; q < this->queriesPerBucket[bucket]; q++)
            {
                std::cout << this->buckets[bucket][q]->getWord(0) << std::endl;
            }
            std::cout << "######################" << std::endl;
        }
    }
}
QueryTable ::~QueryTable()
{
    for (int bucket = 0; bucket < MAX_QUERY_BUCKETS; ++bucket)
    {
        for (int q = 0; q < this->queriesPerBucket[bucket]; q++)
        {
            delete this->buckets[bucket][q]; // delete queries
        }
        delete[] this->buckets[bucket]; // delete buckets
    }
    delete[] this->buckets; // delete hash table
}

/////////////////////////////////////////////////////////////////////////////

matchedQuery ::matchedQuery(QueryID id)
{
    this->id = id;
    this->next = NULL;
}
void matchedQuery ::setNext(matchedQuery *next)
{
    this->next = next;
}
matchedQuery *matchedQuery ::getNext()
{
    return this->next;
}
QueryID matchedQuery ::getId()
{
    return this->id;
}

matchedQueryList ::matchedQueryList()
{
    this->head = NULL;
}
void matchedQueryList ::addToList(QueryID id)
{
    if (this->head == NULL)
    {
        this->head = new matchedQuery(id);
        return;
    }
    matchedQuery *new_head = new matchedQuery(id);
    matchedQuery *old_head = this->head;
    new_head->setNext(old_head);
    this->head = new_head;
}
void matchedQueryList ::printList()
{
    matchedQuery *curr = this->head;
    while (curr)
    {
        std::cout << curr->getId() << std::endl;
        curr = curr->getNext();
    }
}
matchedQuery *matchedQueryList ::getHead()
{
    return this->head;
}

matchedQueryList ::~matchedQueryList()
{
    matchedQuery *prev, *curr = this->head;
    while (curr)
    {
        prev = curr;
        curr = curr->getNext();
        delete prev;
    }
}

/////////////////////////////////////////////////////////////////////////////

EntryTable ::EntryTable() // struct used for exact match
{
    this->buckets = new entry **[MAX_BUCKETS](); // pointers to buckets
    memset(this->entriesPerBucket, 0, MAX_BUCKETS * (sizeof(int)));
}
unsigned long EntryTable ::addToBucket(unsigned long hash, entry *e)
{
    if (e == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new entry *[ENTRIES_PER_BUCKET](); // create bucket
        memset(this->buckets[hash], 0, ENTRIES_PER_BUCKET);
        this->buckets[hash][0] = e; // first entry in bucket
        this->entriesPerBucket[hash]++;
        return hash;
    }

    if (this->entriesPerBucket[hash] % ENTRIES_PER_BUCKET == 0) // have reached limit of bucket
    {
        entry **resized = new entry *[this->entriesPerBucket[hash] + ENTRIES_PER_BUCKET](); // create bigger bucket
        for (i = 0; i < this->entriesPerBucket[hash]; ++i)
        {
            resized[i] = this->buckets[hash][i];
        }
        delete[] this->buckets[hash];
        this->buckets[hash] = resized; // pointer to the new bigger bucket
    }

    int pos = entryBinarySearch(this->buckets[hash], 0, this->entriesPerBucket[hash] - 1, e->getWord());

    if (pos == -1)
    {
        return hash;
    }

    if (this->buckets[hash][pos] != NULL)
    {

        if (strcmp(this->buckets[hash][pos]->getWord(), e->getWord()) == 0) // already exists, add to payload
        {
            this->buckets[hash][pos]->addToPayload(e->getPayload()->getId(), e->getPayload()->getThreshold());
            return hash;
        }
    }

    if (this->entriesPerBucket[hash] > pos) // in the middle of the array
    {
        for (i = this->entriesPerBucket[hash]; i > pos; i--)
        {
            this->buckets[hash][i] = this->buckets[hash][i - 1];
            this->buckets[hash][i - 1] = NULL;
        }
    }
    this->buckets[hash][pos] = e; // new node in array
    this->entriesPerBucket[hash]++;
    return hash;
}

void EntryTable ::wordLookup(const word w, entry_list *result) // exact match lookup
{
    unsigned long hash = hashFunction(w);
    entry *tempEntry;
    payloadNode *currPayloadNode;
    int pos = -1;
    pos = findEntry(this->buckets[hash], 0, this->entriesPerBucket[hash] - 1, w); // find word if it exists
    if (pos == -1)
    {
        return;
    }
    if (this->buckets[hash][pos] && this->buckets[hash][pos]->getPayload())
    {
        create_entry(&w, &tempEntry);
        currPayloadNode = this->buckets[hash][pos]->getPayload();
        while (currPayloadNode) // add every query id of payload into result entrylist
        {
            tempEntry->addToPayload(currPayloadNode->getId(), currPayloadNode->getThreshold());
            currPayloadNode = currPayloadNode->getNext();
        }
        result->addEntry(tempEntry);
    }
}
const int EntryTable ::getEntriesPerBucket(unsigned long hash) const
{
    return this->entriesPerBucket[hash];
}

entry **EntryTable ::getBucket(unsigned long hash) const
{
    return this->buckets[hash];
}
void EntryTable ::printBucket(unsigned long hash) const
{
    payloadNode *curr;
    std::cout << "Bucket no " << hash << std::endl;
    for (int i = 0; i < this->entriesPerBucket[hash]; ++i)
    {
        std::cout << this->buckets[hash][i]->getWord() << std::endl;
        std::cout << "Query IDs :" << std::endl;
        curr = this->buckets[hash][i]->getPayload();
        while (curr)
        {
            std::cout << " - " << curr->getId() << std::endl;
            curr = curr->getNext();
        }
    }
    std::cout << "######################" << std::endl;
}

void EntryTable ::printTable() const
{
    for (int bucket = 0; bucket < MAX_BUCKETS; ++bucket)
    {
        if (this->entriesPerBucket[bucket])
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int e = 0; e < this->entriesPerBucket[bucket]; e++)
            {
                std::cout << this->buckets[bucket][e]->getWord() << std::endl;
                std::cout << "Query IDs :" << std::endl;
                this->buckets[bucket][e]->printPayload();
            }
            std::cout << "######################" << std::endl;
        }
    }
}
void EntryTable ::deleteQueryId(word givenWord, const QueryID queryId) // delete queryid from payload
{
    unsigned long hash = hashFunction(givenWord);                                             // find bucket
    int pos = findEntry(this->buckets[hash], 0, this->entriesPerBucket[hash] - 1, givenWord); // find record
    if(pos !=-1){
        this->buckets[hash][pos]->deletePayloadNode(queryId);
    }
    
}
EntryTable ::~EntryTable()
{
    for (int bucket = 0; bucket < MAX_BUCKETS; ++bucket)
    {
        if (this->buckets[bucket])
        {
            delete[] this->buckets[bucket]; // delete buckets
        }
    }
    delete[] this->buckets; // delete hash table
}

//////////////////////////////////////////////////////////////////////////

ResultTable ::ResultTable()
{
    this->buckets = new QueryID *[MAX_QUERY_BUCKETS](); // pointers to buckets / arrays of query pointers -> ***
    memset(this->queriesPerBucket, 0, MAX_QUERY_BUCKETS * (sizeof(int)));
}

unsigned long ResultTable ::addToBucket(unsigned long hash, QueryID id)
{
    int i;
    QueryID *resized;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new QueryID[QUERIES_PER_BUCKET](); // create bucket
        this->buckets[hash][0] = id;                             // first query in bucket
        this->queriesPerBucket[hash]++;
        return hash;
    }
    if (this->queriesPerBucket[hash] % QUERIES_PER_BUCKET == 0) // have reached limit of bucket
    {
        resized = new QueryID[this->queriesPerBucket[hash] + QUERIES_PER_BUCKET](); // create bigger bucket
        for (i = 0; i < this->queriesPerBucket[hash]; ++i)
        {
            resized[i] = this->buckets[hash][i]; // copy queries accumulated until now
        }
        delete[] this->buckets[hash];  // delete bucket
        this->buckets[hash] = resized; // pointer to the new bigger bucket
    }
    int pos = resultBinarySearch(this->buckets[hash], 0, this->queriesPerBucket[hash] - 1, id);
    if (pos == -1)
    {
        return hash;
    }
    if (this->queriesPerBucket[hash] >= pos) // in the middle of the array
    {
        for (i = this->queriesPerBucket[hash]; i > pos; i--)
        {
            this->buckets[hash][i] = this->buckets[hash][i - 1];
        }
    }
    this->buckets[hash][pos] = id; // new node in array
    this->queriesPerBucket[hash]++;
    return hash;
}

const int ResultTable ::getQueriesPerBucket(unsigned long hash) const
{
    return this->queriesPerBucket[hash];
}

QueryID *ResultTable ::getBucket(unsigned long hash) const
{
    return this->buckets[hash];
}

void ResultTable ::printBucket(unsigned long hash) const
{
    std::cout << "Bucket no " << hash << std::endl;
    for (int i = 0; i < this->queriesPerBucket[hash]; ++i)
    {
        std::cout << this->buckets[hash][i] << std::endl;
    }
    std::cout << "######################" << std::endl;
}

void ResultTable ::printTable() const
{
    for (int bucket = 0; bucket < MAX_QUERY_BUCKETS; ++bucket)
    {
        if (this->queriesPerBucket[bucket])
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int q = 0; q < this->queriesPerBucket[bucket]; q++)
            {
                std::cout << this->buckets[bucket][q] << std::endl;
            }
            std::cout << "######################" << std::endl;
        }
    }
}

matchedQueryList *ResultTable ::checkMatch(QueryTable *tempTable)
{
    int bucket, q;
    matchedQueryList *results = new matchedQueryList();
    for (bucket = 0; bucket < MAX_QUERY_BUCKETS; ++bucket)
    {
        if (this->queriesPerBucket[bucket])
        {
            for (q = 0; q < this->queriesPerBucket[bucket]; q++)
            {
                if (!tempTable->getQuery(this->buckets[bucket][q])->matched())
                {
                    tempTable->getQuery(this->buckets[bucket][q])->setFalse();
                }
                else // its a match!
                {
                    results->addToList(this->buckets[bucket][q]);
                    tempTable->getQuery(this->buckets[bucket][q])->setFalse();
                }
            }
        }
    }
    return results;
}
ResultTable ::~ResultTable()
{
    for (int bucket = 0; bucket < MAX_QUERY_BUCKETS; ++bucket)
    {
        if (this->buckets[bucket])
        {
            delete[] this->buckets[bucket]; // delete buckets
        }
    }
    delete[] this->buckets; // delete hash table
}

///////////////////////////////////////////////////////////////////////

result ::result(int numRes, DocID document, QueryID *queries)
{
    this->doc_id = document;
    this->num_res = numRes;
    this->query_ids = queries;
    this->next = NULL;
}

void result ::addResult(result *toAdd)
{
    if (this->next)
    {
        this->next->addResult(toAdd);
        return;
    }
    this->next = toAdd;
}

result *result ::getNext()
{
    return this->next;
}

int result ::getNumRes()
{
    return this->num_res;
}

DocID result ::getDocID()
{
    return this->doc_id;
}

QueryID *result ::getQueries()
{
    return this->query_ids;
}

void storeResult(int numRes, DocID document, QueryID *queries)
{
    result *temp = NULL;
    if (resultList == NULL)
    {
        // If the list is empty create a new node at the head
        resultList = new result(numRes, document, queries);
        unfinishedDocs--; // match document procedure is done for this doc and we can move on to getNextAvailRes
        return;
    }
    // Else insert a node
    temp = new result(numRes, document, queries);
    resultList->addResult(temp);
    unfinishedDocs--;
}
