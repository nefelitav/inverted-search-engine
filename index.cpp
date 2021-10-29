#include "core.h"
#include <iostream>
#include "structs.hpp"
#include "match.hpp"
#include "index.hpp"
#include "treeNodeList.hpp"

using namespace std;

index::index(entry* input, MatchType matchingMetric) {
    //cout<<"Creating new node\n";
    this->indexMatchingType=matchingMetric;
    this->content = input;
    this->children = NULL;
}
index::~index() {
    delete this->content;
    delete this->children;
}

ErrorCode buildEntryIndex(const entry_list* el, MatchType type,class index* ix) {
    try {
        entry* currEntry = el->getHead();
        ix = new class index(currEntry, type);
        while (currEntry->getNext() ){
            ix->addEntry(currEntry->getNext());
            currEntry = currEntry->getNext();
        }
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry_index(class index* ix) {
    try {
        delete ix;
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

MatchType index::getMatchingType() {
    return this->indexMatchingType;
}

int index::addEntry(entry* input) {
    int distance;
    // Get distance for given metric
    if (this->getMatchingType() == MT_HAMMING_DIST) {
        //cout<<"Selected HAMMING\n";
        distance = hammingDistance(this->content->getWord(), input->getWord());
    }else if(this->getMatchingType() == MT_EDIT_DIST) {
        //cout<<"Selected EDIT\n";
        distance = editDistance(this->content->getWord(), input->getWord());
    }

    //cout<<"Distance : "<<distance<<'\n';
    if (this->children == NULL) {            //If there are no children
        //cout<<"No children list found, making new list\n";
        this->children = new treeNodeList(input, distance,this->getMatchingType());
    }else if (this->children->getDistanceFromParent() > distance) {      // Children list exists, first child has bigger dist
        //cout<<"Putting in front\n";
        treeNodeList* oldFirstChild = this->children;
        this->children = new treeNodeList(input, distance, this->getMatchingType(), oldFirstChild);
    }else {          // Children list exists, new entry has equal or greater dist than first child
        //cout<<"Sending in to list\n";
        this->children->addToList(input, distance);
    }
    return 0;
}

int index::printTree() {
    //cout<<"Word: "<<this->content->getWord();
    if (this->children ) {
        //cout<<" HAS CHILDREN: \n";
        this->children->printList();
        //cout<<"Children over for: "<<this->content->getWord();
    }
    //cout<<"\n";
    return 0;
}

int index::search(char* word,int threshold) {
    int distance;
    // Get distance for selected Metric
    if (this->getMatchingType() == MT_HAMMING_DIST) {
        //cout<<"Selected HAMMING\n";
        distance = hammingDistance(this->content->getWord(), word);
    }else if(this->getMatchingType() == MT_EDIT_DIST) {
        //cout<<"Selected EDIT\n";
        distance = editDistance(this->content->getWord(), word);
    }
    if  (distance < threshold) {
        //cout<<this->content->getWord();
    }
    if (this->children) {
       // Func that checks distance from child list, stops at the first one that meets criteria 
    }
    return 0;
}