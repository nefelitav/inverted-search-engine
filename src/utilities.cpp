#include "../include/core.h"
#include "../include/utilities.hpp"

DocTable ::DocTable(DocID id)
{
    this->buckets = new word*[MAX_BUCKETS](); // pointers to buckets
    this->tableID = id;
    memset(this->wordsPerBucket, 0, MAX_BUCKETS*(sizeof(int)));
    //std::cout << "Hash Table is created!" << std::endl;
}
unsigned long DocTable ::addToBucket(unsigned long hash, const word w)
{
    //cout << "hash = " << hash << endl;
    if (!w)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i;
    if (!this->buckets[hash])
    {
        this->buckets[hash] = new word[WORDS_PER_BUCKET](); // create bucket
        for (i = 0; i < WORDS_PER_BUCKET; ++i)
        {
            this->buckets[hash][i] = new char[MAX_WORD_LENGTH];
        }
        strcpy(this->buckets[hash][0], w); // first word in bucket
        ++this->wordsPerBucket[hash];
        //cout << "Bucket no " << hash << " is created" << endl;
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
    //cout << pos << endl;

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
void DocTable ::wordLookup() // search for every word of this doc, if they match with any query entries
{
    int size = 0;
    QueryID *matchedQueries;
    matchedQuery *curr;
    entry_list *result = NULL;
    create_entry_list(&result);

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
            QT->getQuery(curr_payloadnode->getId())->setTrue(curr_entry->getWord());                 // get every query corresponding to queryid of payload and set to true
            RT->addToBucket(hashFunctionById(curr_payloadnode->getId()), curr_payloadnode->getId()); // add queryid to bucket -> deduplicate
            curr_payloadnode = curr_payloadnode->getNext();
        }
        curr_entry = curr_entry->getNext();
    }
    matchedQueryList *results = RT->checkMatch(); // check which queries had all their entries matched, otherwise set to false, to continue process

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

    storeResult(size, this->tableID, matchedQueries);

    //RT->printTable();
    //results->printList();
    delete results;
    delete RT;
    destroy_entry_list(result);
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
            //cout << "Bucket no " << bucket << " is deleted !" << endl;
        }
        
    }
    delete[] this->buckets; // delete hash table
    //std::cout << "Hash Table is deleted!" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////

DocTable *DocumentDeduplication(Document *d, DocTable *HT)
{
    int i;
    if (!d)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    for (i = 0; i < MAX_DOC_WORDS; ++i) // push each word to hash table
    {
        const word w = d->getWord(i);
        //cout << w << endl;
        if (!w)
        {
            break;
        }
        HT->addToBucket(hashFunction(w), w);
    }
    return HT;
    //HT.printTable();
}

//////////////////////////////////////////////////////////////////////////

QueryTable ::QueryTable()
{
    this->buckets = new Query **[MAX_QUERY_BUCKETS](); // pointers to buckets / arrays of query pointers -> ***
    memset(this->queriesPerBucket, 0, MAX_QUERY_BUCKETS*(sizeof(int)));
    //std::cout << "Query Table is created!" << std::endl;
}

