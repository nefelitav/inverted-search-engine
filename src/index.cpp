#include "../include/index.hpp"
#include "../include/utilities.hpp"
#include "../include/structs.hpp"

indexNode ::indexNode(entry *input, QueryID id, unsigned int threshold, MatchType matchingMetric)
{
    this->MatchingType = matchingMetric;
    this->content = input;
    this->children = NULL;
}

MatchType indexNode ::getMatchingType()
{
    return this->MatchingType;
}

ErrorCode indexNode ::addEntry(entry *input, QueryID id, unsigned int threshold)
{
    unsigned int distance;
    // Check Input
    if (input == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }

    // If this node was created as a NULL head node, populate it
    if (this->content == NULL)
    {
        this->content = input;
        return EC_SUCCESS;
    }
    else
    {
        // Get distance for given metric, else return error
        if (this->MatchingType == MT_HAMMING_DIST)
        {
            distance = hammingDistance(this->content->getWord(), input->getWord());
        }
        else if (this->MatchingType == MT_EDIT_DIST)
        {
            distance = editDistance(this->content->getWord(), input->getWord());
        }
        else
        {
            return EC_FAIL;
        }
        // If the entry already exists, add to payload
        if (distance == 0)
        {
            try
            {
                this->content->addToPayload(id, threshold);
            }
            catch (const std::exception &_)
            {
                throw std::runtime_error("Entry by same query already in the tree");
                return EC_FAIL;
            }
            return EC_SUCCESS;
        }

        // If the node under examination is not the same as the given entry
        if (this->children == NULL)
        {
            //If there are no children create new list with the input in the first node
            this->children = new indexList(input, distance, this->MatchingType, id, threshold);
        }
        else if (this->children->getDistanceFromParent() > distance)
        {
            // Children list exists, first child has bigger distance
            indexList *oldFirstChild = this->children;
            this->children = new indexList(input, distance, this->MatchingType, id, threshold, oldFirstChild);
        }
        else
        { // Children list exists, new entry has equal or greater distance than first child
            try
            { // Give the entry to the list to handle
                this->children->addToList(input, distance, id, threshold);
                return EC_SUCCESS;
            }
            catch (const std::exception &_)
            {
                throw std::runtime_error("Word already in the tree");
                return EC_FAIL;
            }
        }
    }
    return EC_SUCCESS;
}

int indexNode ::printTree(int depth)
{
    // For debugging
    if (this->content == NULL)
    {
        std::cout << "EMPTY\n";
        return 0;
    }
    std::cout << "Word: " << this->content->getWord() << "  Payload: "
              << "\n";
    this->content->printPayload();

    if (this->children)
    {
        this->children->printList(depth);
    }
    return 0;
}
entry *indexNode ::getEntry()
{
    return this->content;
}

indexList *indexNode ::getChildren()
{
    return this->children;
}
indexNode ::~indexNode()
{
    delete this->children;
}

void printHammingTrees()
{
    // For debugging
    for (int k = 0; k < 27; ++k)
    {
        std::cout << (k + 4) << ": \n";
        hammingIndexes[k]->printTree();
    }
}

////////////////////////////////////////////////////////////////////////////////////

