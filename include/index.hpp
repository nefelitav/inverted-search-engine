#ifndef INDEX
#define INDEX

#include <iostream>
#include "core.h"
#include "structs.hpp"

class treeNodeList;

class indexNode {
    private:
        MatchType MatchingType;
        entry* content;
        treeNodeList* children;
    public:
        indexNode(entry* input, MatchType matchingMetric = MT_EDIT_DIST);
        ErrorCode addEntry(entry* input);
        int printTree(int depth = 0);
        entry* getEntry();
        treeNodeList* getChildren();
        MatchType getMatchingType();
        ~indexNode();
};

ErrorCode build_entry_index(const entry_list* el, MatchType type, indexNode** ix);
ErrorCode lookup_entry_index(const word* w, indexNode* ix, int threshold, entry_list* result);
ErrorCode destroy_entry_index(indexNode* ix);

class childQueueNode {
    private:
        indexNode* entry;
        childQueueNode* next;
    public:
        // Create a new queueNode, given input and (optionaly, if this is not the first node) the previous head to set as next
        childQueueNode(indexNode* input, childQueueNode* next = nullptr);
        // Return content of this node, and the head to it's next node, to be the new head
        void pop(indexNode** content, childQueueNode** newHead);
        childQueueNode* getNext();
};

class Queue {
    private:
        childQueueNode* head;
    public:
        Queue();
        void enqueue(indexNode** input);
        indexNode* dequeue();
        int getSize();
};

class treeNodeList {
    private:
        int distanceFromParent;
        indexNode* node;
        treeNodeList* next;
    public:
        treeNodeList(entry* content, int distance, MatchType matchingMetric, treeNodeList* next = nullptr);
        int addToList(entry* content, int distance);
        int getDistanceFromParent() const;
        void printList(int depth = 0);
        indexNode* getNode() const ;
        treeNodeList* getNext() const;
        ~treeNodeList();
};

#endif  //INDEX