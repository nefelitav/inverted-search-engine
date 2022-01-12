#include "tests.hpp"

//////////////////////////// Structs ////////////////////////////

void test_query(void)
{
    char *q_words = new char[MAX_QUERY_LENGTH]();
    memcpy(q_words, "blue purple green black yellow", MAX_QUERY_LENGTH);
    Query q(0, q_words, MT_EXACT_MATCH, 0);
    TEST_CHECK(strcmp(q.getWord(0), "blue") == 0);
    TEST_CHECK(strcmp(q.getWord(2), "green") == 0);
    TEST_CHECK((q.getWord(6) == NULL)); // max query length = 5
    TEST_CHECK((q.getId() == 0));
    TEST_CHECK((q.getMatchingDistance() == 0));
    TEST_CHECK((q.getMatchingType() == MT_EXACT_MATCH));
    q.setTrue((char *)"blue");
    TEST_CHECK((q.getMatchedEntries()[0] == true));
    q.setFalse();
    TEST_CHECK((q.getMatchedEntries()[0] == false));
    q.setTrue((char *)"blue");
    q.setTrue((char *)"purple");
    q.setTrue((char *)"green");
    q.setTrue((char *)"black");
    q.setTrue((char *)"yellow");
    TEST_CHECK((q.matched() == true)); // everything to true -> its a match
    delete[] q_words;
}

void test_document(void)
{
    char *d_words = new char[MAX_DOC_LENGTH]();
    strcpy(d_words, "blue purple green black yellow blue purple green black yellow");
    Document *d = new Document(0, d_words);
    TEST_CHECK(strcmp(d->getWord(0), "blue") == 0);
    TEST_CHECK(strcmp(d->getWord(2), "green") == 0);
    TEST_CHECK(strcmp(d->getWord(7), "green") == 0);
    TEST_CHECK((d->getWord(10) == NULL));
    delete d;
    delete[] d_words;
}

void test_entry(void)
{
    char *q_words = new char[MAX_QUERY_LENGTH]();
    strcpy(q_words, "hello world lorem ipsum");
    Query q(2, q_words, MT_EXACT_MATCH, 0);

    const word w = q.getWord(0);
    const word w2 = q.getWord(1);
    const word w3 = NULL;

    entry *e = NULL;
    entry *e2 = NULL;
    entry *e3 = NULL;

    // entries well created
    ErrorCode errorcode = create_entry(&w, &e);
    TEST_CHECK((errorcode == EC_SUCCESS));
    errorcode = create_entry(&w2, &e2);
    TEST_CHECK((errorcode == EC_SUCCESS));
    TEST_CHECK((e != NULL));
    TEST_CHECK((e2 != NULL));

    // create entry with nullptr
    TEST_CHECK((create_entry(&w3, &e3) == EC_FAIL));

    // instance variables well initialized
    TEST_CHECK((strcmp(e->getWord(), "hello") == 0));
    TEST_CHECK((strcmp(e2->getWord(), "world") == 0));
    TEST_CHECK((e->getNext() == NULL));
    TEST_CHECK((e2->getNext() == NULL));
    TEST_CHECK((e->getPayload() == NULL));
    TEST_CHECK((e2->getPayload() == NULL));

    // entries well destroyed
    errorcode = destroy_entry(e);
    TEST_CHECK((errorcode == EC_SUCCESS));
    errorcode = destroy_entry(e2);
    TEST_CHECK((errorcode == EC_SUCCESS));
    delete[] q_words;
}

void test_entry_set_get_payload(void)
{
    word testWord = (word) "TESTWORD1";
    entry *testEntry;
    create_entry(&testWord, &testEntry);

    TEST_CHECK(testEntry->getPayload() == NULL);
    testEntry->addToPayload(1, 2);
    TEST_CHECK(testEntry->getPayload()->getId() == 1);
    TEST_CHECK(testEntry->getPayload()->getThreshold() == 2);

    destroy_entry(testEntry);
}

void test_entry_emptyPayload(void)
{
    char *testWord1 = (char *)"TESTWORD1";
    entry *testEntry1;
    create_entry(&testWord1, &testEntry1);

    TEST_CHECK(testEntry1->emptyPayload() == true);
    testEntry1->addToPayload(0, 0);
    TEST_CHECK(testEntry1->emptyPayload() == false);

    destroy_entry(testEntry1);
}

void test_payloadNode_constructor_getters(void)
{
    payloadNode *testNode = new payloadNode(1, 1);
    TEST_CHECK(testNode->getId() == 1);
    TEST_CHECK(testNode->getThreshold() == 1);
    TEST_CHECK(testNode->getNext() == NULL);
    delete testNode;
}

void test_payloadNode_setNext(void)
{
    payloadNode *testNode1 = new payloadNode(1, 1);
    payloadNode *testNode2 = new payloadNode(2, 2);
    // Check Initial State is NULL
    TEST_CHECK(testNode1->getNext() == NULL);
    TEST_CHECK(testNode2->getNext() == NULL);
    // Test Setting to another Node
    testNode1->setNext(testNode2);
    TEST_CHECK(testNode1->getNext() == testNode2);
    // Test Setting to NULL
    testNode1->setNext(NULL);
    TEST_CHECK(testNode1->getNext() == NULL);
    delete testNode1;
    delete testNode2;
}

void test_payload_addToPayload(void)
{
    // Create a test entry
    char *testWord1 = (char *)"TESTWORD1";
    entry *testEntry1;
    create_entry(&testWord1, &testEntry1);

    // Add payload with QueryID 2, check that it is accessible
    testEntry1->addToPayload(2, 2);
    TEST_CHECK(testEntry1->getPayload()->getId() == 2);
    // Add payload with QueryID 3, check that both 2 and 3 are accessible
    testEntry1->addToPayload(3, 3);
    TEST_CHECK(testEntry1->getPayload()->getId() == 3);
    TEST_CHECK(testEntry1->getPayload()->getNext()->getId() == 2);
    // Add payload with QueryID 1, check that all 1, 2 and 3 are accessible
    testEntry1->addToPayload(1, 1);
    TEST_CHECK(testEntry1->getPayload()->getId() == 3);
    TEST_CHECK(testEntry1->getPayload()->getNext()->getId() == 2);
    TEST_CHECK(testEntry1->getPayload()->getNext()->getNext()->getId() == 1);

    // Cleanup
    destroy_entry(testEntry1);
}

void test_payload_deletePayloadNode(void)
{
    // Create entry with 3 payload nodes
    char *testWord1 = (char *)"TESTWORD1";
    entry *testEntry1;
    create_entry(&testWord1, &testEntry1);
    testEntry1->addToPayload(1, 1);
    testEntry1->addToPayload(2, 2);
    testEntry1->addToPayload(3, 3);

    // Delete the node with QueryID 1 and check that it is removed from the list
    testEntry1->deletePayloadNode(1);
    TEST_CHECK(testEntry1->getPayload()->getId() == 3);
    TEST_CHECK(testEntry1->getPayload()->getNext()->getId() == 2);

    // Delete the node with QueryID 2 and check that it is removed from the list
    testEntry1->deletePayloadNode(2);
    TEST_CHECK(testEntry1->getPayload()->getId() == 3);

    // Delete the node with QueryID 3 and check that the list is now empty
    testEntry1->deletePayloadNode(3);
    TEST_CHECK(testEntry1->emptyPayload());

    // Cleanup
    destroy_entry(testEntry1);
}