ErrorCode lookup_entry_index(const word w, entry_list *result, MatchType queryMatchingType)
{

    unsigned int distance = 0;
    // Check input
    if (w == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }

    // Store the children of current node, if any
    indexList *currChild;

    payloadNode *currPayloadNode;

    // New result to be stored on the result list
    entry *tempEntry = NULL;

    // Nodes to be examined next
    Stack stack;
    indexNode *toadd;
    indexNode *currNode = NULL;

    // Select the index to search in
    if (queryMatchingType == MT_EDIT_DIST)
    {
        currNode = editIndex;
    }
    else if (queryMatchingType == MT_HAMMING_DIST)
    {
        currNode = hammingIndexes[strlen(w) - 4];
    }
    else
    {
        return EC_FAIL;
    }

    // If empty, return no result
    if (currNode->getEntry() == NULL)
    {
        return EC_SUCCESS;
    }

    // Main loop
    while (currNode)
    {
        tempEntry = NULL;
        // Calculate distance from target word
        if (queryMatchingType == MT_HAMMING_DIST)
        {
            distance = hammingDistance(currNode->getEntry()->getWord(), w);
        }
        else if (queryMatchingType == MT_EDIT_DIST)
        {
            distance = editDistance(currNode->getEntry()->getWord(), w);
        }
        else
        {
            return EC_FAIL;
        }

        // Add to results if close enough and the entry exists
        currPayloadNode = currNode->getEntry()->getPayload();
        while (currPayloadNode)
        {
            if (currPayloadNode->getThreshold() >= distance)
            {
                if (tempEntry == NULL)
                {
                    tempEntry = new entry(currNode->getEntry()->getWord());
                }
                tempEntry->addToPayload(currPayloadNode->getId(), currPayloadNode->getThreshold());
            }
            else
            {
                // If we get to a payload with more distance break, they are stored with descending order
                break;
            }
            currPayloadNode = currPayloadNode->getNext();
        }
        if (tempEntry)
        {
            // Add to result, if populated
            result->addEntry(tempEntry);
            tempEntry = NULL;
        }

        // Add any applicable children to examine later
        currChild = currNode->getChildren();
        while (currChild)
        {

            // If the child meets the criteria, add it to the stack
            if ((int)(currChild->getDistanceFromParent()) >= (int)distance - 3 && (currChild->getDistanceFromParent()) <= distance + 3)
            {
                toadd = currChild->getNode();
                stack.add(&toadd);
            }
            else if ((currChild->getDistanceFromParent()) > distance + 3)
            {
                // Children are ordered by distance, if one in not acceptable we can skip the rest
                break;
            }
            currChild = currChild->getNext();
        }
        // Get the next item to be examined
        currNode = stack.pop();
    }
    return EC_SUCCESS;
}

