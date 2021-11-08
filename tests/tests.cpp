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

// Match functions
void test_exact_match(void)
{   
    word word1 = (word)"hello";
    word word2 = (word)"hello";
    word word3 = (word)"hellno";
    const word ptrNull = NULL;
    // The words are the same
    TEST_ASSERT(exactMatch(word1,word2) == true);
    // The words are different
    TEST_ASSERT(exactMatch(word1,word3) == false);
    // Test the exceptions for NULL pointers
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
    // Test distance for good input
    TEST_ASSERT(hammingDistance(word1,word2) == 0);
    TEST_ASSERT(hammingDistance(word1,word3) == 2);
    TEST_ASSERT(hammingDistance(word1,word4) == 1);
    // Test exceptions for bad input (NULL Pointers)
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
    // Test distance for good input
    TEST_ASSERT( editDistance(word1,word2) == 0);
    TEST_ASSERT( editDistance(word1,word3) == 1);
    TEST_ASSERT( editDistance(word1,word4) == 1);
    TEST_ASSERT( editDistance(word1,word5) == 1);
    // Test exceptions for bad input (NULL Pointers)
    TEST_EXCEPTION( editDistance(ptrNull, word1), std::exception);
    TEST_EXCEPTION( editDistance(ptrNull, ptrNull), std::exception);
}

// Stack
void test_add(void) {
    // Setup
    indexNode* nullPTR = NULL;
	Stack testStack;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
    indexNode* testIndex3 = new indexNode(testEntry3);

    // Try to add NULL
    TEST_EXCEPTION(testStack.add(NULL), std::exception);
    TEST_EXCEPTION(testStack.add(&nullPTR), std::exception);
    TEST_ASSERT(testStack.getSize() == 0);

    // Add one, check size
    testStack.add(&testIndex1);
    TEST_ASSERT(testStack.getSize() == 1);

    // Add second, check size
    testStack.add(&testIndex1);
    TEST_ASSERT(testStack.getSize() == 2);

    // Add third, check size
    testStack.add(&testIndex3);
    TEST_ASSERT(testStack.getSize() == 3);

    // Cleanup
    while(testStack.getSize() > 0) {
        testStack.pop();
    }
    delete testIndex1;
    delete testIndex2;
    delete testIndex3;
    delete testEntry1;
    delete testEntry2;
    delete testEntry3;

}

void test_pop(void) {
    // Setup
    indexNode* nodeToReturn;
	Stack testStack;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
    indexNode* testIndex3 = new indexNode(testEntry3);

    // Test empty stack
    nodeToReturn = testStack.pop();
    TEST_ASSERT(nodeToReturn == NULL);      // We should get NULL, the stack is empty

    // Test stack with one item
    testStack.add(&testIndex1);
    nodeToReturn = testStack.pop();
    TEST_ASSERT(nodeToReturn == testIndex1); // We should get node we put in
    nodeToReturn = testStack.pop();
    TEST_ASSERT(nodeToReturn == NULL);      // The stack is empty, return NULL again

    // Test stack with multiple items
    testStack.add(&testIndex1);         // Add multiple indexings
    testStack.add(&testIndex2);
    testStack.add(&testIndex3);
    nodeToReturn = testStack.pop();
    TEST_ASSERT(nodeToReturn == testIndex3);    // Get them one by one
    nodeToReturn = testStack.pop();
    TEST_ASSERT(nodeToReturn == testIndex2);
    nodeToReturn = testStack.pop();
    TEST_ASSERT(nodeToReturn == testIndex1);
    nodeToReturn = testStack.pop();
    TEST_ASSERT(nodeToReturn == NULL);          // The stack is empty, return NULL again

    // Cleanup
    delete testIndex1;
    delete testIndex2;
    delete testIndex3;
    delete testEntry1;
    delete testEntry2;
    delete testEntry3;
}

void test_stackNode_create(void) {
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
    stackNode* testNode1 = NULL;
    stackNode* testNode2 = NULL;

    // Test exception for NULL input
    TEST_EXCEPTION(testNode1 = new stackNode(NULL), std::exception);
    // Check that the pointer did not change
    TEST_ASSERT(testNode1 == NULL);

    // Check that this is the only node
    testNode1 = new stackNode(testIndex1);
    TEST_ASSERT(testNode1->getNext() == NULL);

    // Check that the first can see the second
    testNode2 = new stackNode(testIndex2, testNode1);
    TEST_ASSERT(testNode2->getNext() == testNode1);

    // Cleanup
    delete testNode1;
    delete testNode2;
    delete testIndex1;
    delete testIndex2;
    delete testEntry1;
    delete testEntry2;
}