void test_entrylist(void)
{
    char *q_words = new char[MAX_QUERY_LENGTH]();
    strcpy(q_words, "hello world lorem ipsum");
    Query q(2, q_words, MT_EXACT_MATCH, 0);

    const word w = q.getWord(0);
    const word w2 = q.getWord(1);
    const word w3 = NULL;

    entry *e = NULL;
    entry *e2 = NULL;
    entry *e3 = NULL;
    entry_list *el = NULL;

    // entry list well created
    ErrorCode errorcode = create_entry_list(&el);
    TEST_CHECK((errorcode == EC_SUCCESS));
    TEST_CHECK((el != NULL));
    TEST_CHECK((get_number_entries(el) == 0));

    // entries well created
    errorcode = create_entry(&w, &e);
    TEST_CHECK((errorcode == EC_SUCCESS));
    errorcode = create_entry(&w2, &e2);
    TEST_CHECK((errorcode == EC_SUCCESS));
    TEST_CHECK((e != NULL));
    TEST_CHECK((e2 != NULL));

    // create entry with nullptr
    TEST_CHECK((create_entry(&w3, &e3) == EC_FAIL));
    // create entry list with nullptr
    TEST_CHECK((add_entry(el, e3) == EC_FAIL));

    // instance variables well initialized
    TEST_CHECK((strcmp(e->getWord(), "hello") == 0));
    TEST_CHECK((strcmp(e2->getWord(), "world") == 0));
    TEST_CHECK((e->getNext() == NULL));
    TEST_CHECK((e2->getNext() == NULL));

    // entries well added to the entry list
    errorcode = add_entry(el, e);
    TEST_CHECK((errorcode == EC_SUCCESS));
    errorcode = add_entry(el, e2);
    TEST_CHECK((errorcode == EC_SUCCESS));
    TEST_CHECK((get_number_entries(el) == 2));

    // e2 goes to the head of the list, so its next entry is e
    TEST_CHECK((strcmp(get_first(el)->getWord(), "world") == 0));
    TEST_CHECK((strcmp(get_next(el, e2)->getWord(), "hello") == 0));
    TEST_CHECK((get_next(el, e) == NULL));

    // entries and entry list well destroyed
    errorcode = destroy_entry_list(el);
    TEST_CHECK((errorcode == EC_SUCCESS));
    delete[] q_words;
}

//////////////////////////// Hash Tables ////////////////////////////

void test_binary_search(void)
{
    word *words = new word[3];
    int words_until_now = 0;
    for (int i = 0; i < 3; i++)
    {
        words[i] = new char[MAX_WORD_LENGTH]();
    }
    strcpy(words[0], "b");
    words_until_now++;

    TEST_CHECK((binarySearch(words, 0, words_until_now - 1, (char *)"b") == -1));
    TEST_CHECK((binarySearch(words, 0, words_until_now - 1, (char *)"a") == 0));
    TEST_CHECK((binarySearch(words, 0, words_until_now - 1, (char *)"c") == 1));
    TEST_EXCEPTION(binarySearch(NULL, 0, words_until_now - 1, NULL), std::exception); // pass NULL as word

    for (int i = 0; i < 3; i++)
    {
        delete[] words[i];
    }
    delete[] words;
}
void test_query_binary_search(void)
{
    int queries_until_now = 0;
    char *q_words = new char[MAX_QUERY_LENGTH]();
    memcpy(q_words, "hello people things", MAX_QUERY_LENGTH);
    Query **q = new Query *[1]();
    q[0] = new Query(1, q_words, MT_EXACT_MATCH, 0);

    queries_until_now++;

    TEST_CHECK((queryBinarySearch(q, 0, queries_until_now - 1, 1) == -1));
    TEST_CHECK((queryBinarySearch(q, 0, queries_until_now - 1, 0) == 0));
    TEST_CHECK((queryBinarySearch(q, 0, queries_until_now - 1, 2) == 1));
    TEST_EXCEPTION(queryBinarySearch(NULL, 0, queries_until_now - 1, 0), std::exception); // pass NULL as query array
    delete q[0];
    delete[] q;
    delete[] q_words;
}

void test_find_query(void)
{
    int queries_until_now = 0;
    char *q_words = new char[MAX_QUERY_LENGTH]();
    memcpy(q_words, "hello people things", MAX_QUERY_LENGTH);
    Query **q = new Query *[5]();

    q[0] = new Query(0, q_words, MT_EXACT_MATCH, 0);
    q[1] = new Query(1, q_words, MT_EXACT_MATCH, 0);
    q[2] = new Query(2, q_words, MT_EXACT_MATCH, 0);
    q[3] = new Query(3, q_words, MT_EXACT_MATCH, 0);
    q[4] = new Query(4, q_words, MT_EXACT_MATCH, 0);

    queries_until_now += 5;
    TEST_CHECK((findQuery(q, 0, queries_until_now - 1, 0) == 0));
    TEST_CHECK((findQuery(q, 0, queries_until_now - 1, 2) == 2));
    TEST_CHECK((findQuery(q, 0, queries_until_now - 1, 4) == 4));

    TEST_EXCEPTION(findQuery(NULL, 0, queries_until_now - 1, 0), std::exception); // pass NULL as query array

    for (int i = 0; i < 5; i++)
        delete q[i];
    delete[] q;
    delete[] q_words;
}

void test_entry_binary_search(void)
{
    entry **entries = new entry *[1]();
    int entries_until_now = 0;
    entries[0] = new entry((char *)"b");

    entries_until_now++;

    TEST_CHECK(entryBinarySearch(entries, 0, entries_until_now - 1, (char *)"a") == 0);
    TEST_CHECK(entryBinarySearch(entries, 0, entries_until_now - 1, (char *)"b") == 0); // add to payload
    TEST_CHECK(entryBinarySearch(entries, 0, entries_until_now - 1, (char *)"c") == 1);

    TEST_EXCEPTION(entryBinarySearch(NULL, 0, entries_until_now - 1, (char *)"b"), std::exception); // pass NULL as query array

    delete entries[0];
    delete[] entries;
}

void test_find_entry(void)
{
    entry **entries = new entry *[5]();
    int entries_until_now = 0;
    entries[0] = new entry((char *)"z");
    entries[1] = new entry((char *)"d");
    entries[2] = new entry((char *)"c");
    entries[3] = new entry((char *)"b");
    entries[4] = new entry((char *)"a");

    entries_until_now += 5;
    TEST_CHECK(findEntry(entries, 0, entries_until_now - 1, (char *)"a") == 4);
    TEST_CHECK(findEntry(entries, 0, entries_until_now - 1, (char *)"d") == 1);
    TEST_CHECK(findEntry(entries, 0, entries_until_now - 1, (char *)"c") == 2);
    TEST_CHECK(findEntry(entries, 0, entries_until_now - 1, (char *)"k") == -1); // not found

    TEST_EXCEPTION(findEntry(entries, 0, entries_until_now - 1, NULL), std::exception);
    TEST_CHECK(findEntry(NULL, 0, entries_until_now - 1, (char *)"c") == -1); // bucket is empty

    for (int i = 0; i < 5; i++)
        delete entries[i];
    delete[] entries;
}

void test_result_binary_search()
{
    QueryID ids[1];
    ids[0] = 23;
    TEST_CHECK(resultBinarySearch(ids, 0, 0, 0) == 0);
    TEST_CHECK(resultBinarySearch(ids, 0, 0, 23) == -1);
    TEST_CHECK(resultBinarySearch(ids, 0, 0, 50) == 1);

    TEST_EXCEPTION(resultBinarySearch(NULL, 0, 0, 2), std::exception); // pass NULL as query array
}

