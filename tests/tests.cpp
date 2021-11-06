#include "tests.hpp"
/*
void test_Query(void)
{
    char *q_words = new char[MAX_QUERY_LENGTH];
    memcpy(q_words, "hello\0world\0how\0are\0you\0", MAX_QUERY_LENGTH);
    Query q(q_words, 1);
    TEST_ASSERT((*(q.getText()) == 'h'));
    TEST_ASSERT(strcmp(q.getWord(0),"hello") == 0);
    TEST_ASSERT(strcmp(q.getWord(1),"world") == 0);
    TEST_ASSERT(strcmp(q.getWord(2),"how") == 0);
    TEST_ASSERT(strcmp(q.getWord(3),"are") == 0);

    delete[] q_words;
}

void test_Document(void)
{
    const char* s = "hello world how are you";
    int i = 0;
    while(s[i] != '\0' || s[i+1] != '\0')
    {
        i++;
    }
    char *d_words = new char[strlen(s) + 1]; // 1 -> for the two /0 in the end
    strcpy(d_words, s); 
    Document d(d_words, 1);
    TEST_ASSERT(strcmp(d.getText(),"hello") == 0);
    TEST_ASSERT(strcmp(d.getWord(0),"hello") == 0);
    TEST_ASSERT(strcmp(d.getWord(1),"world") == 0);
    TEST_ASSERT(strcmp(d.getWord(2),"how") == 0);
    TEST_ASSERT(strcmp(d.getWord(3),"are") == 0);

    delete[] d_words;
}
*/
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

// void test_create_hash_table(void) {}

//////////////////////////// Queue ////////////////////////////
void test_enqueue(void) {
    // Setup
    indexNode* nullPTR = NULL;
	Queue testQueue;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
    indexNode* testIndex3 = new indexNode(testEntry3);

    // Try to enqueue NULL
    TEST_EXCEPTION(testQueue.enqueue(NULL), std::exception);
    TEST_EXCEPTION(testQueue.enqueue(&nullPTR), std::exception);
    TEST_ASSERT(testQueue.getSize() == 0);

    // Enqueue one, check size
    testQueue.enqueue(&testIndex1);
    TEST_ASSERT(testQueue.getSize() == 1);

    // Enqueue second, check size
    testQueue.enqueue(&testIndex1);
    TEST_ASSERT(testQueue.getSize() == 2);

    // Enqueue third, check size
    testQueue.enqueue(&testIndex3);
    TEST_ASSERT(testQueue.getSize() == 3);

    // Dequeue all (do not store returned pointer, delete the contents manualy)
    while(testQueue.getSize()>0) {
        testQueue.dequeue();
    }
    delete testIndex1;
    delete testIndex2;
    delete testIndex3;

}

