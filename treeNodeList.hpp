#ifndef TREE_NODE_LIST
#define TREE_NODE_LIST

#include "structs.hpp"
#include "index.hpp"
#include "core.h"

class treeNodeList{
    private:
        int distanceFromParrent;
        class index* node;
        treeNodeList* next;
    public:
        treeNodeList(entry* content,int distance, MatchType matchingMetric, treeNodeList* ptrToNextNode = nullptr);
        ~treeNodeList();
        int addToList(entry* content,int distance);
        int getDistanceFromParent();
        int printList();
        class index* getNode();
        treeNodeList* getNext();
};

#endif //TREE_NODE_LIST