void test_doc_table(void)
{
    DocTable *HT = new DocTable(1); // create hash table
    const word w = new char[MAX_WORD_LENGTH];
    strcpy(w, "hello");
    unsigned long hash = HT->addToBucket(hashFunction(w), w); // add word to hash table
    TEST_CHECK((hash == hashFunction(w)));
    TEST_CHECK((HT->getWordsPerBucket(hash) == 1));                                                           // one word in this bucket
    hash = HT->addToBucket(hashFunction(w), w);                                                               // add the exact same word to hash table
    TEST_CHECK((HT->getWordsPerBucket(hash) == 1));                                                           // still one word , coz duplicates are not added
    TEST_CHECK((HT->getBucket(hash)[0] != HT->getBucket(hash)[1] && strcmp(HT->getBucket(hash)[0], w) == 0)); // check that the second word wasnt added
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
void test_query_table(void)
{
    QueryTable *QT = new QueryTable(); // create hash table
    char *q_words = new char[MAX_QUERY_LENGTH]();
    memcpy(q_words, "blue purple green black yellow", MAX_QUERY_LENGTH);
    Query *q = new Query(0, q_words, MT_EXACT_MATCH, 0);
    Query *q2 = new Query(0, q_words, MT_EXACT_MATCH, 0);

    unsigned long hash = QT->addToBucket(hashFunctionById(q->getId()), q); // add query to hash table
    TEST_CHECK((hash == hashFunctionById(q->getId())));
    TEST_CHECK((QT->getQueriesPerBucket(hash) == 1));                                              // one query in this bucket
    hash = QT->addToBucket(hashFunctionById(q2->getId()), q2);                                     // add the exact same queryid to hash table
    TEST_CHECK((QT->getQueriesPerBucket(hash) == 1));                                              // still one query , coz duplicates are not added
    TEST_CHECK((QT->getBucket(hash)[0] != QT->getBucket(hash)[1] && QT->getBucket(hash)[0] == q)); // check that the second query wasnt added
    TEST_EXCEPTION(QT->addToBucket(hashFunctionById(0), NULL), std::exception);                    // pass NULL as query

    delete[] q_words;
    delete QT;
}

void test_entry_table(void)
{
    EntryTable *ET = new EntryTable(); // create hash table
    char *q_words = new char[MAX_QUERY_LENGTH]();
    strcpy(q_words, "hello world lorem ipsum");
    Query q(2, q_words, MT_EXACT_MATCH, 0);

    const word w = q.getWord(0);
    const word w2 = q.getWord(1);

    entry *e = NULL;
    entry *e2 = NULL;

    // entries well created
    ErrorCode errorcode = create_entry(&w, &e);
    TEST_CHECK((errorcode == EC_SUCCESS));
    errorcode = create_entry(&w2, &e2);

    ET->addToBucket(hashFunction(w), e);
    TEST_CHECK(ET->getEntriesPerBucket(hashFunction(w)) == 1);
    TEST_CHECK(ET->getBucket(hashFunction(w))[0] == e);

    ET->addToBucket(hashFunction(w2), e2);
    TEST_CHECK(ET->getEntriesPerBucket(hashFunction(w2)) == 1);
    TEST_CHECK(ET->getBucket(hashFunction(w2))[0] == e2);

    destroy_entry(e);
    destroy_entry(e2);

    delete[] q_words;
    delete ET;
}

void test_deduplication(void)
{
    char *d_words = new char[MAX_DOC_LENGTH]();
    strcpy(d_words, "hello world lorem ipsum hello world lorem ipsum");
    Document *d = new Document(3, d_words);
    DocTable *HT = new DocTable(1);
    DocumentDeduplication(d, HT);
    TEST_EXCEPTION(DocumentDeduplication(NULL, HT), std::exception);
    delete d;
    delete HT;
    delete[] d_words;
}

void test_hash_function(void)
{
    TEST_CHECK(hashFunction((char *)"hello") == hashFunction((char *)"hello"));
    TEST_CHECK(hashFunction((char *)"hello") <= MAX_BUCKETS);
    TEST_EXCEPTION(hashFunction(NULL), std::exception);
    TEST_EXCEPTION(hashFunction((char *)" "), std::exception);
}

//////////////////////////// Distance Functions ////////////////////////////

void test_exact_match(void)
{
    word word1 = (word) "hello";
    word word2 = (word) "hello";
    word word3 = (word) "hellno";
    const word ptrNull = NULL;
    // The words are the same
    TEST_CHECK(exactMatch(word1, word2) == true);
    // The words are different
    TEST_CHECK(exactMatch(word1, word3) == false);
    // Test the exceptions for NULL pointers
    TEST_EXCEPTION(exactMatch(ptrNull, word1), std::exception);
    TEST_EXCEPTION(exactMatch(ptrNull, ptrNull), std::exception);
}

void test_hamming(void)
{
    word word1 = (word) "hello";
    word word2 = (word) "hello";
    word word3 = (word) "hellno";
    word word4 = (word) "hellO";
    // Test distance for good input
    TEST_CHECK(hammingDistance(word1, word2) == 0);
    TEST_CHECK(hammingDistance(word1, word4) == 1);
    // Test exception for different size words (NULL Pointers)
    TEST_EXCEPTION(hammingDistance(word1, word3), std::exception);
}

void test_edit(void)
{
    word word1 = (word) "hello";
    word word2 = (word) "hello";
    word word3 = (word) "hellno";
    word word4 = (word) "hellO";
    word word5 = (word) "_hello";
    TEST_CHECK(editDistance(word1, word2) == 0);
    TEST_CHECK(editDistance(word1, word3) == 1);
    TEST_CHECK(editDistance(word1, word4) == 1);
    TEST_CHECK(editDistance(word1, word5) == 1);
}

//////////////////////////// Stack ////////////////////////////

void test_stack_add(void)
{
    // Setup
    indexNode *nullPTR = NULL;
    Stack testStack;
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD2";
    word testWord3 = (word) "TESTWORD3";
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);
    indexNode *testIndex1 = new indexNode(testEntry1);
    indexNode *testIndex2 = new indexNode(testEntry2);
    indexNode *testIndex3 = new indexNode(testEntry3);

    // Try to add NULL
    TEST_EXCEPTION(testStack.add(NULL), std::exception);
    TEST_EXCEPTION(testStack.add(&nullPTR), std::exception);
    TEST_CHECK(testStack.getSize() == 0);

    // Add one, check size
    testStack.add(&testIndex1);
    TEST_CHECK(testStack.getSize() == 1);

    // Add second, check size
    testStack.add(&testIndex1);
    TEST_CHECK(testStack.getSize() == 2);

    // Add third, check size
    testStack.add(&testIndex3);
    TEST_CHECK(testStack.getSize() == 3);

    // Cleanup
    while (testStack.getSize() > 0)
    {
        testStack.pop();
    }
    delete testIndex1;
    delete testIndex2;
    delete testIndex3;
    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
    destroy_entry(testEntry3);
}

void test__stack_pop(void)
{
    // Setup
    indexNode *nodeToReturn;
    Stack testStack;
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD2";
    word testWord3 = (word) "TESTWORD3";
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);
    indexNode *testIndex1 = new indexNode(testEntry1);
    indexNode *testIndex2 = new indexNode(testEntry2);
    indexNode *testIndex3 = new indexNode(testEntry3);

    // Test empty stack
    nodeToReturn = testStack.pop();
    TEST_CHECK(nodeToReturn == NULL); // We should get NULL, the stack is empty

    // Test stack with one item
    testStack.add(&testIndex1);
    nodeToReturn = testStack.pop();
    TEST_CHECK(nodeToReturn == testIndex1); // We should get node we put in
    nodeToReturn = testStack.pop();
    TEST_CHECK(nodeToReturn == NULL); // The stack is empty, return NULL again

    // Test stack with multiple items
    testStack.add(&testIndex1); // Add multiple indexings
    testStack.add(&testIndex2);
    testStack.add(&testIndex3);
    nodeToReturn = testStack.pop();
    TEST_CHECK(nodeToReturn == testIndex3); // Get them one by one
    nodeToReturn = testStack.pop();
    TEST_CHECK(nodeToReturn == testIndex2);
    nodeToReturn = testStack.pop();
    TEST_CHECK(nodeToReturn == testIndex1);
    nodeToReturn = testStack.pop();
    TEST_CHECK(nodeToReturn == NULL); // The stack is empty, return NULL again

    // Cleanup
    delete testIndex1;
    delete testIndex2;
    delete testIndex3;
    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
    destroy_entry(testEntry3);
}

