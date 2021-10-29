#include <iostream>
#include "structs.hpp"
#include "treeNodeList.hpp"
#include "index.hpp"

using namespace std;

treeNodeList::treeNodeList(entry* content, int distance, MatchType matchingMetric, treeNodeList* ptrToNextNode) {

    // Point to the next list node and set distance from parrent for this node
    this->next = ptrToNextNode;
    this->distanceFromParrent = distance;
    // Create the index node with the given content
    this->node = new class index(content, matchingMetric);
}

treeNodeList::~treeNodeList() {
    if (this->next ) {
        delete this->next;
    }
    delete this->node;
}

int treeNodeList::getDistanceFromParent() {
    return this->distanceFromParrent;
}

int treeNodeList::addToList(entry* content,int distance) {
    treeNodeList* ptrToNextNode = this->next;
    if (distance == this->distanceFromParrent) { // If we have the same distance as this node, push lower in the tree
        this->node->addEntry(content);
    }else if (this->next ) {  // If we are not the last node:
        if (distance > this->next->getDistanceFromParent()) {   // If the next node has distance lower than the input, pass input allong
            this->next->addToList(content, distance);
        }else {  // Else if the next node has higher distance, create a new list node between us
            this->next = new treeNodeList(content, distance, this->node->getMatchingType() ,ptrToNextNode);
        }
    }else {  // If there is no next node, create a new node after me
        this->next = new treeNodeList(content, distance, this->node->getMatchingType(), ptrToNextNode);
    }
    return 0;
}

int treeNodeList::printList() {
    //cout<<"Difference from parent: "<<this->distanceFromParrent<<" ";
    this->node->printTree();
    if (this->next ) {
        this->next->printList();
    }
    return 0;
}


