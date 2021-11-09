#include "../include/index.hpp"
#include "../include/utilities.hpp"

indexNode :: indexNode(entry* input, MatchType matchingMetric) {
    // Check Input
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    if (matchingMetric != MT_HAMMING_DIST && matchingMetric != MT_EDIT_DIST ){
        throw std::invalid_argument( "Invalid Distance Metric");
    }
    this->MatchingType = matchingMetric;
    this->content = input;
    this->children = NULL;
}
indexNode :: ~indexNode() {
    delete this->children;
}

MatchType indexNode :: getMatchingType() {
    return this->MatchingType;
}

ErrorCode indexNode :: addEntry(entry* input) {
    int distance;
    // Check Input
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    // Get distance for given metric
    if (this->getMatchingType() == MT_HAMMING_DIST) {
        distance = hammingDistance(this->content->getWord(), input->getWord());
    } else if(this->getMatchingType() == MT_EDIT_DIST) {
        distance = editDistance(this->content->getWord(), input->getWord());
    }else {
        return EC_FAIL;
    }

    // If the entry already exists in this node raise exception, this should not happen
    if (distance == 0){
        throw std::runtime_error( "Word already in the tree");
        return EC_FAIL;
    }

    if (this->children == NULL) {           //If there are no children create new list with the input in the first node
        this->children = new indexList(input, distance,this->getMatchingType());
    }else if (this->children->getDistanceFromParent() > distance) {      // Children list exists, first child has bigger dist
        indexList* oldFirstChild = this->children;
        this->children = new indexList(input, distance, this->getMatchingType(), oldFirstChild);
    }else {          // Children list exists, new entry has equal or greater dist than first child
        try{         // Give the entry to the list to handle
            this->children->addToList(input, distance);
        }catch (const std::exception& _) {
            throw std::runtime_error( "Word already in the tree");
            return EC_FAIL;
        }
    }
    return EC_SUCCESS;
}

int indexNode :: printTree(int depth) {
    std::cout << "Word: " << this->content->getWord() << "\n";
    if (this->children ) {
        this->children->printList(depth);
    }
    return 0;
}
entry* indexNode :: getEntry() {
    return this->content;
}

indexList* indexNode :: getChildren() {
    return this->children;
}

////////////////////////////////////////////////////////////////////////////////////

ErrorCode build_entry_index(const entry_list* el, MatchType type, indexNode** ix) {
    if(el == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    try {
        entry* currEntry = el->getHead();   //Get the first entry
        *ix = new indexNode(currEntry, type);
        while (currEntry->getNext()) {          // Enter every entry after that
            try {
                (*ix)->addEntry(currEntry->getNext());
            }catch (const std::exception& _) {
                std::cout<<"Word already in the tree\n";
            }
            currEntry = currEntry->getNext();
        }
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
}


ErrorCode lookup_entry_index(const word* w, indexNode* ix, int threshold, entry_list* result) {
    int distance;
    // Check input
    if(w == NULL || *w == NULL || ix == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    if(threshold < 0){
        throw std::invalid_argument( "Invalid Threshold");
    }
    try {
        // Store the children of current node, if any
        indexList* currChild;

        // New result to be stored on the result list
        entry* tempEntry;

        // Nodes to be examined next
        Stack* stack = new Stack();
        indexNode* toadd;

        // Start the search with the root of the tree
        indexNode* currNode = ix;

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
                // If the child meets the criteria, add it to the stack
                if (currChild->getDistanceFromParent() >= distance-threshold && currChild->getDistanceFromParent() <= distance+threshold) {
                    toadd = currChild->getNode();
                    stack->add(&toadd);
                } else if (currChild->getDistanceFromParent() > distance + threshold){
                    // Children are ordered by distance, if one in not acceptable we can skip the rest
                    break;
                }
                currChild = currChild->getNext();
            }

            // Get the next item to be examined
            currNode = stack->pop();
            
        }
        delete stack;
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
}

ErrorCode destroy_entry_index(indexNode* ix) {
    try {
        delete ix;
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
}

////////////////////////////////////////////////////////////////////////////////////


stackNode :: stackNode(indexNode* input, stackNode* oldHead) {
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }else{
        this->entry = input;
        this->next = oldHead;
    }
}
void stackNode :: pop(indexNode** content, stackNode** newHead) {
    // Point the given pointers to the content and the next node respectively and delete this node
    *newHead = this->next;
    *content = this->entry;
    delete this;
}

stackNode* stackNode:: getNext(){
    return this->next;
}


Stack :: Stack() {
    this->head = NULL;
}

void Stack :: add(indexNode** input) {
    if (input == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }else if (*input == NULL){
        throw std::invalid_argument( "Got pointer to NULL pointer");
    }else{
        this->head = new stackNode(*input, this->head);
    }
    
}
indexNode*  Stack :: pop() {
    indexNode* nodeToReturn;
    if (this->head) {
        this->head->pop(&nodeToReturn, &this->head);
    } else {
        nodeToReturn = NULL;
    }
    return nodeToReturn;
}

int Stack :: getSize(){
    int count = 0;
    stackNode* currNode = this->head;
    while (currNode){
        count++;
        currNode = currNode->getNext();
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////

indexList :: indexList(entry* content, int distance, MatchType matchingMetric, indexList* next) {
    // Check input
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
    this->node = new indexNode(content, matchingMetric);
}

indexList :: ~indexList() {
    if (this->next) {
        // Delete next node(s) if they exist
        delete this->next;
    }
    // Then the contents of this node
    delete this->node;
}

int indexList :: getDistanceFromParent() const{
    return this->distanceFromParent;
}

int indexList :: addToList(entry* content,int distance) {
    if (content == NULL){
        throw std::invalid_argument( "Got NULL pointer");
    }
    if (distance <= 0){
        throw std::invalid_argument( "Invalid Distance Value");
    }

    indexList* next = this->next;
    if (distance == this->distanceFromParent) {                 // If we have the same distance as this node, push lower in the tree
        this->node->addEntry(content);
    } else if (this->next) {                                    // If we are not in the last node
        if (distance >= this->next->getDistanceFromParent()) {  // If the next node has distance lower than the input, pass input along
            this->next->addToList(content, distance);
        }else {                                                 // Else if the next node has higher distance, create a new list node between this one and the next
            this->next = new indexList(content, distance, this->node->getMatchingType(), next);
        }
    } else {                                                    // If there is no next node, create a new node after this
        this->next = new indexList(content, distance, this->node->getMatchingType(), next);
    }
    return 0;
}

void indexList::printList(int depth) {
    for (int i = 0; i <= depth*3 + 1; i++ ){
        std::cout << " ";
    }
    std::cout << " |->Diff: " << this->distanceFromParent << " ";
    this->node->printTree( depth + 1);
    if (this->next ) {
        this->next->printList(depth);
    }
}

indexNode* indexList :: getNode() const{
    return this->node;
}

indexList* indexList :: getNext() const {
    return this->next;
}