void test_stackNode_create(void)
{
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD2";
    entry *testEntry1;
    entry *testEntry2;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    indexNode *testIndex1 = new indexNode(testEntry1);
    indexNode *testIndex2 = new indexNode(testEntry2);
    stackNode *testNode1 = NULL;
    stackNode *testNode2 = NULL;

    // Test exception for NULL input
    TEST_EXCEPTION(testNode1 = new stackNode(NULL), std::exception);
    // Check that the pointer did not change
    TEST_CHECK(testNode1 == NULL);

    // Check that this is the only node
    testNode1 = new stackNode(testIndex1);
    TEST_CHECK(testNode1->getNext() == NULL);

    // Check that the first can see the second
    testNode2 = new stackNode(testIndex2, testNode1);
    TEST_CHECK(testNode2->getNext() == testNode1);

    // Cleanup
    delete testNode1;
    delete testNode2;
    delete testIndex1;
    delete testIndex2;
    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
}

void test_stackNode_pop(void)
{
    indexNode *nodeToReturn;
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD2";
    entry *testEntry1;
    entry *testEntry2;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    indexNode *testIndex1 = new indexNode(testEntry1);
    indexNode *testIndex2 = new indexNode(testEntry2);
    stackNode *testNode1 = NULL;
    stackNode *testNode2 = NULL;
    stackNode *childNodeToReturn;

    // Test adding and removing single node
    testNode1 = new stackNode(testIndex1);
    testNode1->pop(&nodeToReturn, &childNodeToReturn);
    TEST_CHECK(nodeToReturn == testIndex1);
    TEST_CHECK(childNodeToReturn == NULL);

    // Test adding and removind 2 nodes
    testNode1 = new stackNode(testIndex1);
    testNode2 = new stackNode(testIndex2, testNode1);
    testNode2->pop(&nodeToReturn, &childNodeToReturn);
    TEST_CHECK(nodeToReturn == testIndex2);
    TEST_CHECK(childNodeToReturn == testNode1);
    testNode1->pop(&nodeToReturn, &childNodeToReturn);
    TEST_CHECK(nodeToReturn == testIndex1);
    TEST_CHECK(childNodeToReturn == NULL);

    delete testIndex1;
    delete testIndex2;
    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
}

//////////////////////////// Index ////////////////////////////

void test_indexNode_constructor_addEntry(void)
{
    // Setup
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD2";
    word testWord3 = (word) "TESTWORD3";
    word testWord4 = (word) "TESTWOR33";
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    entry *testEntry4;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);
    create_entry(&testWord4, &testEntry4);

    indexNode *head;

    // Test creating with exact match (considered invalid for tree creation)

    // Test insertion and exceptions when adding duplicates
    head = new indexNode(testEntry1);
    TEST_CHECK(head->addEntry(testEntry2, 1, 1) == EC_SUCCESS);
    TEST_CHECK(head->addEntry(testEntry3, 1, 1) == EC_SUCCESS);
    TEST_CHECK(head->addEntry(testEntry3, 2, 1) == EC_SUCCESS);
    TEST_CHECK(head->addEntry(testEntry4, 1, 1) == EC_SUCCESS);

    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
    destroy_entry(testEntry3);
    destroy_entry(testEntry4);
    delete head;
}

void test_indexNode_getEntry(void)
{
    word testWord1 = (word) "TESTWORD1";
    entry *testEntry1;
    create_entry(&testWord1, &testEntry1);
    indexNode *head = new indexNode(testEntry1);
    TEST_CHECK(head->getEntry() == testEntry1);

    destroy_entry(testEntry1);
    delete head;
}

void test_indexNode_getChildren(void)
{
    // Setup
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD2";
    word testWord3 = (word) "TESTWORd3";
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);

    indexNode *head;
    indexList *listOfChildren;

    head = new indexNode(testEntry1);
    head->addEntry(testEntry2, 1, 1);
    head->addEntry(testEntry3, 1, 1);

    listOfChildren = head->getChildren();
    // Test the correct order of the nodes in the list
    TEST_CHECK(listOfChildren->getNode()->getEntry() == testEntry2);
    TEST_CHECK(listOfChildren->getDistanceFromParent() == 1);
    TEST_CHECK(listOfChildren->getNext()->getNode()->getEntry() == testEntry3);
    TEST_CHECK(listOfChildren->getNext()->getDistanceFromParent() == 2);

    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
    destroy_entry(testEntry3);
    delete head;
}

void test_indexNode_getMatchingType(void)
{

    // Setup
    word testWord1 = (word) "TESTWORD1";
    entry *testEntry1 = NULL;
    indexNode *head = NULL;

    // Test with default args
    create_entry(&testWord1, &testEntry1);
    head = new indexNode(testEntry1);
    TEST_CHECK(head->getMatchingType() == MT_EDIT_DIST);
    destroy_entry(testEntry1);
    delete head;

    // Test by explicit argument EDIT_DIST
    create_entry(&testWord1, &testEntry1);
    head = new indexNode(testEntry1, MT_EDIT_DIST);
    TEST_CHECK(head->getMatchingType() == MT_EDIT_DIST);
    destroy_entry(testEntry1);
    delete head;

    // Test by explicit argument EDIT_DIST
    create_entry(&testWord1, &testEntry1);
    head = new indexNode(testEntry1, 1, 1, MT_HAMMING_DIST);
    TEST_CHECK(head->getMatchingType() == MT_HAMMING_DIST);
    destroy_entry(testEntry1);
    delete head;
}

//////////////////////////// indexList ////////////////////////////

void test_indexList_constructor(void)
{
    // Setup
    word testWord1 = (word) "TESTWORD1";
    entry *testEntry1;
    create_entry(&testWord1, &testEntry1);
    entry *nullTestEntry = NULL;
    indexList *testList = NULL;

    // Test indexList Constructor Edge Cases
    TEST_EXCEPTION(testList = new indexList(nullTestEntry, 1, MT_EDIT_DIST, 1, 1), std::exception);
    TEST_EXCEPTION(testList = new indexList(testEntry1, 1, MT_EXACT_MATCH, 1, 1), std::exception);
    TEST_EXCEPTION(testList = new indexList(testEntry1, 0, MT_EDIT_DIST, 1, 1), std::exception);

    testList = new indexList(testEntry1, 1, MT_EDIT_DIST, 1, 1);
    TEST_CHECK(testList != NULL);

    destroy_entry(testEntry1);
    delete testList;
}

void test_indexList_addToList(void)
{
    // Setup
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD2";
    word testWord3 = (word) "TESTWORd3";
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);
    entry *nullTestEntry = NULL;
    indexList *testList = NULL;

    // Test adding good inputs
    testList = new indexList(testEntry1, 1, MT_EDIT_DIST, 1, 1);
    TEST_CHECK(testList->addToList(testEntry3, 2, 1, 1) == 0);
    TEST_CHECK(testList->addToList(testEntry2, 1, 1, 1) == 0);

    // Test NULL Pointers and invalid distance from parent
    TEST_EXCEPTION(testList->addToList(testEntry3, 0, 1, 1), std::exception);
    TEST_EXCEPTION(testList->addToList(nullTestEntry, 3, 1, 1), std::exception);

    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
    destroy_entry(testEntry3);
    delete testList;
}

void test_indexList_getDistanceFromParent(void)
{
    // Setup
    word testWord1 = (word) "TESTWORD1";
    entry *testEntry1;
    create_entry(&testWord1, &testEntry1);
    indexList *testList = NULL;

    testList = new indexList(testEntry1, 1, MT_EDIT_DIST, 1, 1);
    TEST_CHECK(testList->getDistanceFromParent() == 1);

    destroy_entry(testEntry1);
    delete testList;
}