void test_stackNode_pop(void) {
    indexNode* nodeToReturn;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
    stackNode* testNode1 = NULL;
    stackNode* testNode2 = NULL;
    stackNode* childNodeToReturn;

    // Test adding and removing single node
    testNode1 = new stackNode(testIndex1);
    testNode1->pop(&nodeToReturn, &childNodeToReturn);
    TEST_ASSERT (nodeToReturn == testIndex1);
    TEST_ASSERT (childNodeToReturn == NULL);

    // Test adding and removind 2 nodes
    testNode1 = new stackNode(testIndex1);
    testNode2 = new stackNode(testIndex2, testNode1);
    testNode2->pop(&nodeToReturn, &childNodeToReturn);
    TEST_ASSERT (nodeToReturn == testIndex2);
    TEST_ASSERT (childNodeToReturn == testNode1);
    testNode1->pop(&nodeToReturn, &childNodeToReturn);
    TEST_ASSERT (nodeToReturn == testIndex1);
    TEST_ASSERT (childNodeToReturn == NULL);

    delete testIndex1;
    delete testIndex2;
    delete testEntry1;
    delete testEntry2;
}

//////////////////////////// Index ////////////////////////////
void test_indexNode_construction_addEntry(void) {
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    word testWord4 = (word)"TESTWOR33";
    

    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    entry* testEntry4 = new entry(testWord4);

    entry* nullPTR = NULL;
    indexNode* head;

    // Test creating with NULL pointer
    TEST_EXCEPTION(head = new indexNode(nullPTR,MT_EDIT_DIST), std::exception);

    // Test creating with exact match (considered invalid for tree creation)
    TEST_EXCEPTION(head = new indexNode(testEntry1,MT_EXACT_MATCH), std::exception);

    // Test insertion and exceptions when adding duplicates
    head = new indexNode(testEntry1);
    TEST_EXCEPTION(head->addEntry(nullPTR), std::exception);
    TEST_ASSERT(head->addEntry(testEntry2) == EC_SUCCESS);
    TEST_ASSERT(head->addEntry(testEntry3) == EC_SUCCESS);
    TEST_EXCEPTION(head->addEntry(testEntry3), std::exception);
    TEST_ASSERT(head->addEntry(testEntry4) == EC_SUCCESS);
    TEST_EXCEPTION(head->addEntry(testEntry4), std::exception);

    delete testEntry1;
    delete testEntry2;
    delete testEntry3;
    delete testEntry4;
    delete head;
}

void test_indexNode_getEntry(void) {
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    indexNode* head = new indexNode(testEntry1);
    TEST_ASSERT(head->getEntry() == testEntry1);    
    
    delete testEntry1;
    delete head;
}

void test_indexNode_getChildren(void) {
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORd3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexNode* head;
    indexList* listOfChildren;

    head = new indexNode(testEntry1);
    head->addEntry(testEntry2);
    head->addEntry(testEntry3);

    listOfChildren=head->getChildren();
    // Test the correct order of the nodes in the list
    TEST_ASSERT(listOfChildren->getNode()->getEntry() == testEntry2);
    TEST_ASSERT(listOfChildren->getDistanceFromParent() == 1);
    TEST_ASSERT(listOfChildren->getNext()->getNode()->getEntry() == testEntry3);
    TEST_ASSERT(listOfChildren->getNext()->getDistanceFromParent() == 2);

    delete testEntry1;
    delete testEntry2;
    delete testEntry3;
    delete head;
}

void test_indexNode_getMatchingType(void) {
    
    // Setup
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = NULL;
    indexNode* head = NULL;

    // Test with default args
    testEntry1 = new entry(testWord1);
    head = new indexNode(testEntry1);
    TEST_ASSERT(head->getMatchingType() == MT_EDIT_DIST);    
    delete testEntry1;
    delete head;

    // Test by explicit argument EDIT_DIST
    testEntry1 = new entry(testWord1);
    head = new indexNode(testEntry1, MT_EDIT_DIST);
    TEST_ASSERT(head->getMatchingType() == MT_EDIT_DIST);
    delete testEntry1;    
    delete head;

    // Test by explicit argument EDIT_DIST
    testEntry1 = new entry(testWord1);
    head = new indexNode(testEntry1, MT_HAMMING_DIST);
    TEST_ASSERT(head->getMatchingType() == MT_HAMMING_DIST);
    delete testEntry1;    
    delete head;
}

//////////////////////////// indexList ////////////////////////////

