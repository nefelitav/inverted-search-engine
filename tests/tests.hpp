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
void test_binary_search(void);
void test_hash_table(void);
void test_hash_function(void);
void test_deduplication(void);
void test_exact_match(void);
void test_hamming(void);
void test_edit(void);
void test_add(void);
void test_pop(void);
void test_stackNode_create(void);
void test_stackNode_pop(void);
void test_indexNode_construction_addEntry(void);
void test_indexNode_getEntry(void);
void test_indexNode_getChildren(void);
void test_indexNode_getMatchingType(void);
void test_indexList_constructor(void);
void test_indexList_addToList(void);
void test_indexList_getDistanceFromParent(void);
void test_indexList_getNode(void);
void test_indexList_getNext(void);
void test_build_entry_index(void);
void test_lookup_entry_index(void);

#endif  //TESTS