ErrorCode addToIndex(entry *toAdd, QueryID queryId, MatchType queryMatchingType, unsigned int threshold)
{
    if (toAdd == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }

    try
    {
        // Add to appropriate index // protect each index
        if (queryMatchingType == MT_EDIT_DIST)
        {
            pthread_mutex_lock(&editLock);
            editIndex->addEntry(toAdd, queryId, threshold);
            pthread_mutex_unlock(&editLock);
        }
        else if (queryMatchingType == MT_HAMMING_DIST)
        {
            pthread_mutex_lock(&hammingLock[strlen(toAdd->getWord()) - 4]);
            hammingIndexes[strlen(toAdd->getWord()) - 4]->addEntry(toAdd, queryId, threshold);
            pthread_mutex_unlock(&hammingLock[strlen(toAdd->getWord()) - 4]);
        }
        else if (queryMatchingType == MT_EXACT_MATCH)
        {
            pthread_mutex_lock(&exactLock);
            ET->addToBucket(hashFunction(toAdd->getWord()), toAdd);
            pthread_mutex_unlock(&exactLock);
        }

        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
}

ErrorCode removeFromIndex(const word givenWord, const QueryID queryId, const MatchType givenType)
{
    // Check input
    if (givenWord == NULL)
    {
        throw std::invalid_argument("Got NULL pointer to word");
    }

    if (givenType != MT_EDIT_DIST && givenType != MT_EXACT_MATCH && givenType != MT_HAMMING_DIST)
    {
        throw std::invalid_argument("Invalid Matching Type");
    }

    unsigned int distance = 0;
    unsigned int threshold = 0;
    indexNode *ix;

    try
    {
        if (givenType == MT_EDIT_DIST)
        {
            ix = editIndex;
        }
        else if (givenType == MT_HAMMING_DIST)
        {
            ix = hammingIndexes[strlen(givenWord) - 4];
        }
        else if (givenType == MT_EXACT_MATCH)
        {
            ET->deleteQueryId(givenWord, queryId);
            return EC_SUCCESS;
        }
        // Store the children of current node, if any
        indexList *currChild;

        // Nodes to be examined next
        Stack stack;
        indexNode *toadd;

        // Start the search with the root of the tree
        indexNode *currNode = ix;

        // Main loop
        while (currNode)
        {
            // Calculate distance from target word
            if (currNode->getMatchingType() == MT_HAMMING_DIST)
            {
                distance = hammingDistance(currNode->getEntry()->getWord(), givenWord);
            }
            else if (currNode->getMatchingType() == MT_EDIT_DIST)
            {
                distance = editDistance(currNode->getEntry()->getWord(), givenWord);
            }
            else
            {
                return EC_FAIL;
            }

            // Delete payload entry if found
            if (currNode->getEntry()->emptyPayload() == false && distance == 0)
            {
                currNode->getEntry()->deletePayloadNode(queryId);
                if (givenType == MT_EDIT_DIST)
                {
                    ix = editIndex;
                }
                else
                {
                    ix = hammingIndexes[strlen(givenWord) - 4];
                }
                return EC_SUCCESS;
            }
            // Add any applicable children to examine later
            currChild = currNode->getChildren();
            while (currChild)
            {
                // If the child meets the criteria, add it to the stack
                if ((int)(currChild->getDistanceFromParent()) >= (int)distance - (int)threshold && (currChild->getDistanceFromParent()) <= distance + threshold)
                {
                    toadd = currChild->getNode();
                    stack.add(&toadd);
                }
                else if ((currChild->getDistanceFromParent()) > distance + threshold)
                {
                    // Children are ordered by distance, if one in not acceptable we can skip the rest
                    break;
                }
                currChild = currChild->getNext();
            }
            // Get the next item to be examined
            currNode = stack.pop();
        }
        if (givenType == MT_EDIT_DIST)
        {
            ix = editIndex;
        }
        else
        {
            ix = hammingIndexes[strlen(givenWord) - 4];
        }
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
}

ErrorCode destroy_entry_index(indexNode *ix)
{
    try
    {
        delete ix;
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
}

////////////////////////////////////////////////////////////////////////////////////

stackNode ::stackNode(indexNode *input, stackNode *oldHead)
{
    if (input == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    this->entry = input;
    this->next = oldHead;
}
void stackNode ::pop(indexNode **content, stackNode **newHead)
{
    // Point the given pointers to the content and the next node respectively and delete this node
    *newHead = this->next;
    *content = this->entry;
    delete this;
}

stackNode *stackNode ::getNext()
{
    return this->next;
}

Stack ::Stack()
{
    this->head = NULL;
}

void Stack ::add(indexNode **input)
{
    if (input == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    else if (*input == NULL)
    {
        throw std::invalid_argument("Got pointer to NULL pointer");
    }
    this->head = new stackNode(*input, this->head);
}
indexNode *Stack ::pop()
{
    indexNode *nodeToReturn;
    if (this->head)
    {
        // Return and the first node and set the next one as head
        this->head->pop(&nodeToReturn, &this->head);
        return nodeToReturn;
    }
    return NULL;
}

int Stack ::getSize()
{
    int count = 0;
    stackNode *currNode = this->head;
    while (currNode)
    {
        ++count;
        currNode = currNode->getNext();
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////

indexList ::indexList(entry *content, unsigned int distance, MatchType matchingMetric, QueryID id, unsigned int threshold, indexList *next)
{
    // Check input
    if (matchingMetric != MT_HAMMING_DIST && matchingMetric != MT_EDIT_DIST)
    {
        throw std::invalid_argument("Invalid Distance Metric");
    }
    if (distance == 0)
    {
        throw std::invalid_argument("Invalid Distance Value");
    }
    if (content == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    // Point to the next list node and set distance from parrent for this node
    this->next = next;
    this->distanceFromParent = distance;
    // Create the index node with the given content
    this->node = new indexNode(content, id, threshold, matchingMetric);
}

unsigned int indexList ::getDistanceFromParent() const
{
    return this->distanceFromParent;
}

int indexList ::addToList(entry *content, unsigned int distance, QueryID id, unsigned int threshold)
{
    if (content == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }

    indexList *next = this->next;

    if (distance == this->distanceFromParent)
    { // If we have the same distance as this node, push lower in the tree
        this->node->addEntry(content, id, threshold);
        return 0;
    }
    else if (this->next)
    { // If we are not in the last node
        if (distance >= this->next->distanceFromParent)
        { // If the next node has distance lower than the input, pass input along
            this->next->addToList(content, distance, id, threshold);
        }
        else if (this->node->getMatchingType() == MT_EDIT_DIST)
        { // Else if the next node has higher distance, create a new list node between this one and the next
            this->next = new indexList(content, distance, this->node->getMatchingType(), id, threshold, next);
        }
        return 0;
    }
    // If there is no next node, create a new node after this
    this->next = new indexList(content, distance, this->node->getMatchingType(), id, threshold, next);
    return 0;
}

void indexList ::printList(int depth)
{
    for (int i = 0; i <= depth * 3 + 1; ++i)
    {
        std::cout << " ";
    }
    std::cout << " |->Diff: " << this->distanceFromParent << " ";
    this->node->printTree(depth + 1);
    if (this->next)
    {
        this->next->printList(depth);
    }
}

indexNode *indexList ::getNode() const
{
    return this->node;
}

indexList *indexList ::getNext() const
{
    return this->next;
}

indexList ::~indexList()
{
    if (this->next)
    {
        // Delete next node(s) if they exist
        delete this->next;
    }
    // Then the contents of this node
    delete this->node;
}
