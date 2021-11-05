#include "tests.hpp"

// Match Functions
void test_exact_match(void)
{   
    word word1=(word)"hello";
    word word2=(word)"hello";
    word word3=(word)"hellno";
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

// Queue
void test_enqueue(void){
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

    while(testQueue.getSize()>0){
        testQueue.dequeue();
    }
    delete testIndex1;
    delete testIndex2;
    delete testIndex3;

}

void test_dequeue(void){
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

void test_childQueueNode_create(void){
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

void test_childQueueNode_pop(void){
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
    {"Test Mxact Match", test_exact_match},
    {"Test hamming", test_hamming},
    {"Test Edit Distance", test_edit},
    {"Test Enqueue", test_enqueue},
    {"Test Dequeue", test_dequeue},
    {"Test childQueueNode", test_childQueueNode_create},
    {"Test childQueueNode", test_childQueueNode_pop},
    //{ "test_create_hash_table", test_create_hash_table },
    { NULL, NULL }
};