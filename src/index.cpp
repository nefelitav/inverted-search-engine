#include "../include/index.hpp"
#include "../include/functions.hpp"

indexing :: indexing(entry* input, MatchType matchingMetric) {
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    if (matchingMetric != MT_HAMMING_DIST && matchingMetric != MT_EDIT_DIST ){
        throw std::invalid_argument( "Invalid Distance Metric");
    }
    //std::cout<<"Creating new node\n";
    this->MatchingType = matchingMetric;
    this->content = input;
    this->children = NULL;
}
indexing :: ~indexing() {
    delete this->content;
    delete this->children;
}

MatchType indexing :: getMatchingType() {
    return this->MatchingType;
}

ErrorCode indexing :: addEntry(entry* input) {
    int distance;
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    // Get distance for given metric
    if (this->getMatchingType() == MT_HAMMING_DIST) {
        //cout<<"Selected HAMMING\n";
        distance = hammingDistance(this->content->getWord(), input->getWord());
    } else if(this->getMatchingType() == MT_EDIT_DIST) {
        //cout<<"Selected EDIT\n";
        distance = editDistance(this->content->getWord(), input->getWord());
    }else {
        return EC_FAIL;
    }

    if (this->children == NULL) {           //If there are no children
        this->children = new treeNodeList(input, distance,this->getMatchingType());
    }else if (this->children->getDistanceFromParent() > distance) {      // Children list exists, first child has bigger dist
        treeNodeList* oldFirstChild = this->children;
        this->children = new treeNodeList(input, distance, this->getMatchingType(), oldFirstChild);
    }else {          // Children list exists, new entry has equal or greater dist than first child
        this->children->addToList(input, distance);
    }
    return EC_SUCCESS;
}

int indexing :: printTree(int depth) {
    std::cout << "Word: " << this->content->getWord() << "\n";
    if (this->children ) {
        this->children->printList(depth);
    }
    return 0;
}
entry* indexing :: getEntry() {
    return this->content;
}

treeNodeList* indexing :: getChildren() {
    return this->children;
}

////////////////////////////////////////////////////////////////////////////////////

ErrorCode build_entry_index(const entry_list* el, MatchType type, indexing** ix) {
    if(el == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    try {
        entry* currEntry = el->getHead();
        *ix = new indexing(currEntry, type);
        while (currEntry->getNext()) {
            (*ix)->addEntry(currEntry->getNext());
            currEntry = currEntry->getNext();
        }
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
}


ErrorCode lookup_entry_index(const word* w, indexing* ix, int threshold, entry_list* result) {
    int distance;
    if(w == NULL || *w == NULL || ix == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    if(threshold < 0){
        throw std::invalid_argument( "Invalid Threshold");
    }
    try {
        // Store the children of current node, if any
        treeNodeList* currChild;

        // New result to be stored on the result list
        entry* tempEntry;

        // Nodes to be examined next
        Queue* queue = new Queue();
        indexing* toEnqueue;

        // Start the search with the root of the tree
        indexing* currNode = ix;

        // Main loop
        while (currNode) {
            // Calculate distance from target word
            if (currNode->getMatchingType() == MT_HAMMING_DIST) {
                distance = hammingDistance(currNode->getEntry()->getWord(), *w);
            } else if (currNode->getMatchingType() == MT_EDIT_DIST) {
                distance = editDistance(currNode->getEntry()->getWord(), *w);
            } else {
                return EC_FAIL;
            }
            
            // Add to results if close enough
            if  (distance <= threshold) {
                tempEntry = new entry (currNode->getEntry()->getWord());
                result->addEntry (tempEntry);
            }

            // Add any applicable children to examine later
            currChild = currNode->getChildren();
            while (currChild) {
                if (currChild->getDistanceFromParent() >= distance-threshold && currChild->getDistanceFromParent() <= distance+threshold) {
                    toEnqueue = currChild->getNode();
                    queue->enqueue(&toEnqueue);
                } else if (currChild->getDistanceFromParent() > distance + threshold){
                    // Children are ordered by distance, if one in not acceptable we can skip the rest
                    break;
                }
                currChild = currChild->getNext();
            }

            // Get the next item to be examined
            currNode = queue->dequeue();
            
        }
        delete queue;
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
}

ErrorCode destroy_entry_index(indexing* ix) {
    try {
        delete ix;
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
}

////////////////////////////////////////////////////////////////////////////////////


childQueueNode :: childQueueNode(indexing* input, childQueueNode* oldHead) {
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }else{
        this->entry = input;
        this->next = oldHead;
    }
}
void childQueueNode :: pop(indexing** content, childQueueNode** newHead) {
    *newHead = this->next;
    *content = this->entry;
    delete this;
}

childQueueNode* childQueueNode:: getNext(){
    return this->next;
}


Queue :: Queue() {
    this->head = NULL;
}

void Queue :: enqueue(indexing** input) {
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }else if (*input == NULL){
        throw std::invalid_argument( "Got pointer to NULL pointer");
    }else{
        this->head = new childQueueNode(*input, this->head);
    }
    
}
indexing*  Queue :: dequeue() {
    indexing* nodeToReturn;
    if (this->head) {
        this->head->pop(&nodeToReturn, &this->head);
    } else {
        nodeToReturn = NULL;
    }
    return nodeToReturn;
}

int Queue :: getSize(){
    int count = 0;
    childQueueNode* currNode = this->head;
    while (currNode){
        count++;
        currNode = currNode->getNext();
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////

treeNodeList :: treeNodeList(entry* content, int distance, MatchType matchingMetric, treeNodeList* next) {
    if (content == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    if (matchingMetric != MT_HAMMING_DIST && matchingMetric != MT_EDIT_DIST ){
        throw std::invalid_argument( "Invalid Distance Metric");
    }
    if (distance <= 0){
        throw std::invalid_argument( "Invalid Distance Value");
    }
    // Point to the next list node and set distance from parrent for this node
    this->next = next;
    this->distanceFromParent = distance;
    // Create the index node with the given content
    this->node = new indexing(content, matchingMetric);
}

treeNodeList :: ~treeNodeList() {
    if (this->next) {
        delete this->next;
    }
    delete this->node;
}

int treeNodeList :: getDistanceFromParent() const{
    return this->distanceFromParent;
}

int treeNodeList :: addToList(entry* content,int distance) {
    if (content == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    if (distance <= 0){
        throw std::invalid_argument( "Invalid Distance Value");
    }

    treeNodeList* next = this->next;
    if (distance == this->distanceFromParent) {                 // If we have the same distance as this node, push lower in the tree
        this->node->addEntry(content);
    } else if (this->next) {                                    // If we are not in the last node
        if (distance > this->next->getDistanceFromParent()) {   // If the next node has distance lower than the input, pass input along
            this->next->addToList(content, distance);
        }else {                                                 // Else if the next node has higher distance, create a new list node between these
            this->next = new treeNodeList(content, distance, this->node->getMatchingType(), next);
        }
    } else {                                                     // If there is no next node, create a new node after this
        this->next = new treeNodeList(content, distance, this->node->getMatchingType(), next);
    }
    return 0;
}

void treeNodeList::printList(int depth) {
    for (int i = 0; i <= depth*3 + 1; i++ ){
        std::cout << " ";
    }
    std::cout << " |->Diff: " << this->distanceFromParent << " ";
    this->node->printTree( depth + 1);
    if (this->next ) {
        this->next->printList(depth);
    }
}

indexing* treeNodeList :: getNode() const{
    return this->node;
}

treeNodeList* treeNodeList :: getNext() const {
    return this->next;
}

