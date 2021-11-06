#include "tests.hpp"

// Structs
void test_query(void)
{
    // too large / small word in the input and too many words given
    char *q_words = new char[MAX_QUERY_LENGTH]();
    memcpy(q_words, "hello    people how hellopeoplehellopeoplehellopeople things people how", MAX_QUERY_LENGTH);
    Query q(q_words, 0);
	//q.printQuery();

    TEST_ASSERT((q.getText() != NULL));                 // well created query
    TEST_ASSERT((*(q.getText()) == 'h'));               // first letter
    TEST_ASSERT((q.get_word_num() == 4));               // 4 words coz the rest are too small or too large
    TEST_ASSERT(strcmp(q.getWord(0), "hello") == 0);
    TEST_ASSERT(strcmp(q.getWord(2), "things") == 0);
    TEST_ASSERT((q.getWord(6) == NULL));                // max query length = 5

    delete[] q_words;

    // empty input
    TEST_EXCEPTION(Query q2(NULL, 1), std::exception); // given null as input -> throw exception
}

void test_document(void)
{
    // too big/small word in the input
    char *d_words = new char[MAX_DOC_LENGTH]();
    strcpy(d_words, "hello people    how hellopeoplehellopeoplehellopeople things people how");
    Document d(d_words, 0);
	//d.printDocument();

    TEST_ASSERT((d.getText() != NULL));
    TEST_ASSERT((*(d.getText()) == 'h'));
    TEST_ASSERT((d.get_word_num() == 4));
    TEST_ASSERT(strcmp(d.getWord(0),"hello") == 0);
    TEST_ASSERT(strcmp(d.getWord(2),"things") == 0);
    TEST_ASSERT((d.getWord(6) == NULL));

    delete[] d_words;

    // empty input
    TEST_EXCEPTION(Document d2(NULL, 1), std::exception);
}

void test_entry(void)
{
    char *d_words = new char[MAX_DOC_LENGTH]();
    strcpy(d_words, "hello world lorem ipsum");
    Document d(d_words, 2);

    const word w = d.getWord(0);
    const word w2 = d.getWord(1);
    const word w3 = NULL;

    entry* e = NULL;
    entry* e2 = NULL;
    entry* e3 = NULL;
    entry_list* el = NULL;

    // entry list well created
    ErrorCode errorcode = create_entry_list(&el);
    TEST_ASSERT((errorcode == EC_SUCCESS));
    TEST_ASSERT((el != NULL));
    TEST_ASSERT((get_number_entries(el) == 0));
    
    // entries well created
    errorcode = create_entry(&w, &e);
    TEST_ASSERT((errorcode == EC_SUCCESS));
    errorcode = create_entry(&w2, &e2);
    TEST_ASSERT((errorcode == EC_SUCCESS));
    TEST_ASSERT((e != NULL));               
    TEST_ASSERT((e2 != NULL));  

    // create entry with nullptr
    TEST_ASSERT((create_entry(&w3, &e3) == EC_FAIL));  
    // create entry list with nullptr
    TEST_ASSERT((add_entry(el, e3) == EC_FAIL));             

    // instance variables well initialized  
    TEST_ASSERT((strcmp(e->getWord(), "hello") == 0));
    TEST_ASSERT((strcmp(e2->getWord(), "world") == 0));
    TEST_ASSERT((e->getNext() == NULL));
    TEST_ASSERT((e2->getNext() == NULL));
    TEST_ASSERT((e->getPayload() == NULL));
    TEST_ASSERT((e2->getPayload() == NULL));

    // entries well added to the entry list
    errorcode = add_entry(el, e);
    TEST_ASSERT((errorcode == EC_SUCCESS));
    errorcode = add_entry(el, e2);
    TEST_ASSERT((errorcode == EC_SUCCESS));
    TEST_ASSERT((get_number_entries(el) == 2));

    // e2 goes to the head of the list, so its next entry is e
    TEST_ASSERT((strcmp(get_first(el)->getWord(), "world") == 0));
    TEST_ASSERT((strcmp(get_next(el, e2)->getWord(), "hello") == 0));
    TEST_ASSERT((get_next(el, e) == NULL));

    // entries and entry list well destroyed
    //errorcode = destroy_entry(e);
    //TEST_ASSERT((errorcode == EC_SUCCESS));
    //errorcode = destroy_entry(e2);
    //TEST_ASSERT((errorcode == EC_SUCCESS));
    errorcode = destroy_entry_list(el);
    TEST_ASSERT((errorcode == EC_SUCCESS));
    delete[] d_words;
}