void test_indexList_constructor(void) {
    // Setup
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    entry* nullTestEntry = NULL;
    indexList* testList = NULL;

    // Test indexList Constructor Edge Cases
    TEST_EXCEPTION(testList = new indexList(nullTestEntry, 1, MT_EDIT_DIST), std::exception);
    TEST_EXCEPTION(testList = new indexList(testEntry1, 1,  MT_EXACT_MATCH), std::exception);
    TEST_EXCEPTION(testList = new indexList(testEntry1, 0,  MT_EDIT_DIST), std::exception);
    TEST_EXCEPTION(testList = new indexList(testEntry1, -1,  MT_EDIT_DIST), std::exception);

    testList = new indexList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT (testList != NULL);

    delete testEntry1;
    delete testList;
}

void test_indexList_addToList(void) {
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORd3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    entry* nullTestEntry = NULL;
    indexList* testList = NULL;

    // Test adding good inputs
    testList = new indexList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT(testList->addToList(testEntry3, 2) == 0);
    TEST_ASSERT(testList->addToList(testEntry2, 1) == 0);

    // Test NULL Pointers and invalid distance from parent
    TEST_EXCEPTION(testList->addToList(testEntry3, 0), std::exception);
    TEST_EXCEPTION(testList->addToList(testEntry3, -1), std::exception);
    TEST_EXCEPTION(testList->addToList(nullTestEntry, 3), std::exception);

    delete testEntry1;
    delete testEntry2;
    delete testEntry3;
    delete testList;
}

void test_indexList_getDistanceFromParent(void) {
    // Setup
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    indexList* testList = NULL;

    testList = new indexList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT (testList->getDistanceFromParent() == 1);

    delete testEntry1;
    delete testList;
}

void test_indexList_getNode(void) {
    // Setup
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    indexList* testList = NULL;

    testList = new indexList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT (testList->getNode()->getEntry() == testEntry1);


    delete testEntry1;
    delete testList;
}

void test_indexList_getNext(void) {
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD22";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    indexList* testList = NULL;

    testList = new indexList(testEntry1, 1,  MT_EDIT_DIST);
    testList->addToList(testEntry2, 2);
    TEST_ASSERT (testList->getNext()->getNode()->getEntry() == testEntry2);

    delete testEntry2;
    delete testEntry1;
    delete testList;
}

//////////////////////////// indexInterface ////////////////////////////
void test_build_entry_index(void) {
    // Setup
    char* word1=(char*)"TESTWORD1";   // Parent
    char* word2=(char*)"TESTWORD2";   // Child diff=1
    char* word3=(char*)"TESTWOR22";   // Child diff=2
    char* word4=(char*)"TESTWORD3";   // Child diff=1,1
    char* word5=(char*)"TESTWORD4";   // Child diff=1,1
    char* word6=(char*)"TESTWORD5";   // Child diff=1,1
    entry* testEntry1 = new entry(word1);
    entry* testEntry2 = new entry(word2);
    entry* testEntry3 = new entry(word3);
    entry* testEntry4 = new entry(word4);
    entry* testEntry5 = new entry(word5);
    entry* testEntry6 = new entry(word6);
    entry_list* test_list = new entry_list();
    entry_list* nullList = NULL;
    test_list->addEntry(testEntry6);
    test_list->addEntry(testEntry5);
    test_list->addEntry(testEntry4);
    test_list->addEntry(testEntry3);
    test_list->addEntry(testEntry2);
    test_list->addEntry(testEntry1);
    indexNode* tree = NULL;
    
    // Test exception when giving a NULL list
    TEST_EXCEPTION(build_entry_index(nullList, MT_EDIT_DIST ,&tree), std::exception);

    // Test returned EC_FAIL when passing invalid distance metric
    TEST_ASSERT(build_entry_index(test_list, MT_EXACT_MATCH ,&tree) == EC_FAIL);

    // Test Correct Index Creation
    TEST_ASSERT(build_entry_index(test_list, MT_EDIT_DIST ,&tree) == EC_SUCCESS);

    destroy_entry_index(tree);
    destroy_entry_list(test_list);
}