void test_indexList_getNode(void)
{
    // Setup
    word testWord1 = (word) "TESTWORD1";
    entry *testEntry1;
    create_entry(&testWord1, &testEntry1);
    indexList *testList = NULL;

    testList = new indexList(testEntry1, 1, MT_EDIT_DIST, 1, 1);
    TEST_CHECK(testList->getNode()->getEntry() == testEntry1);

    destroy_entry(testEntry1);
    delete testList;
}

void test_indexList_getNext(void)
{
    // Setup
    word testWord1 = (word) "TESTWORD1";
    word testWord2 = (word) "TESTWORD22";
    entry *testEntry1;
    entry *testEntry2;
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    indexList *testList = NULL;

    testList = new indexList(testEntry1, 1, MT_EDIT_DIST, 1, 1);
    testList->addToList(testEntry2, 2, 1, 1);
    TEST_CHECK(testList->getNext()->getNode()->getEntry() == testEntry2);

    destroy_entry(testEntry1);
    destroy_entry(testEntry2);
    delete testList;
}

void test_lookup_entry_index(void)
{
    char *testWord1 = (char *)"TESTWORD1"; // Parent
    char *testWord2 = (char *)"TESTWORD2"; // Child diff=1
    char *testWord3 = (char *)"TESTWOR22"; // Child diff=2
    char *testWord4 = (char *)"TESTWORD3"; // Child diff=1,1
    char *testWord5 = (char *)"TESTWORD4"; // Child diff=1,1
    char *testWord6 = (char *)"TESTWORD5"; // Child diff=1,1
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    entry *testEntry4;
    entry *testEntry5;
    entry *testEntry6;
    entry *testEntry7;
    entry *testEntry8;
    entry *testEntry9;
    entry *testEntry10;
    entry *testEntry11;
    entry *testEntry12;
    InitializeIndex();
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);
    create_entry(&testWord4, &testEntry4);
    create_entry(&testWord5, &testEntry5);
    create_entry(&testWord6, &testEntry6);
    create_entry(&testWord1, &testEntry7);
    create_entry(&testWord2, &testEntry8);
    create_entry(&testWord3, &testEntry9);
    create_entry(&testWord4, &testEntry10);
    create_entry(&testWord5, &testEntry11);
    create_entry(&testWord6, &testEntry12);
    entry *resultNode = NULL;
    entry_list *result = NULL;
    entry_list *test_list;
    create_entry_list(&result);
    create_entry_list(&test_list);
    test_list->addEntry(testEntry12);
    test_list->addEntry(testEntry11);
    test_list->addEntry(testEntry10);
    test_list->addEntry(testEntry9);
    test_list->addEntry(testEntry8);
    test_list->addEntry(testEntry7);
    test_list->addEntry(testEntry6);
    test_list->addEntry(testEntry5);
    test_list->addEntry(testEntry4);
    test_list->addEntry(testEntry3);
    test_list->addEntry(testEntry2);
    test_list->addEntry(testEntry1);
    testEntry1->addToPayload(1, 1);
    testEntry2->addToPayload(2, 1);
    testEntry3->addToPayload(3, 1);
    testEntry4->addToPayload(4, 1);
    testEntry5->addToPayload(5, 1);
    testEntry6->addToPayload(6, 1);
    testEntry7->addToPayload(1, 1);
    testEntry8->addToPayload(2, 1);
    testEntry9->addToPayload(3, 0);
    testEntry10->addToPayload(4, 1);
    testEntry11->addToPayload(5, 1);
    testEntry12->addToPayload(6, 1);
    addToIndex(testEntry1, 1, MT_EDIT_DIST, 1);
    addToIndex(testEntry2, 2, MT_EDIT_DIST, 1);
    addToIndex(testEntry3, 3, MT_EDIT_DIST, 1);
    addToIndex(testEntry4, 4, MT_EDIT_DIST, 1);
    addToIndex(testEntry5, 5, MT_EDIT_DIST, 1);
    addToIndex(testEntry6, 6, MT_EDIT_DIST, 1);
    addToIndex(testEntry7, 1, MT_HAMMING_DIST, 1);
    addToIndex(testEntry8, 2, MT_HAMMING_DIST, 1);
    addToIndex(testEntry9, 3, MT_HAMMING_DIST, 0);
    addToIndex(testEntry10, 4, MT_HAMMING_DIST, 1);
    addToIndex(testEntry11, 5, MT_HAMMING_DIST, 1);
    addToIndex(testEntry12, 6, MT_HAMMING_DIST, 1);
    const word nullText = NULL;

    // Test exceptions on bad arguments
    TEST_EXCEPTION(lookup_entry_index(nullText, result, MT_EDIT_DIST), std::exception);

    //editIndex->printTree();

    // This word does not exist, but there are several words with a distance 1 to it
    const word searchTermEdit1 = (char *)"TESTWORD\0";
    TEST_CHECK(lookup_entry_index(searchTermEdit1, result, MT_EDIT_DIST) == EC_SUCCESS);
    // Test that only the correct words matched
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry6->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry5->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry4->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry1->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);

    // Clear the result entry_list
    destroy_entry_list(result);

    // Search for word that exists as second child
    const word searchTermEdit2 = (char *)"TESTWOR22\0";
    create_entry_list(&result);
    TEST_CHECK(lookup_entry_index(searchTermEdit2, result, MT_EDIT_DIST) == EC_SUCCESS);
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry3->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);
    destroy_entry_list(result);

    //////////////// Test For Hamming Indexes
    create_entry_list(&result);
    // This word does not exist, but there are several words with a distance 1 to it
    const word searchTermHamming1 = (char *)"TESTWORD\0";
    TEST_CHECK(lookup_entry_index(searchTermHamming1, result, MT_HAMMING_DIST) == EC_SUCCESS);
    resultNode = result->getHead();
    TEST_CHECK(resultNode == NULL);

    const word searchTermHamming2 = (char *)"TESTWORD1\0";
    TEST_CHECK(lookup_entry_index(searchTermHamming2, result, MT_HAMMING_DIST) == EC_SUCCESS);
    // Test that only the correct words matched
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry6->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry5->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry4->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry1->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);

    // Clear the result entry_list
    destroy_entry_list(result);

    // Search for word that exists as second child
    const word testTermHamming3 = (char *)"TESTWOR22\0";
    create_entry_list(&result);
    TEST_CHECK(lookup_entry_index(testTermHamming3, result, MT_HAMMING_DIST) == EC_SUCCESS);
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry3->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);
    destroy_entry_list(result);

    // Clear the indexes and the entries
    DestroyIndex();
    destroy_entry_list(test_list);
}