// Deduplication
void test_binary_search(void)
{
    word* words = new word[3];
    int words_until_now = 0;
    for(int i = 0; i < 3; i++)
    {
        words[i] = new char[MAX_WORD_LENGTH]();
    }
    strcpy(words[0], "b");
    words_until_now++;
    // with this function i avoid adding duplicates
    // but i also keep the bucket sorted
    TEST_ASSERT((binarySearch(words, 0, words_until_now - 1, (char*)"b") == -1)); 
    TEST_ASSERT((binarySearch(words, 0, words_until_now - 1, (char*)"a") == 0)); 
    TEST_ASSERT((binarySearch(words, 0, words_until_now - 1, (char*)"c") == 1)); 
    TEST_EXCEPTION(binarySearch(NULL, 0, words_until_now - 1, NULL), std::exception); // pass NULL as word

    for(int i = 0; i < 3; i++)
    {
        delete[] words[i];
    }
    delete[] words;
}

void test_hash_table(void)
{
    HashTable* HT = new HashTable;                                   // create hash table
    const word w = new char[MAX_WORD_LENGTH];
    strcpy(w, "hello");
    unsigned long hash = HT->addToBucket(hashFunction(w), w);        // add word to hash table
    TEST_ASSERT(( hash == hashFunction(w) ));                   
    TEST_ASSERT(( HT->getWordsPerBucket(hash) == 1 ));               // one word in this bucket
    hash = HT->addToBucket(hashFunction(w), w);                      // add the exact same word to hash table
    TEST_ASSERT(( HT->getWordsPerBucket(hash) == 1 ));               // still one word , coz duplicates are not added
    TEST_ASSERT((HT->getBucket(hash)[0] != HT->getBucket(hash)[1] && strcmp(HT->getBucket(hash)[0], w) == 0));  // check that the second word wasnt added
    //HT.printBucket(hash);
    TEST_EXCEPTION(HT->addToBucket(hashFunction(NULL), NULL), std::exception); // pass NULL as word
    delete[] w;
    
    const word w2 = new char[MAX_WORD_LENGTH];
    strcpy(w2, "world");
    hash = HT->addToBucket(hashFunction(w2), w2);     
    //HT.printTable();
    delete[] w2;
    delete HT;
}

void test_deduplication(void)
{
    char *d_words = new char[MAX_DOC_LENGTH]();
    strcpy(d_words, "hello world lorem ipsum hello world lorem ipsum");
    Document d(d_words, 3);
    HashTable* HT = new HashTable();
    Deduplication(&d, HT);
    TEST_EXCEPTION(Deduplication(NULL, HT), std::exception);
    delete HT;
    delete[] d_words;
}
void test_hash_function(void)
{
    TEST_ASSERT(hashFunction((char*)"hello") == hashFunction((char*)"hello"));
    TEST_ASSERT(hashFunction((char*)"hello") <= MAX_BUCKETS);
    TEST_EXCEPTION(hashFunction(NULL), std::exception);
    TEST_EXCEPTION(hashFunction((char*)" "), std::exception);
}

// Match Functions
void test_exact_match(void)
{   
    word word1 = (word)"hello";
    word word2 = (word)"hello";
    word word3 = (word)"hellno";
    const word ptrNull = NULL;
    TEST_ASSERT(exactMatch(word1,word2) == true);
    TEST_ASSERT(exactMatch(word1,word3) == false);
    TEST_EXCEPTION(exactMatch(ptrNull, word1), std::exception);
    TEST_EXCEPTION(exactMatch(ptrNull, ptrNull), std::exception);
}

void test_hamming(void)
{   
    word word1 = (word)"hello";
    word word2 = (word)"hello";
    word word3 = (word)"hellno";
    word word4 = (word)"hellO";
    const word ptrNull = NULL;
    TEST_ASSERT(hammingDistance(word1,word2) == 0);
    TEST_ASSERT(hammingDistance(word1,word3) == 2);
    TEST_ASSERT(hammingDistance(word1,word4) == 1);
    TEST_EXCEPTION(hammingDistance(ptrNull, word1), std::exception);
    TEST_EXCEPTION(hammingDistance(ptrNull, ptrNull), std::exception);
}

void test_edit(void)
{   
    word word1 = (word)"hello";
    word word2 = (word)"hello";
    word word3 = (word)"hellno";
    word word4 = (word)"hellO";
    word word5 = (word)"_hello";
    const word ptrNull = NULL;
    TEST_ASSERT( editDistance(word1,word2) == 0);
    TEST_ASSERT( editDistance(word1,word3) == 1);
    TEST_ASSERT( editDistance(word1,word4) == 1);
    TEST_ASSERT( editDistance(word1,word5) == 1);
    TEST_EXCEPTION( editDistance(ptrNull, word1), std::exception);
    TEST_EXCEPTION( editDistance(ptrNull, ptrNull), std::exception);
}

