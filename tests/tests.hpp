#ifndef TESTS
#define TESTS

#include "../include/acutest.h"
#include "../include/core.h"
#include "../include/utilities.hpp"
#include "../include/structs.hpp"
#include "../include/index.hpp"

    void test_query(void);
    void test_document(void);
    void test_entry(void);
    void test_entry_set_get_payload(void);
    void test_entry_emptyPayload(void);
    void test_entrylist(void);
    
    void test_binary_search(void);
    void test_query_binary_search(void);
    void test_entry_binary_search(void);
    void test_result_binary_search(void);
    void test_find_query(void);
    void test_find_entry(void);

    void test_hash_function(void);
    void test_deduplication(void);

    void test_start_query(void);

    void test_doc_table(void);
    void test_query_table(void);
    void test_entry_table(void);

    void test_exact_match(void);
    void test_hamming(void);
    void test_edit(void);
    void test_stack_add(void);
    void test__stack_pop(void);
    void test_stackNode_create(void);
    void test_stackNode_pop(void);
    void test_indexNode_constructor_addEntry(void);
    void test_indexNode_getEntry(void);
    void test_indexNode_getChildren(void);
    void test_indexNode_getMatchingType(void);
    void test_indexList_constructor(void);
    void test_indexList_addToList(void);
    void test_indexList_getDistanceFromParent(void);
    void test_indexList_getNode(void);
    void test_indexList_getNext(void);
    void test_lookup_entry_index(void);
    void test_removeFromIndex(void);
    void test_addToIndex(void);
    void test_payloadNode_constructor_getters(void);
    void test_payloadNode_setNext(void);
    void test_payload_addToPayload(void);
    void test_payload_deletePayloadNode(void);
    void test_result_storeResult(void);
    void test_GetNextAvailRes(void);

#endif  //TESTS
