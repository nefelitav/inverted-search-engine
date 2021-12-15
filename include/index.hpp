#ifndef INDEX
#define INDEX

#include <iostream>
#include "core.h"
#include "structs.hpp"

class entry;
typedef char *word;
class entry_list;
class indexList;

class indexNode {
    private:
        MatchType MatchingType;
        entry* content;
        indexList* children;
    public:
        indexNode(entry* input, QueryID id = 1, unsigned int threshold = 0, MatchType matchingMetric = MT_EDIT_DIST);
        ErrorCode addEntry(entry* input, QueryID id, unsigned int threshold);
        int printTree(int depth = 0);
        entry* getEntry();
        indexList* getChildren();
        MatchType getMatchingType();
        ~indexNode();
};

class indexList {
    private:
        unsigned int distanceFromParent;
        indexNode* node;
        indexList* next;
    public:
        indexList(entry* content, unsigned int distance, MatchType matchingMetric, QueryID id, unsigned int threshold, indexList* next = nullptr);
        int addToList(entry* content, unsigned int distance, QueryID id, unsigned int threshold);    // Insert new entry in the right place in the list
        unsigned int getDistanceFromParent() const;
        void printList(int depth = 0);
        indexNode* getNode() const ;
        indexList* getNext() const;
        ~indexList();
};

//ErrorCode build_entry_index(const entry_list* el, MatchType type, indexNode** ix);
ErrorCode lookup_entry_index(const word w, entry_list *result, MatchType queryMatchingType);
ErrorCode destroy_entry_index(indexNode* ix);
ErrorCode InitializeIndex();
ErrorCode addToIndex(entry* toAdd, QueryID queryId, MatchType queryMatchingType, unsigned int threshold);
ErrorCode removeFromIndex(const word givenWord, const QueryID queryId, const MatchType givenType);
void printHammingTrees();

class stackNode
{
    private:
        indexNode *entry;
        stackNode *next;

    public:
        // Create a new StackNode, given input and (optionally, if this is not the first node) the previous head to set as next
        stackNode(indexNode *input, stackNode *next = nullptr);
        // Return content of this node, and the head to its next node, to be the new head
        void pop(indexNode **content, stackNode **newHead);
        stackNode *getNext();
};

class Stack
{
    private:
        stackNode *head;
    public:
        Stack();
        void add(indexNode **input);
        indexNode *pop();
        int getSize();
};


// Global Vars for Index Initialization
extern indexNode *editIndex;
extern indexNode **hammingIndexes;
extern void *exactHash;

#endif  //INDEX
