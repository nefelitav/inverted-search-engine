#include "index.hpp"
#include "functions.hpp"

indexing :: indexing(entry* input, MatchType matchingMetric) {
    std::cout<<"Creating new node\n";
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

int indexing :: addEntry(entry* input) {
    int distance;

    // Get distance for given metric
    if (this->getMatchingType() == MT_HAMMING_DIST) {
        //cout<<"Selected HAMMING\n";
        distance = hammingDistance(this->content->getWord(), input->getWord());
    } else if(this->getMatchingType() == MT_EDIT_DIST) {
        //cout<<"Selected EDIT\n";
        distance = editDistance(this->content->getWord(), input->getWord());
    } else if(this->getMatchingType() == MT_EXACT_MATCH) {
        distance = exactMatch(this->content->getWord(), input->getWord());
    }

    //cout<<"Distance : "<<distance<<'\n';
    if (this->children == NULL) {            //If there are no children
        //cout<<"No children list found, making new list\n";
        this->children = new treeNodeList(input, distance,this->getMatchingType());
    } else if (this->children->getDistanceFromParent() > distance) {      // Children list exists, first child has bigger dist
        //cout<<"Putting in front\n";
        treeNodeList* oldFirstChild = this->children;
        this->children = new treeNodeList(input, distance, this->getMatchingType(), oldFirstChild);
    } else {          // Children list exists, new entry has equal or greater dist than first child
        //cout<<"Sending in to list\n";
        this->children->addToList(input, distance);
    }
    return 0;
}

int indexing :: printTree() {
    std::cout<<"Word: "<<this->content->getWord();
    if (this->children ) {
        std::cout<<" HAS CHILDREN: \n";
        this->children->printList();
        std::cout<<"Children over for: "<<this->content->getWord();
    }
    std::cout<<"\n";
    return 0;
}
entry* indexing :: getEntry() {
    return this->content;
}

treeNodeList* indexing :: getChildren() {
    return this->children;
}

ErrorCode buildEntryIndex(const entry_list* el, MatchType type, indexing** ix) {
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


ErrorCode lookup_entry_index(const word* w, indexing* ix, int threshold, entry_list** result) {
    int distance;

    try {
        // List to store results
        *result = new entry_list();

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
            } else if (currNode->getMatchingType() == MT_EXACT_MATCH) {
                distance = exactMatch(currNode->getEntry()->getWord(), *w);
            }

            // Add to results if close enough
            if  (distance <= threshold) {
                tempEntry = new entry (currNode->getEntry()->getWord());
                (*result)->addEntry (tempEntry);
            }

            // Add any applicable children to examine later
            currChild = currNode->getChildren();
            while (currChild) {
                if (currChild->getDistanceFromParent() >= distance-threshold && currChild->getDistanceFromParent() <= distance+threshold) {
                    toEnqueue = currChild->getNode();
                    queue->enqueue(&toEnqueue);
                } else {
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



childQueue :: childQueue(indexing* input, childQueue* oldHead) {
    this->entry = input;
    this->next = oldHead;
}
void childQueue :: pop(indexing** content, childQueue** newHead) {
    *newHead = this->next;
    *content = this->entry;
    delete this;
}
Queue :: Queue() {
    this->head = NULL;
}
int Queue :: enqueue(indexing** input) {
    this->head = new childQueue(*input, this->head);
    return 0;
}
int Queue :: dequeue(indexing** nodeToReturn) {
    if (this->head) {
        this->head->pop(nodeToReturn, &this->head);
    } else {
        *nodeToReturn = NULL;
    }
    return 0;
}


treeNodeList :: treeNodeList(entry* content, int distance, MatchType matchingMetric, treeNodeList* next) {

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

int treeNodeList :: getDistanceFromParent() {
    return this->distanceFromParent;
}

void treeNodeList :: addToList(entry* content,int distance) {
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
}

void treeNodeList :: printList() {
    std::cout << "Difference from parent: " << this->distanceFromParent << " ";
    this->node->printTree();
    if (this->next) {
        this->next->printList();
    }
}

indexing* treeNodeList :: getNode() {
    return this->node;
}

treeNodeList* treeNodeList :: getNext() {
    return this->next;
}

