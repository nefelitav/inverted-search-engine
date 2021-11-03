#ifndef INDEX
#define INDEX

#include <iostream>
#include "core.h"
#include "structs.hpp"

class treeNodeList;

class indexing {
    private:
        MatchType MatchingType;
        entry* content;
        treeNodeList* children;
    public:
        indexing(entry* input, MatchType matchingMetric);
        int addEntry(entry* input);
        int printTree(int depth = 0);
        class entry* getEntry();
        class treeNodeList* getChildren();
        MatchType getMatchingType();
        ~indexing();
};

////////////////////////////////////////////////////////////////////////////////////

ErrorCode build_entry_index(const entry_list* el, MatchType type, indexing** ix);
ErrorCode lookup_entry_index(const word* w, indexing* ix, int threshold, entry_list* result);
ErrorCode destroy_entry_index(indexing* ix);

////////////////////////////////////////////////////////////////////////////////////

class childQueue {
    private:
        indexing* entry;
        childQueue* next;
    public:
        childQueue(indexing* input, childQueue* next = nullptr);
        void pop(indexing** content, childQueue** newHead);
};

class Queue {
    private:
        childQueue* head;
    public:
        Queue();
        int enqueue(indexing** input);
        int dequeue(indexing** nodeToReturn);
        childQueue* get_head() const;
};

////////////////////////////////////////////////////////////////////////////////////

class treeNodeList {
    private:
        int distanceFromParent;
        indexing* node;
        treeNodeList* next;
    public:
        treeNodeList(entry* content, int distance, MatchType matchingMetric, treeNodeList* next = nullptr);
        void addToList(entry* content, int distance);
        int getDistanceFromParent();
        void printList(int depth = 0);
        indexing* getNode();
        treeNodeList* getNext();
        ~treeNodeList();
};

#endif  //INDEX