void test_addToIndex(void)
{
    char *testWord1 = (char *)"TESTWORD1"; // Parent
    char *testWord2 = (char *)"TESTWORD2"; // Child diff=1
    char *testWord3 = (char *)"TESTWOR22"; // Child diff=2
    char *testWord4 = (char *)"TESTWORD3"; // Child diff=1,1
    char *testWord5 = (char *)"TESTWORD4"; // Child diff=1,1
    char *testWord6 = (char *)"TESTWORD5"; // Child diff=1,1
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    entry *testEntry4;
    entry *testEntry5;
    entry *testEntry6;
    entry *testEntry7;
    entry *testEntry8;
    entry *testEntry9;
    entry *testEntry10;
    entry *testEntry11;
    entry *testEntry12;
    InitializeIndex();
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);
    create_entry(&testWord4, &testEntry4);
    create_entry(&testWord5, &testEntry5);
    create_entry(&testWord6, &testEntry6);
    create_entry(&testWord1, &testEntry7);
    create_entry(&testWord2, &testEntry8);
    create_entry(&testWord3, &testEntry9);
    create_entry(&testWord4, &testEntry10);
    create_entry(&testWord5, &testEntry11);
    create_entry(&testWord6, &testEntry12);
    testEntry1->addToPayload(1, 1);
    testEntry2->addToPayload(2, 1);
    testEntry3->addToPayload(3, 1);
    testEntry4->addToPayload(4, 1);
    testEntry5->addToPayload(5, 1);
    testEntry6->addToPayload(6, 1);
    testEntry7->addToPayload(1, 1);
    testEntry8->addToPayload(2, 1);
    testEntry9->addToPayload(3, 0);
    testEntry10->addToPayload(4, 1);
    testEntry11->addToPayload(5, 1);
    testEntry12->addToPayload(6, 1);
    entry *resultNode = NULL;
    entry_list *result = NULL;
    entry_list *test_list;
    create_entry_list(&result);
    create_entry_list(&test_list);
    test_list->addEntry(testEntry12);
    test_list->addEntry(testEntry11);
    test_list->addEntry(testEntry10);
    test_list->addEntry(testEntry9);
    test_list->addEntry(testEntry8);
    test_list->addEntry(testEntry7);
    test_list->addEntry(testEntry6);
    test_list->addEntry(testEntry5);
    test_list->addEntry(testEntry4);
    test_list->addEntry(testEntry3);
    test_list->addEntry(testEntry2);
    test_list->addEntry(testEntry1);
    addToIndex(testEntry1, 1, MT_EDIT_DIST, 1);
    addToIndex(testEntry2, 2, MT_EDIT_DIST, 1);
    addToIndex(testEntry3, 3, MT_EDIT_DIST, 1);
    addToIndex(testEntry4, 4, MT_EDIT_DIST, 1);
    addToIndex(testEntry5, 5, MT_EDIT_DIST, 1);
    addToIndex(testEntry6, 6, MT_EDIT_DIST, 1);

    addToIndex(testEntry7, 1, MT_HAMMING_DIST, 1);
    addToIndex(testEntry8, 2, MT_HAMMING_DIST, 1);
    addToIndex(testEntry9, 3, MT_HAMMING_DIST, 0);
    addToIndex(testEntry10, 4, MT_HAMMING_DIST, 1);
    addToIndex(testEntry11, 5, MT_HAMMING_DIST, 1);
    addToIndex(testEntry12, 6, MT_HAMMING_DIST, 1);

    // This word does not exist, but there are several words with a distance 1 to it
    const word searchItem = (char *)"TESTWORD\0";
    TEST_CHECK(lookup_entry_index(searchItem, result, MT_EDIT_DIST) == EC_SUCCESS);

    // Test that (only) the removed word does not apear
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry6->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry5->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry4->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry1->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);
    destroy_entry_list(result);

    //////////Test For Hamming Indexes
    create_entry_list(&result);
    const word searchTermHamming = (char *)"TESTWORD1\0";
    TEST_CHECK(lookup_entry_index(searchTermHamming, result, MT_HAMMING_DIST) == EC_SUCCESS);

    // Test that only the correct words matched
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry6->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry5->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry4->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry1->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);

    // Destroy the Indexes
    DestroyIndex();

    // Clear the result entry_list
    destroy_entry_list(result);
    destroy_entry_list(test_list);
}

void test_removeFromIndex(void)
{
    char *testWord1 = (char *)"TESTWORD1"; // Parent
    char *testWord2 = (char *)"TESTWORD2"; // Child diff=1
    char *testWord3 = (char *)"TESTWOR22"; // Child diff=2
    char *testWord4 = (char *)"TESTWORD3"; // Child diff=1,1
    char *testWord5 = (char *)"TESTWORD4"; // Child diff=1,1
    char *testWord6 = (char *)"TESTWORD5"; // Child diff=1,1
    entry *testEntry1;
    entry *testEntry2;
    entry *testEntry3;
    entry *testEntry4;
    entry *testEntry5;
    entry *testEntry6;
    entry *testEntry7;
    entry *testEntry8;
    entry *testEntry9;
    entry *testEntry10;
    entry *testEntry11;
    entry *testEntry12;
    InitializeIndex();
    create_entry(&testWord1, &testEntry1);
    create_entry(&testWord2, &testEntry2);
    create_entry(&testWord3, &testEntry3);
    create_entry(&testWord4, &testEntry4);
    create_entry(&testWord5, &testEntry5);
    create_entry(&testWord6, &testEntry6);
    create_entry(&testWord1, &testEntry7);
    create_entry(&testWord2, &testEntry8);
    create_entry(&testWord3, &testEntry9);
    create_entry(&testWord4, &testEntry10);
    create_entry(&testWord5, &testEntry11);
    create_entry(&testWord6, &testEntry12);
    testEntry1->addToPayload(1, 1);
    testEntry2->addToPayload(2, 1);
    testEntry3->addToPayload(3, 1);
    testEntry4->addToPayload(4, 1);
    testEntry5->addToPayload(5, 1);
    testEntry6->addToPayload(6, 1);
    testEntry7->addToPayload(1, 1);
    testEntry8->addToPayload(2, 1);
    testEntry9->addToPayload(3, 0);
    testEntry10->addToPayload(4, 1);
    testEntry11->addToPayload(5, 1);
    testEntry12->addToPayload(6, 1);
    entry *resultNode = NULL;
    entry_list *result = NULL;
    entry_list *test_list;
    create_entry_list(&result);
    create_entry_list(&test_list);
    test_list->addEntry(testEntry12);
    test_list->addEntry(testEntry11);
    test_list->addEntry(testEntry10);
    test_list->addEntry(testEntry9);
    test_list->addEntry(testEntry8);
    test_list->addEntry(testEntry7);
    test_list->addEntry(testEntry6);
    test_list->addEntry(testEntry5);
    test_list->addEntry(testEntry4);
    test_list->addEntry(testEntry3);
    test_list->addEntry(testEntry2);
    test_list->addEntry(testEntry1);
    addToIndex(testEntry1, 1, MT_EDIT_DIST, 1);
    addToIndex(testEntry2, 2, MT_EDIT_DIST, 1);
    addToIndex(testEntry3, 3, MT_EDIT_DIST, 1);
    addToIndex(testEntry4, 4, MT_EDIT_DIST, 1);
    addToIndex(testEntry5, 5, MT_EDIT_DIST, 1);
    addToIndex(testEntry6, 6, MT_EDIT_DIST, 1);

    addToIndex(testEntry7, 1, MT_HAMMING_DIST, 1);
    addToIndex(testEntry8, 2, MT_HAMMING_DIST, 1);
    addToIndex(testEntry9, 3, MT_HAMMING_DIST, 0);
    addToIndex(testEntry10, 4, MT_HAMMING_DIST, 1);
    addToIndex(testEntry11, 5, MT_HAMMING_DIST, 1);
    addToIndex(testEntry12, 6, MT_HAMMING_DIST, 1);

    const word textToRemove = (char *)"TESTWORD1\0";
    removeFromIndex(textToRemove, 1, MT_EDIT_DIST);

    // This word does not exist, but there are several words with a distance 1 to it
    const word searchItem = (char *)"TESTWORD\0";
    TEST_CHECK(lookup_entry_index(searchItem, result, MT_EDIT_DIST) == EC_SUCCESS);

    // Test that (only) the removed word does not apear
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry6->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry5->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry4->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);
    destroy_entry_list(result);

    //////////Test For Hamming Indexes
    create_entry_list(&result);
    removeFromIndex(textToRemove, 1, MT_HAMMING_DIST);
    const word searchTermHamming2 = (char *)"TESTWORD1\0";
    TEST_CHECK(lookup_entry_index(searchTermHamming2, result, MT_HAMMING_DIST) == EC_SUCCESS);

    // Test that only the correct words matched
    resultNode = result->getHead();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry6->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry5->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry4->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(strcmp(resultNode->getWord(), testEntry2->getWord()) == 0);
    resultNode = resultNode->getNext();
    TEST_CHECK(resultNode == NULL);

    // Destroy the Indexes
    DestroyIndex();

    // Clear the result entry_list
    destroy_entry_list(result);
    destroy_entry_list(test_list);
}

//////////////////////////// Result ////////////////////////////

