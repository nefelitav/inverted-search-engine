#include <iostream>
#include "match.hpp"
#include "index.hpp"
#include "structs.hpp"


using namespace std;

index::index(entry* input, MatchType matchingMetric){
    cout<<"Creating new node\n";
    this->indexMatchingType=matchingMetric;
    this->content = input;
    this->children = NULL;
}
index::~index(){
    delete this->content;
    delete this->children;
}

MatchType index::getMatchingType(){
    return this->indexMatchingType;
}

int index::addEntry(entry* input){
    int distance;

    // Get distance for given metric
    if (this->getMatchingType() == MT_HAMMING_DIST){
        distance = hammingDistance(this->content->getWord(), input->getWord());
    }else if(this->getMatchingType() == MT_EDIT_DIST){
        distance = editDistance(this->content->getWord(), input->getWord());
    }

    if (this->children == NULL){
        //If there are no children
        this->children = new treeNodeList(input, distance,this->getMatchingType());
    }else if (this->children->getDistanceFromParent() > distance){
        // Children list exists, first child has bigger dist
        treeNodeList* oldFirstChild = this->children;
        this->children = new treeNodeList(input, distance, this->getMatchingType(), oldFirstChild);
    }else{
        // Children list exists, new entry has equal or greater dist than first child          
        this->children->addToList(input, distance);
    }
    return 0;
}

int index::printTree(){
    cout<<"Word: "<<this->content->getWord();
    if (this->children ){
        cout<<" HAS CHILDREN: \n";
        this->children->printList();
        cout<<"Children over for: "<<this->content->getWord();
    }
    cout<<"\n";
    return 0;
}
class entry* index::getEntry(){
    return this->content;
}

class treeNodeList* index::getChildren(){
    return this->children;
}


ErrorCode lookup_entry_index(const word* w,class index* ix, int threshold, entry_list** result){
    int distance;

    try {
        // List to store results
        *result = new entry_list();

        // Store the children of current node, if any
        class treeNodeList* currChild;

        // New result to be stored on the result list
        entry* tempEntry;

        // Nodes to be examined next
        queueHandler* queue = new queueHandler();
        class index* toEnqueue;

        // Start the search with the root of the tree
        class index* currNode = ix;

        // Main loop
        while (currNode){

            // Calculate distance from target word
            if (currNode->getMatchingType() == MT_HAMMING_DIST){
                distance = hammingDistance(currNode->getEntry()->getWord(), *w);
            }else if(currNode->getMatchingType() == MT_EDIT_DIST){
                distance = editDistance(currNode->getEntry()->getWord(), *w);
            }

            // Add to results if within threshold
            if  (distance <= threshold){
                tempEntry = new entry (currNode->getEntry()->getWord());
                (*result)->addEntry (tempEntry);
            }

            // Add any applicable chilren to examine later
            currChild = currNode->getChildren();
            while (currChild){
                if (currChild->getDistanceFromParent() >= distance-threshold && currChild->getDistanceFromParent() <= distance+threshold){
                    toEnqueue = currChild->getNode();
                    queue->enqueue(&toEnqueue);
                }else{
                    // Children are ordered by distance, if one in not acceptable we can skip the rest
                    break;
                }
                currChild = currChild->getNext();
            }

            // Get the next item to be examined
            queue->dequeue(&currNode);
            
        }
        delete queue;
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
}

ErrorCode buildEntryIndex(entry_list* el, MatchType type,class index** ix){
    try{
        entry* currEntry = el->getHead();
        *ix = new class index(currEntry, type);
        while (currEntry->getNext() ){
        (*ix)->addEntry(currEntry->getNext());
        currEntry = currEntry->getNext();
        }
        return EC_SUCCESS;
    }catch (const exception& _) {
        return EC_FAIL;
    }
}

ErrorCode destroy_entry_index(class index* ix){
    try{
        delete ix;
        return EC_SUCCESS;
    }catch (const exception& _) {
        return EC_FAIL;
    }
}