unsigned long QueryTable ::addToBucket(unsigned long hash, Query *q)
{
    if (!q)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i;
    if (!this->buckets[hash])
    {
        this->buckets[hash] = new Query *[QUERIES_PER_BUCKET](); // create bucket
        this->buckets[hash][0] = q;                              // first query in bucket
        this->queriesPerBucket[hash]++;
        //std::cout << "Bucket no " << hash << " is created" << std::endl;
        //std::cout << "Query no " << this->buckets[hash][0]->getId() << " is created" << std::endl;
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
    //std::cout << "pos = " << std::endl;
    if (pos == -1)
    {
        delete q;
        return hash;
    }
    if (this->queriesPerBucket[hash] >= pos) // in the middle of the array
    {
        for (i = this->queriesPerBucket[hash]; i > pos; i--)
        {
            this->buckets[hash][i] = this->buckets[hash][i - 1];
        }
    }
    this->buckets[hash][pos] = q; // new node in array
    this->queriesPerBucket[hash]++;
    //std::cout << "Query no " << this->buckets[hash][0]->getId() << " is created" << std::endl;
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
    int pos = findQuery(this->buckets[hash], 0, this->queriesPerBucket[hash] - 1, id); // find query position
    // std::cout << hash << " " << pos << std::endl;
    for (int i = 0; i < MAX_QUERY_WORDS; ++i) // remove every entry of this query from index
    {
        if (this->buckets[hash][pos]->getWord(i))
        {
            // std::cout << this->buckets[hash][pos]->getWord(i) <<  std::endl;
            removeFromIndex(this->buckets[hash][pos]->getWord(i), id, this->buckets[hash][pos]->getMatchingType());
        }
    }
    //ET->printTable();
    delete this->buckets[hash][pos];
    if (this->queriesPerBucket[hash] >= pos) // in the middle of the array
    {
        for (int i = pos; i < this->queriesPerBucket[hash]; ++i)
        {
            this->buckets[hash][i] = this->buckets[hash][i + 1]; // shift
        }
    }
    this->queriesPerBucket[hash]--;
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
            //std::cout << "Query no " << this->buckets[bucket][q]->getId() << " is deleted!" << std::endl;
            delete this->buckets[bucket][q]; // delete queries
        }
        delete[] this->buckets[bucket]; // delete buckets
        //std::cout << "Bucket no " << bucket << " is deleted!" << std::endl;
    }
    delete[] this->buckets; // delete hash table
    //std::cout << "Query Table is deleted!" << std::endl;
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
    if (!this->head)
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
void matchedQueryList ::removeQuery(matchedQuery *toRemove)
{
    matchedQuery *curr = this->head;
    matchedQuery *next = curr->getNext();
    if (curr == toRemove)
    {
        delete curr;
        this->head = next;
        return;
    }
    while (curr)
    {
        if (curr->getNext() == toRemove)
        {
            curr->setNext(curr->getNext()->getNext());
            delete toRemove;
            return;
        }
        curr = curr->getNext();
    }
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
    memset(this->entriesPerBucket, 0, MAX_BUCKETS*(sizeof(int)));
    //std::cout << "Hash Table is created!" << std::endl;
}
unsigned long EntryTable ::addToBucket(unsigned long hash, entry *e)
{
    // std::cout << "----------hash = " << hash << std::endl;
    if (!e)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    int i;
    if (!this->buckets[hash])
    {
        this->buckets[hash] = new entry *[ENTRIES_PER_BUCKET](); // create bucket
        this->buckets[hash][0] = e;                              // first entry in bucket
        this->entriesPerBucket[hash]++;
        // std::cout << "Bucket no " << hash << " is created" << std::endl;
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
    //std::cout << e->getWord() << "pos = "<< pos << std::endl;

    if (!strcmp(this->buckets[hash][pos]->getWord(), e->getWord()))
    {
        this->buckets[hash][pos]->addToPayload(e->getPayload()->getId(), e->getPayload()->getThreshold());
        return hash;
    }

    if (this->entriesPerBucket[hash] >= pos) // in the middle of the array
    {
        for (i = this->entriesPerBucket[hash]; i > pos; i--)
        {
            this->buckets[hash][i] = this->buckets[hash][i - 1];
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
    this->buckets[hash][pos]->deletePayloadNode(queryId);
}
EntryTable ::~EntryTable()
{
    for (int bucket = 0; bucket < MAX_BUCKETS; ++bucket)
    {
        if (this->buckets[bucket])
        {
            delete[] this->buckets[bucket]; // delete buckets
            // std::cout << "Bucket no " << bucket << " is deleted!" << std::endl;
        }
    }
    delete[] this->buckets; // delete hash table
    // std::cout << "Hash Table is deleted!" << std::endl;
}

//////////////////////////////////////////////////////////////////////////



ResultTable ::ResultTable()
{
    this->buckets = new QueryID *[MAX_QUERY_BUCKETS](); // pointers to buckets / arrays of query pointers -> ***
    memset(this->queriesPerBucket, 0, MAX_QUERY_BUCKETS*(sizeof(int)));
    //std::cout << "Result Table is created!" << std::endl;
}

unsigned long ResultTable ::addToBucket(unsigned long hash, QueryID id)
{
    int i;
    QueryID *resized;
    if (!this->buckets[hash])
    {
        this->buckets[hash] = new QueryID[QUERIES_PER_BUCKET](); // create bucket
        this->buckets[hash][0] = id;                             // first query in bucket
        this->queriesPerBucket[hash]++;
        //std::cout << "Bucket no " << hash << " is created" << std::endl;
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
    //std::cout << "pos = " << std::endl;
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
    //std::cout << "QueryID no " << this->buckets[hash][0]->getId() << " is created" << std::endl;
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

void ResultTable :: printBucket(unsigned long hash) const
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

matchedQueryList *ResultTable ::checkMatch()
{
    int bucket, q;
    matchedQueryList *results = new matchedQueryList();
    for (bucket = 0; bucket < MAX_QUERY_BUCKETS; ++bucket)
    {
        if (this->queriesPerBucket[bucket])
        {
            for (q = 0; q < this->queriesPerBucket[bucket]; q++)
            {
                //std::cout << this->buckets[bucket][q] << std::endl;
                if (!QT->getQuery(this->buckets[bucket][q])->matched())
                {
                    QT->getQuery(this->buckets[bucket][q])->setFalse();
                }
                else // its a match!
                {
                    //std::cout<<"MATCH "<<this->buckets[bucket][q]<<"\n";
                    results->addToList(this->buckets[bucket][q]);
                    QT->getQuery(this->buckets[bucket][q])->setFalse();
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
            //std::cout << "Bucket no " << bucket << " is deleted!" << std::endl;
        }
    }
    delete[] this->buckets; // delete hash table
    //std::cout << "Query Table is deleted!" << std::endl;
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
    if (!resultList)
    {
        resultList = new result(numRes, document, queries);
        return;
    }
    temp = new result(numRes, document, queries);
    resultList->addResult(temp);
}