void test_merge_sort(void)
{
    QueryID queryIds[10] = {12, 2, 4, 1, 7, 57, 83, 5, 89, 9};
    mergeSort(queryIds, 0, 9);
    TEST_CHECK((queryIds[0] == 1));
    TEST_CHECK((queryIds[1] == 2));
    TEST_CHECK((queryIds[9] == 89));
    TEST_CHECK((queryIds[8] == 83));
}

void test_result_constructor_getters(void)
{
    // Create 2 result entries
    QueryID *testQ1 = new QueryID[1];
    testQ1[0] = 3;
    result *testResult = new result(1, (DocID)1, testQ1);
    TEST_CHECK(testResult->getDocID() == 1);
    TEST_CHECK(testResult->getNumRes() == 1);
    TEST_CHECK(testResult->getQueries() == testQ1);

    delete testResult;
    delete[] testQ1;
}

void test_result_addResult(void)
{
    // Create 2 result entries
    QueryID *testQ1 = new QueryID[1];
    testQ1[0] = 3;
    QueryID *testQ2 = new QueryID[2];
    testQ2[0] = 4;
    testQ2[1] = 5;
    result *testResult1 = new result(1, (DocID)1, testQ1);
    result *testResult2 = new result(2, (DocID)2, testQ2);
    testResult1->addResult(testResult2);

    // Test that the second result is accessible and was formed properly
    TEST_CHECK(testResult1->getNext()->getDocID() == 2);
    TEST_CHECK(testResult1->getNext()->getNumRes() == 2);
    TEST_CHECK(testResult1->getNext()->getQueries() == testQ2);

    // delete list because normally main would delete it
    result *prev, *curr = testResult1;
    while (curr)
    {
        prev = curr;
        curr = curr->getNext();
        delete prev;
    }
    // Cleanup
    delete[] testQ1;
    delete[] testQ2;
}

void test_result_storeResult(void)
{
    // Create 2 result entries
    InitializeIndex();
    QueryID *testQ1 = new QueryID[1];
    testQ1[0] = 3;
    QueryID *testQ2 = new QueryID[2];
    testQ2[0] = 4;
    testQ2[1] = 5;
    storeResult(1, (DocID)1, testQ1);
    storeResult(2, (DocID)2, testQ2);

    // Test that the first result was formed properly
    TEST_CHECK(resultList->getDocID() == 1);
    TEST_CHECK(resultList->getNumRes() == 1);
    TEST_CHECK(resultList->getQueries() == testQ1);

    // Test that the second result is accessible and was formed properly
    TEST_CHECK(resultList->getNext()->getDocID() == 2);
    TEST_CHECK(resultList->getNext()->getNumRes() == 2);
    TEST_CHECK(resultList->getNext()->getQueries() == testQ2);

    // delete list because normally main would delete it
    result *prev, *curr = resultList;
    while (curr)
    {
        prev = curr;
        curr = curr->getNext();
        delete prev;
    }
    // Cleanup
    delete[] testQ1;
    delete[] testQ2;
    DestroyIndex();
}

//////////////////////////// Interface ////////////////////////////

void test_InitializeIndex_DestroyIndex(void)
{
    InitializeIndex();
    // everything well created
    TEST_CHECK((EntryList != NULL));
    TEST_CHECK((QT != NULL));
    TEST_CHECK((ET != NULL));
    TEST_CHECK((editIndex != NULL));
    for (int i = 0; i < 27; i++)
    {
        TEST_CHECK((hammingIndexes[i] != NULL));
    }
    DestroyIndex();
}

void test_StartQuery_EndQuery(void)
{
    InitializeIndex();
    char *q_words = new char[MAX_QUERY_LENGTH]();

    // create queries and delete them and check if they are well inserted in structs
    memcpy(q_words, "blue purple green black yellow", MAX_QUERY_LENGTH);
    StartQuery(0, q_words, MT_EXACT_MATCH, 0);
    pthread_mutex_lock(&unfinishedQueriesLock);
    if (unfinishedQueries != 0)
    {
        pthread_cond_wait(&unfinishedQueriesCond, &unfinishedQueriesLock);
    }
    pthread_mutex_unlock(&unfinishedQueriesLock);
    TEST_CHECK((QT->getQueriesPerBucket(hashFunctionById(0)) == 1));
    TEST_CHECK((EntryList->getEntryNum() == 5));
    TEST_CHECK((ET->getEntriesPerBucket(hashFunction((char *)"blue")) == 1)); // well inserted in entry table
    TEST_CHECK(((ET->getBucket(hashFunction((char *)"blue"))[0])->getPayload()->getId() == 0));
    EndQuery(0);
    TEST_CHECK(((ET->getBucket(hashFunction((char *)"blue"))[0])->getPayload() == NULL));
    TEST_CHECK((ET->getEntriesPerBucket(hashFunction((char *)"blue")) == 1)); // still in there but with NULL payload

    memcpy(q_words, "blue purple green black yellow", MAX_QUERY_LENGTH);
    StartQuery(1, q_words, MT_EDIT_DIST, 1);
    pthread_mutex_lock(&unfinishedQueriesLock);
    if (unfinishedQueries != 0)
    {
        pthread_cond_wait(&unfinishedQueriesCond, &unfinishedQueriesLock);
    }
    pthread_mutex_unlock(&unfinishedQueriesLock);
    TEST_CHECK((QT->getQueriesPerBucket(hashFunctionById(1)) == 1));
    TEST_CHECK((EntryList->getEntryNum() == 10));
    TEST_CHECK((strcmp(editIndex->getEntry()->getWord(), "blue") == 0)); // well inserted in index

    memcpy(q_words, "blue purple green black yellow", MAX_QUERY_LENGTH);
    StartQuery(2, q_words, MT_HAMMING_DIST, 2);
    pthread_mutex_lock(&unfinishedQueriesLock);
    if (unfinishedQueries != 0)
    {
        pthread_cond_wait(&unfinishedQueriesCond, &unfinishedQueriesLock);
    }
    pthread_mutex_unlock(&unfinishedQueriesLock);
    TEST_CHECK((QT->getQueriesPerBucket(hashFunctionById(1)) == 1));
    TEST_CHECK((EntryList->getEntryNum() == 15));
    TEST_CHECK((strcmp(hammingIndexes[0]->getEntry()->getWord(), "blue") == 0)); // well inserted in index

    EndQuery(2);
    TEST_CHECK((hammingIndexes[0]->getEntry()->getPayload() == NULL)); // well deleted
    EndQuery(1);
    TEST_CHECK((editIndex->getEntry()->getPayload() == NULL)); // well deleted

    delete[] q_words;
    DestroyIndex();
}

void test_ExactMatch_WordLookup(void)
{
    InitializeIndex();
    char *q_words = new char[MAX_QUERY_LENGTH]();
    memcpy(q_words, "blue purple green black yellow", MAX_QUERY_LENGTH);
    // create query
    StartQuery(0, q_words, MT_EXACT_MATCH, 0);
    pthread_mutex_lock(&unfinishedQueriesLock);
    if (unfinishedQueries != 0)
    {
        pthread_cond_wait(&unfinishedQueriesCond, &unfinishedQueriesLock);
    }
    pthread_mutex_unlock(&unfinishedQueriesLock);
    // create document
    char *d_words = new char[MAX_DOC_LENGTH]();
    strcpy(d_words, "blue purple green black yellow blue purple green black yellow");
    Document *d = new Document(0, d_words);
    // deduplicate document
    DocTable *DT;
    DT = new DocTable(0);
    DocumentDeduplication(d, DT);

    // initialize entrylist with potential results
    entry_list *result = NULL;
    create_entry_list(&result);

    // check for match
    for (int bucket = 0; bucket < MAX_BUCKETS; ++bucket) // for each bucket
    {
        if (DT->getWordsPerBucket(bucket))
        {
            for (int word = 0; word < DT->getWordsPerBucket(bucket); ++word) // for each word
            {
                ET->wordLookup(DT->getBucket(bucket)[word], result);
            }
        }
    }
    TEST_CHECK((result->getEntryNum() == 5)); // 5 common words found

    delete d;
    delete DT;
    delete[] d_words;
    delete[] q_words;
    destroy_entry_list(result);
    DestroyIndex();
}

