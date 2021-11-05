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
        indexing(entry* input, MatchType matchingMetric = MT_EDIT_DIST);
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

class childQueueNode {
    private:
        indexing* entry;
        childQueueNode* next;
    public:
        // Create a new queueNode, given input and (optionaly, if this is not the first node) the previous head to set as next
        childQueueNode(indexing* input, childQueueNode* next = nullptr);
        // Return content of this node, and the head to it's next node, to be the new head
        void pop(indexing** content, childQueueNode** newHead);
        childQueueNode* getNext();
};

class Queue {
    private:
        childQueueNode* head;
    public:
        Queue();
        void enqueue(indexing** input);
        indexing* dequeue();
        int getSize();
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