// Queue
void test_enqueue(void) {
    // Setup
    indexing* nullPTR = NULL;
	Queue testQueue;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexing* testIndex1 = new indexing(testEntry1);
    indexing* testIndex2 = new indexing(testEntry2);
    indexing* testIndex3 = new indexing(testEntry3);

    TEST_EXCEPTION(testQueue.enqueue(NULL), std::exception);
    TEST_EXCEPTION(testQueue.enqueue(&nullPTR), std::exception);
    TEST_ASSERT(testQueue.getSize() == 0);

    testQueue.enqueue(&testIndex1);
    TEST_ASSERT(testQueue.getSize() == 1);

    testQueue.enqueue(&testIndex1);
    TEST_ASSERT(testQueue.getSize() == 2);

    testQueue.enqueue(&testIndex3);
    TEST_ASSERT(testQueue.getSize() == 3);

    while(testQueue.getSize() > 0) {
        testQueue.dequeue();
    }
    delete testIndex1;
    delete testIndex2;
    delete testIndex3;

}

void test_dequeue(void) {
    // Setup
    indexing* nodeToReturn;
	Queue testQueue;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexing* testIndex1 = new indexing(testEntry1);
    indexing* testIndex2 = new indexing(testEntry2);
    indexing* testIndex3 = new indexing(testEntry3);

    // Test empty queue
    nodeToReturn = testQueue.dequeue();
    TEST_ASSERT(nodeToReturn == NULL);      // We should get NULL, the queue is empty

    // Test queue with one item
    testQueue.enqueue(&testIndex1);
    nodeToReturn = testQueue.dequeue();
    TEST_ASSERT(nodeToReturn == testIndex1); // We should get node we put in
    nodeToReturn = testQueue.dequeue();
    TEST_ASSERT(nodeToReturn == NULL);      // The queue is empty, return NULL again

    // Test queue with multiple items
    testQueue.enqueue(&testIndex1);         // Add multiple indexNodes
    testQueue.enqueue(&testIndex2);
    testQueue.enqueue(&testIndex3);
    nodeToReturn = testQueue.dequeue();
    TEST_ASSERT(nodeToReturn == testIndex3);    // Get them one by one
    nodeToReturn = testQueue.dequeue();
    TEST_ASSERT(nodeToReturn == testIndex2);
    nodeToReturn = testQueue.dequeue();
    TEST_ASSERT(nodeToReturn == testIndex1);
    nodeToReturn = testQueue.dequeue();
    TEST_ASSERT(nodeToReturn == NULL);          // The queue is empty, return NULL again

    delete testIndex1;
    delete testIndex2;
    delete testIndex3;
}

void test_childQueueNode_create(void) {
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    indexing* testIndex1 = new indexing(testEntry1);
    indexing* testIndex2 = new indexing(testEntry2);
    childQueueNode* testNode1 = NULL;
    childQueueNode* testNode2 = NULL;

    TEST_EXCEPTION(testNode1 = new childQueueNode(NULL), std::exception);
    TEST_ASSERT(testNode1 == NULL);

    testNode1 = new childQueueNode(testIndex1);
    TEST_ASSERT(testNode1->getNext() == NULL);

    testNode2 = new childQueueNode(testIndex2, testNode1);
    TEST_ASSERT(testNode2->getNext() == testNode1);

    delete testNode1;
    delete testNode2;
    delete testIndex1;
    delete testIndex2;
}

void test_childQueueNode_pop(void) {
    indexing* nodeToReturn;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    indexing* testIndex1 = new indexing(testEntry1);
    indexing* testIndex2 = new indexing(testEntry2);
    childQueueNode* testNode1 = NULL;
    childQueueNode* testNode2 = NULL;
    childQueueNode* childNodeToReturn;

    testNode1 = new childQueueNode(testIndex1);
    testNode1->pop(&nodeToReturn, &childNodeToReturn);
    TEST_ASSERT (nodeToReturn == testIndex1);
    TEST_ASSERT (childNodeToReturn == NULL);
    testNode1 = new childQueueNode(testIndex1);
    testNode2 = new childQueueNode(testIndex2, testNode1);
    testNode2->pop(&nodeToReturn, &childNodeToReturn);
    TEST_ASSERT (nodeToReturn == testIndex2);
    TEST_ASSERT (childNodeToReturn == testNode1);
    testNode1->pop(&nodeToReturn, &childNodeToReturn);
    TEST_ASSERT (nodeToReturn == testIndex1);
    TEST_ASSERT (childNodeToReturn == NULL);

    delete testIndex1;
    delete testIndex2;
}


TEST_LIST = {
    {"Query", test_query} ,
    {"Document", test_document} ,
    {"Entry", test_entry} ,
    {"Binary Search", test_binary_search} ,
    {"Hash Function", test_hash_function} ,
    {"Deduplication", test_deduplication} ,
    {"Hash Table", test_hash_table} ,
    {"Exact Match", test_exact_match},
    {"Hamming Distance", test_hamming},
    {"Edit Distance", test_edit},
    {"Enqueue", test_enqueue},
    {"Dequeue", test_dequeue},
    {"Create ChildQueueNode", test_childQueueNode_create},
    {"Pop ChildQueueNode", test_childQueueNode_pop},
    { NULL, NULL }
};