void test_GetNextAvailRes(void)
{
    // Create 2 result entries
    InitializeIndex();
    unfinishedDocs = 2; // Emulate Docs at the Queue
    QueryID *testQ1 = new QueryID[1];
    testQ1[0] = 3;
    QueryID *testQ2 = new QueryID[2];
    testQ2[0] = 4;
    testQ2[1] = 5;
    storeResult(1, (DocID)1, testQ1);
    storeResult(2, (DocID)2, testQ2);
    // The variables that will receive the result
    unsigned int receivedNum;
    DocID receivedDocID;
    QueryID *receivedQueryIDs;
    // Check that the first result is received properly
    GetNextAvailRes(&receivedDocID, &receivedNum, &receivedQueryIDs);

    TEST_CHECK(receivedDocID == 1);
    TEST_CHECK(receivedNum == 1);
    TEST_CHECK(receivedQueryIDs == testQ1);

    // Check that the second result is received properly
    GetNextAvailRes(&receivedDocID, &receivedNum, &receivedQueryIDs);
    TEST_CHECK(receivedDocID == 2);
    TEST_CHECK(receivedNum == 2);
    TEST_CHECK(receivedQueryIDs == testQ2);

    // Cleanup
    delete[] testQ1;
    delete[] testQ2;
    DestroyIndex();
}

///////////////// matcherQuery and matchedQueryList //////////////////

void test_matchedQuery_constructor_getters(void)
{
    matchedQuery *testQuery1 = new matchedQuery(1);
    TEST_CHECK(testQuery1->getId() == 1);
    TEST_CHECK(testQuery1->getNext() == NULL);
    delete testQuery1;
}

void test_matchedQuery_setNext(void)
{
    matchedQuery *testQuery1 = new matchedQuery(1);
    matchedQuery *testQuery2 = new matchedQuery(2);
    testQuery1->setNext(testQuery2);
    TEST_CHECK(testQuery1->getNext() == testQuery2);
    delete testQuery1;
    delete testQuery2;
}

void test_matchedQueryList_constructor_addToList_getHead(void)
{
    matchedQueryList *testList = new matchedQueryList();
    TEST_CHECK(testList->getHead() == NULL);
    testList->addToList(1);
    testList->addToList(2);
    TEST_CHECK(testList->getHead()->getId() == 2);
    TEST_CHECK(testList->getHead()->getNext()->getId() == 1);
    delete testList;
}

void test_job(void)
{
    char *d_words = new char[MAX_DOC_LENGTH]();
    strcpy(d_words, "blue purple green black yellow blue purple green black yellow");
    MatchDocumentArgs *testArgs = new MatchDocumentArgs(1, d_words);
    Job *testJob = new Job(MatchDocumentJob, testArgs);
    TEST_CHECK(((MatchDocumentArgs *)(testJob->getArgs()))->getDocId() == 1);
    TEST_CHECK(strcmp(((MatchDocumentArgs *)(testJob->getArgs()))->getDocStr(), d_words) == 0);
    delete testJob;
    delete[] d_words;
    delete testArgs;
}

void test_JobScheduler_create_destroy_get(void)
{
    TEST_CHECK(scheduler == NULL);

    pthread_mutex_lock(&queueLock);
    scheduler = new JobScheduler(10);
    pthread_mutex_unlock(&queueLock);

    TEST_CHECK(scheduler != NULL);

    pthread_mutex_lock(&queueLock);
    TEST_CHECK(scheduler->getQueue()->isEmpty());
    pthread_mutex_unlock(&queueLock);

    TEST_CHECK(scheduler->getThreads() == 10);
    globalExit = true;
    pthread_cond_broadcast(&queueEmptyCond);
    pthread_cond_broadcast(&resEmptyCond);
    pthread_cond_broadcast(&unfinishedQueriesCond);
    delete scheduler;
    scheduler = NULL;
}

void test_JobScheduler_submitJob(void)
{
    InitializeIndex();
    pthread_mutex_lock(&queueLock);
    TEST_CHECK(scheduler->getQueue()->isEmpty());
    pthread_mutex_unlock(&queueLock);

    StartQuery(0, "WORD", MT_EDIT_DIST, 1);
    EndQuery(0);
    pthread_mutex_lock(&unfinishedQueriesLock);
    if (unfinishedQueries != 0)
    {
        pthread_cond_wait(&unfinishedQueriesCond, &unfinishedQueriesLock);
    }
    DestroyIndex();
    pthread_mutex_unlock(&unfinishedQueriesLock);
}

TEST_LIST = {
    {"Query", test_query},
    {"Document", test_document},
    {"Entry", test_entry},
    {"Entry payload Set and Get", test_entry_set_get_payload},
    {"Entry emptyPayload", test_entry_emptyPayload},
    {"EntryList", test_entrylist},
    {"Binary Search", test_binary_search},
    {"Query Binary Search", test_query_binary_search},
    {"Entry Binary Search", test_entry_binary_search},
    {"Result Binary Search", test_result_binary_search},
    {"Find Query", test_find_query},
    {"Find Entry", test_find_entry},
    {"Hash Function", test_hash_function},
    {"Document Deduplication", test_deduplication},
    {"Doc Table", test_doc_table},
    {"Query Table", test_query_table},
    {"Entry Table", test_entry_table},
    {"Exact Match", test_exact_match},
    {"Hamming Distance", test_hamming},
    {"Edit Distance", test_edit},
    {"Stack Add", test_stack_add},
    {"Stack Pop", test__stack_pop},
    {"Create StackNode", test_stackNode_create},
    {"StackNode Pop", test_stackNode_pop},
    {"indexNode Constructor and Add Entry", test_indexNode_constructor_addEntry},
    {"indexNode getEntry", test_indexNode_getEntry},
    {"indexNode getChildren", test_indexNode_getChildren},
    {"indexNode getMatchingType", test_indexNode_getMatchingType},
    {"indexList Constructor", test_indexList_constructor},
    {"indexList addEntry", test_indexList_addToList},
    {"indexList getDistanceFromParent", test_indexList_getDistanceFromParent},
    {"indexList getNode", test_indexList_getNode},
    {"indexList getNext", test_indexList_getNext},
    {"lookup_entry_index", test_lookup_entry_index},
    {"Remove word from index", test_removeFromIndex},
    {"Add word to index", test_addToIndex},
    {"payloadNode Constructor, Getters", test_payloadNode_constructor_getters},
    {"payloadNode setNext", test_payloadNode_setNext},
    {"payload addToPayload", test_payload_addToPayload},
    {"payload deleteNode", test_payload_deletePayloadNode},
    {"Merge Sort", test_merge_sort},
    {"result constructor and getter functions", test_result_constructor_getters},
    {"addResult", test_result_addResult},
    {"store result", test_result_storeResult},
    {"InitializeIndex and DestroyIndex", test_InitializeIndex_DestroyIndex},
    {"StartQuery and EndQuery", test_StartQuery_EndQuery},
    {"ExactMatch WordLookup", test_ExactMatch_WordLookup},
    {"GetNextAvailRes", test_GetNextAvailRes},
    {"matchedQuery constructor and getters", test_matchedQuery_constructor_getters},
    {"matchedQuery setNext", test_matchedQuery_setNext},
    {"matchedQueryList constructor addToList getHead", test_matchedQueryList_constructor_addToList_getHead},
    {"job", test_job},
    {"JobScheduler constructor, destructor and get functions", test_JobScheduler_create_destroy_get},
    {"JobScheduler submitJob", test_JobScheduler_submitJob},
    {NULL, NULL}};