void test_dequeue(void) {
    // Setup
    indexNode* nodeToReturn;
	Queue testQueue;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
    indexNode* testIndex3 = new indexNode(testEntry3);

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
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
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
    indexNode* nodeToReturn;
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    indexNode* testIndex1 = new indexNode(testEntry1);
    indexNode* testIndex2 = new indexNode(testEntry2);
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

//////////////////////////// Index ////////////////////////////
void test_indexNode_construction_addEntry(void){
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORD3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    entry* nullPTR = NULL;
    indexNode* head;

    // Test creating with NULL pointer
    TEST_EXCEPTION(head = new indexNode(nullPTR,MT_EDIT_DIST), std::exception);

    // Test creating with exact match (considered invalid for tree creation)
    TEST_EXCEPTION(head = new indexNode(testEntry1,MT_EXACT_MATCH), std::exception);

    head = new indexNode(testEntry1);
    TEST_EXCEPTION(head->addEntry(nullPTR), std::exception);
    TEST_ASSERT(head->addEntry(testEntry2) == EC_SUCCESS);
    TEST_ASSERT(head->addEntry(testEntry3) == EC_SUCCESS);
    
    delete head;
}

void test_indexNode_getEntry(void) {
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    indexNode* head = new indexNode(testEntry1);
    TEST_ASSERT(head->getEntry() == testEntry1);    
    
    delete head;
}

void test_indexNode_getChildren(void){
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORd3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    indexNode* head;
    treeNodeList* listOfChildren;

    head = new indexNode(testEntry1);
    head->addEntry(testEntry2);
    head->addEntry(testEntry3);

    listOfChildren=head->getChildren();
    TEST_ASSERT(listOfChildren->getNode()->getEntry() == testEntry2);
    TEST_ASSERT(listOfChildren->getDistanceFromParent() == 1);
    TEST_ASSERT(listOfChildren->getNext()->getNode()->getEntry() == testEntry3);
    TEST_ASSERT(listOfChildren->getNext()->getDistanceFromParent() == 2);

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
    delete head;

    // Test by explicit argument EDIT_DIST
    testEntry1 = new entry(testWord1);
    head = new indexNode(testEntry1, MT_EDIT_DIST);
    TEST_ASSERT(head->getMatchingType() == MT_EDIT_DIST);    
    delete head;

    // Test by explicit argument EDIT_DIST
    testEntry1 = new entry(testWord1);
    head = new indexNode(testEntry1, MT_HAMMING_DIST);
    TEST_ASSERT(head->getMatchingType() == MT_HAMMING_DIST);    
    delete head;
}

//////////////////////////// treeNodeList ////////////////////////////

void treeNodeList_constructor(void){
    // Setup
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    entry* nullTestEntry = NULL;
    treeNodeList* testList = NULL;

    // Test treeNodeList Constructor Edge Cases
    TEST_EXCEPTION(testList = new treeNodeList(nullTestEntry, 1, MT_EDIT_DIST), std::exception);
    TEST_EXCEPTION(testList = new treeNodeList(testEntry1, 1,  MT_EXACT_MATCH), std::exception);
    TEST_EXCEPTION(testList = new treeNodeList(testEntry1, 0,  MT_EDIT_DIST), std::exception);
    TEST_EXCEPTION(testList = new treeNodeList(testEntry1, -1,  MT_EDIT_DIST), std::exception);

    testList = new treeNodeList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT (testList != NULL);

    delete testList;
}

void treeNodeList_addToList(void){
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD2";
    word testWord3 = (word)"TESTWORd3";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    entry* testEntry3 = new entry(testWord3);
    entry* nullTestEntry = NULL;
    treeNodeList* testList = NULL;

    testList = new treeNodeList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT(testList->addToList(testEntry3, 2) == 0);
    TEST_ASSERT(testList->addToList(testEntry2, 1) == 0);
    TEST_EXCEPTION(testList->addToList(testEntry3, 0), std::exception);
    TEST_EXCEPTION(testList->addToList(testEntry3, -1), std::exception);
    TEST_EXCEPTION(testList->addToList(nullTestEntry, 3), std::exception);

    delete testList;
}

void treeNodeList_getDistanceFromParent(void){
    // Setup
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    treeNodeList* testList = NULL;

    testList = new treeNodeList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT (testList->getDistanceFromParent() == 1);

    delete testList;
}

void treeNodeList_getNode(void){
    // Setup
    word testWord1 = (word)"TESTWORD1";
    entry* testEntry1 = new entry(testWord1);
    treeNodeList* testList = NULL;

    testList = new treeNodeList(testEntry1, 1,  MT_EDIT_DIST);
    TEST_ASSERT (testList->getNode()->getEntry() == testEntry1);

    delete testList;
}

void treeNodeList_getNext(void){
    // Setup
    word testWord1 = (word)"TESTWORD1";
    word testWord2 = (word)"TESTWORD22";
    entry* testEntry1 = new entry(testWord1);
    entry* testEntry2 = new entry(testWord2);
    treeNodeList* testList = NULL;

    testList = new treeNodeList(testEntry1, 1,  MT_EDIT_DIST);
    testList->addToList(testEntry2, 2);
    TEST_ASSERT (testList->getNext()->getNode()->getEntry() == testEntry2);

    delete testList;
}

//////////////////////////// indexInterface ////////////////////////////
void test_build_entry_index(void){
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
    
    TEST_EXCEPTION(build_entry_index(nullList, MT_EDIT_DIST ,&tree), std::exception);
    TEST_ASSERT(build_entry_index(test_list, MT_EXACT_MATCH ,&tree) == EC_FAIL);
    TEST_ASSERT(build_entry_index(test_list, MT_EDIT_DIST ,&tree) == EC_SUCCESS);

    destroy_entry_index(tree);
    delete test_list;
}


void test_lookup_entry_index(void){
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
    entry* resultNode = NULL;
    entry_list* result = new entry_list();;
    entry_list* test_list = new entry_list();
    test_list->addEntry(testEntry6);
    test_list->addEntry(testEntry5);
    test_list->addEntry(testEntry4);
    test_list->addEntry(testEntry3);
    test_list->addEntry(testEntry2);
    test_list->addEntry(testEntry1);
    indexNode* tree = NULL;
    indexNode* nullTree = NULL;
    const word text = (char*)"TESTWOR22\0";
    build_entry_index(test_list, MT_EDIT_DIST ,&tree);

    TEST_EXCEPTION(lookup_entry_index(&text,tree, -1, result), std::exception);
    TEST_EXCEPTION(lookup_entry_index(&text,nullTree, 0, result), std::exception);
    TEST_EXCEPTION(lookup_entry_index(&text,tree, -1, result), std::exception);
    TEST_ASSERT(lookup_entry_index(&text,tree, 0, result) == EC_SUCCESS);

    TEST_ASSERT(lookup_entry_index(&text,tree, 0, result) == EC_SUCCESS);
    resultNode=result->getHead();
    TEST_ASSERT(strcmp(resultNode->getWord(),testEntry3->getWord())==0);

    resultNode=result->getHead();
    while(resultNode){
        entry* tempResult = NULL;
        tempResult=resultNode->getNext();
        delete resultNode;
        resultNode = tempResult;
    }
    delete result;


    destroy_entry_index(tree);
    delete test_list;
    
}


TEST_LIST = {
    //{"test_Query", test_Query} ,
    //{"test_Document", test_Document} ,
    {"Test Exact Match", test_exact_match},
    {"Test hamming", test_hamming},
    {"Test Edit Distance", test_edit},
    {"Test Enqueue", test_enqueue},
    {"Test Dequeue", test_dequeue},
    {"Test childQueueNode Creation", test_childQueueNode_create},
    {"Test childQueueNode Pop", test_childQueueNode_pop},
    {"Test indexNode Creation", test_indexNode_construction_addEntry},
    {"Test indexNode getEntry", test_indexNode_getEntry},
    {"Test indexNode getChildren", test_indexNode_getChildren},
    {"Test indexNode getMatchingType", test_indexNode_getMatchingType},
    {"Test treeNodeList Constructor", treeNodeList_constructor},
    {"Test treeNodeList addEntry", treeNodeList_addToList},
    {"Test treeNodeList getDistanceFromParent", treeNodeList_getDistanceFromParent},
    {"Test treeNodeList getNode", treeNodeList_getNode},
    {"Test treeNodeList getNext", treeNodeList_getNext},
    {"Test build_entry_index", test_build_entry_index},
    {"Test build_entry_index", test_lookup_entry_index},

    
    { NULL, NULL }
};