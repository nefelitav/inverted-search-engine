#ifndef INDEX
#define INDEX

#include <iostream>
#include "core.h"
#include "structs.hpp"
class entry;
typedef char* word;
class entry_list;
class indexList;

class indexNode {
    private:
        MatchType MatchingType;
        entry** content;
        indexList* children;
    public:
        indexNode(entry** input, int givenId = 1, int givenThreshold = 0, MatchType matchingMetric = MT_EDIT_DIST);
        ErrorCode addEntry(entry** input, int givenId, int givenThreshold);
        int printTree(int depth = 0);
        entry** getEntry();
        indexList* getChildren();
        MatchType getMatchingType();
        ~indexNode();
};

//ErrorCode build_entry_index(const entry_list* el, MatchType type, indexNode** ix);
ErrorCode lookup_entry_index(const word* w, indexNode* ix, int threshold, entry_list* result);
ErrorCode destroy_entry_index(indexNode* ix);
ErrorCode InitializeIndex();
ErrorCode addToIndex(entry** toAdd, int queryId, MatchType queryMatchingType, int threshold);
ErrorCode removeFromIndex(indexNode* ix, const word* givenWord, int queryId, MatchType givenType);
////////////////////////////////////////////////////////////////////////////////////

class stackNode {
    private:
        indexNode* entry;
        stackNode* next;
    public:
        // Create a new StackNode, given input and (optionaly, if this is not the first node) the previous head to set as next
        stackNode(indexNode* input, stackNode* next = nullptr);
        // Return content of this node, and the head to it's next node, to be the new head
        void pop(indexNode** content, stackNode** newHead);
        stackNode* getNext();
};

class Stack {
    private:
        stackNode* head;
    public:
        Stack();
        void add(indexNode** input);
        indexNode* pop();
        int getSize();
};

////////////////////////////////////////////////////////////////////////////////////

class indexList {
    private:
        int distanceFromParent;
        indexNode* node;
        indexList* next;
    public:
        indexList(entry** content, int distance, MatchType matchingMetric, int givenId, int givenThreshold, indexList* next = nullptr);
        int addToList(entry** content, int distance, int givenId, int givenThreshold);    // Insert new entry in the right place in the list
        int getDistanceFromParent() const;
        void printList(int depth = 0);
        indexNode* getNode() const ;
        indexList* getNext() const;
        ~indexList();
};
////////////////////////////////////////////////////////////////////////////////////

class payloadNode{
    private:
        int id;
        int threshold;
        payloadNode* next;
    public:
        payloadNode(int givenId, int givenThreshold, payloadNode* givenNext = NULL);
        int getId();
        int getThreshold();
        payloadNode* getNext();
        void setNext(payloadNode* newNext);
        void addNode(int givenId, int givenThreshold);
};

class payloadList{
    private:
        payloadNode* first;
    public:
        payloadList();
        ~payloadList();
        void insertNode(int givenId, int givenThreshold);
        void deleteNode(int id);
        bool isEmpty();
        payloadNode* getFirst();

};

////////////////////////////////////////////////////////////////////////////////////

//Global Var for Index Initialization
extern indexNode* editIndex;
extern indexNode** hammingIndexes;
extern void* exactHash;

#endif  //INDEX