void test_lookup_entry_index(void) {
    char* word1=(char*)"TESTWORD1";   // Parent
    char* word2=(char*)"TESTWORD2";   // Child diff=1
    char* word3=(char*)"TESTWOR22";   // Child diff=2
    char* word4=(char*)"TESTWORD3";   // Child diff=1,1
    char* word5=(char*)"TESTWORD4";   // Child diff=1,1
    char* word6=(char*)"TESTWORD5";   // Child diff=1,1
    entry* testEntry1 = new entry(word1);
    entry* testEntry2 = new entry(word2);
    entry* testEntry3 = new entry(word3);
    entry* testEntry4 = new entry(word4);
    entry* testEntry5 = new entry(word5);
    entry* testEntry6 = new entry(word6);
    entry* tempResult = NULL;
    entry* resultNode = NULL;
    entry_list* result = new entry_list();
    entry_list* test_list = new entry_list();
    test_list->addEntry(testEntry6);
    test_list->addEntry(testEntry5);
    test_list->addEntry(testEntry4);
    test_list->addEntry(testEntry3);
    test_list->addEntry(testEntry2);
    test_list->addEntry(testEntry1);
    indexNode* tree = NULL;
    indexNode* nullTree = NULL;
    build_entry_index(test_list, MT_EDIT_DIST ,&tree);
    const word text1 = (char*)"TESTWOR22\0";
    const word nullText = NULL;

    // Test exceptions on bad arguments
    TEST_EXCEPTION(lookup_entry_index(&nullText, tree, -1, result), std::exception);
    TEST_EXCEPTION(lookup_entry_index(&text1,nullTree, 0, result), std::exception);
    TEST_EXCEPTION(lookup_entry_index(&text1,tree, -1, result), std::exception);

    // Search for word that exists with exact match
    TEST_ASSERT(lookup_entry_index(&text1,tree, 0, result) == EC_SUCCESS);
    resultNode=result->getHead();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry3->getWord())==0);
    resultNode=result->getHead();

    // Clear the result entry_list
    while(resultNode) {
        entry* tempResult = NULL;
        tempResult=resultNode->getNext();
        delete resultNode;
        resultNode = tempResult;
    }
    delete result;
    result = NULL;

    // Search for word that does not exist with exact match
    const word text2 = (char*)"TESTWOR\0";
    TEST_ASSERT ( lookup_entry_index(&text2, tree, 0, result) == EC_SUCCESS);
    TEST_ASSERT ( resultNode == NULL);
    delete result;
    result = NULL;

    // This word does not exist, but there are several words with a distance 1 to it
    const word text3 = (char*)"TESTWORD\0";
    result = new entry_list();
    TEST_ASSERT ( lookup_entry_index(&text3, tree, 1, result) == EC_SUCCESS);
    // Test that only the correct words matched
    resultNode = result->getHead();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry6->getWord())==0);
    resultNode = resultNode->getNext();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry5->getWord())==0);
    resultNode = resultNode->getNext();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry4->getWord())==0);
    resultNode = resultNode->getNext();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry2->getWord())==0);
    resultNode = resultNode->getNext();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry1->getWord())==0);
    resultNode = resultNode->getNext();
    TEST_ASSERT(resultNode == NULL);

    // Clear the result entry_list
    resultNode=result->getHead();
    while(resultNode){
        tempResult = resultNode->getNext();
        delete resultNode;
        resultNode = tempResult;
    }
    delete result;
    result = NULL;


    // Search for word that exists as second child using exact match
    const word text5 = (char*)"TESTWOR22\0";
    result = new entry_list();
    TEST_ASSERT ( lookup_entry_index(&text5, tree, 0, result) == EC_SUCCESS);
    resultNode = result->getHead();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry3->getWord())==0);
    resultNode = resultNode->getNext();
    TEST_ASSERT(resultNode == NULL);
    resultNode=result->getHead();
    // Clear the result entry_list
    while(resultNode){
        tempResult = NULL;
        tempResult=resultNode->getNext();
        delete resultNode;
        resultNode = tempResult;
    }
    delete result;
    result = NULL;

    destroy_entry_index(tree);
    destroy_entry_list(test_list);
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
    {"Stack Add", test_add},
    {"Stack Remove", test_pop},
    {"Create StackNode", test_stackNode_create},
    {"Pop StackNode", test_stackNode_pop},
    {"indexNode Constructor and Add Entry", test_indexNode_construction_addEntry},
    {"indexNode getEntry", test_indexNode_getEntry},
    {"indexNode getChildren", test_indexNode_getChildren},
    {"indexNode getMatchingType", test_indexNode_getMatchingType},
    {"indexList Constructor", test_indexList_constructor},
    {"indexList addEntry", test_indexList_addToList},
    {"indexList getDistanceFromParent", test_indexList_getDistanceFromParent},
    {"indexList getNode", test_indexList_getNode},
    {"indexList getNext", test_indexList_getNext},
    {"build_entry_index", test_build_entry_index},
    {"lookup_entry_index", test_lookup_entry_index},
    { NULL, NULL }
};    