#include "../include/index.hpp"
#include "../include/utilities.hpp"

indexNode ::indexNode(entry **input, QueryID id, unsigned int threshold, MatchType matchingMetric)
{
    if (matchingMetric != MT_HAMMING_DIST && matchingMetric != MT_EDIT_DIST)
    {
        throw std::invalid_argument("Invalid Distance Metric");
    }
    this->MatchingType = matchingMetric;
    this->content = input;
    this->children = NULL;
    if (input != NULL)
    {
        (*this->getEntry())->addPayload(id, threshold);
    }
}
indexNode ::~indexNode()
{
    delete this->children;
}

MatchType indexNode ::getMatchingType()
{
    return this->MatchingType;
}

ErrorCode indexNode ::addEntry(entry **input, QueryID id, unsigned int threshold)
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
        try
        {
            (*this->content)->addPayload(id, threshold);
        }
        catch (const std::exception &_)
        {
            throw std::runtime_error("Entry by same query already in the tree");
            return EC_FAIL;
        }
    }
    else
    {
        // Get distance for given metric
        if (this->getMatchingType() == MT_HAMMING_DIST)
        {
            distance = hammingDistance((*this->content)->getWord(), (*input)->getWord());
        }
        else if (this->getMatchingType() == MT_EDIT_DIST)
        {
            distance = editDistance((*this->content)->getWord(), (*input)->getWord());
        }
        else
        {
            return EC_FAIL;
        }

        // If the entry already exists, add to payload list
        if (distance == 0)
        {
            try
            {
                (*this->content)->addPayload(id, threshold);
            }
            catch (const std::exception &_)
            {
                throw std::runtime_error("Entry by same query already in the tree");
                return EC_FAIL;
            }
            return EC_SUCCESS;
        }

        if (this->children == NULL)
        { //If there are no children create new list with the input in the first node
            this->children = new indexList(input, distance, this->getMatchingType(), id, threshold);
        }
        else if (this->children->getDistanceFromParent() > distance)
        { // Children list exists, first child has bigger dist
            indexList *oldFirstChild = this->children;
            this->children = new indexList(input, distance, this->getMatchingType(), id, threshold, oldFirstChild);
        }
        else
        { // Children list exists, new entry has equal or greater dist than first child
            try
            { // Give the entry to the list to handle
                this->children->addToList(input, distance, id, threshold);
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
    std::cout << "Word: " << (*this->content)->getWord() << "Payload: " << (*this->content)->getPayload()->isEmpty() << "\n";
    if (this->children)
    {
        this->children->printList(depth);
    }
    return 0;
}
entry **indexNode ::getEntry()
{
    return this->content;
}

indexList *indexNode ::getChildren()
{
    return this->children;
}

////////////////////////////////////////////////////////////////////////////////////

ErrorCode lookup_entry_index(const word *w, entry_list *result, MatchType queryMatchingType)
{
    int threshold = 3;
    int distance;
    indexNode *ix;

    // Check input
    if (w == NULL || *w == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    if (threshold < 0)
    {
        throw std::invalid_argument("Invalid Threshold");
    }

    try
    {
        // Store the children of current node, if any
        indexList *currChild;

        payloadNode *currPayloadNode;

        // New result to be stored on the result list
        entry *tempEntry = NULL;

        // Nodes to be examined next
        Stack *stack = new Stack();
        indexNode *toadd;

        if (queryMatchingType == MT_EDIT_DIST)
        {
            ix = editIndex;
        }
        else if (queryMatchingType == MT_HAMMING_DIST)
        {
            ix = hammingIndexes[strlen(*w) - 4];
        }
        else if (queryMatchingType == MT_EXACT_MATCH)
        {
            // TODO
        }
        else
        {
            return EC_FAIL;
        }

        if (ix->getEntry() == NULL)
        {
            delete stack;
            return EC_SUCCESS;
        }

        // Start the search with the root of the tree
        indexNode *currNode = ix;

        // Main loop
        while (currNode)
        {
            // Calculate distance from target word
            if (queryMatchingType == MT_HAMMING_DIST)
            {
                distance = hammingDistance((*currNode->getEntry())->getWord(), *w);
            }
            else if (queryMatchingType == MT_EDIT_DIST)
            {
                distance = editDistance((*currNode->getEntry())->getWord(), *w);
            }
            else
            {
                return EC_FAIL;
            }

            // Add to results if close enough and the entry exists
            if ((*currNode->getEntry())->getPayload()->isEmpty() == false)
            {

                currPayloadNode = (*currNode->getEntry())->getPayload()->getHead();
                while (currPayloadNode != NULL)

                {
                    if ((int)currPayloadNode->getThreshold() >= distance)
                    {
                        if (tempEntry == NULL)
                        {
                            tempEntry = new entry((*currNode->getEntry())->getWord());
                        }
                        tempEntry->addPayload(currPayloadNode->getId(), currPayloadNode->getThreshold());
                    }
                    else
                    {
                        break;
                    }
                    currPayloadNode = currPayloadNode->getNext();
                }
                if (tempEntry != NULL)
                {
                    result->addEntry(tempEntry);
                    tempEntry = NULL;
                }
            }

            // Add any applicable children to examine later
            currChild = currNode->getChildren();
            while (currChild)
            {
                // If the child meets the criteria, add it to the stack
                if ((int)(currChild->getDistanceFromParent()) >= distance - threshold && (int)(currChild->getDistanceFromParent()) <= distance + threshold)
                {
                    toadd = currChild->getNode();
                    stack->add(&toadd);
                }
                else if ((int)(currChild->getDistanceFromParent()) > distance + threshold)
                {
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

ErrorCode addToIndex(entry **toAdd, QueryID queryId, MatchType queryMatchingType, unsigned int threshold)
{

    if (toAdd == NULL || *toAdd == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    if (threshold < 0)
    {
        throw std::invalid_argument("Invalid Threshold");
    }
    if (queryId <= 0)
    {
        throw std::invalid_argument("Invalid Query ID");
    }
    if (queryMatchingType != MT_EDIT_DIST && queryMatchingType != MT_EXACT_MATCH && queryMatchingType != MT_HAMMING_DIST)
    {
        throw std::invalid_argument("Invalid Matching Type");
    }

    try
    {
        if (queryMatchingType == MT_EDIT_DIST)
        {
            editIndex->addEntry(toAdd, queryId, threshold);
        }
        else if (queryMatchingType == MT_HAMMING_DIST)
        {
            hammingIndexes[strlen((*toAdd)->getWord()) - 4]->addEntry(toAdd, queryId, threshold);
        }
        else if (queryMatchingType == MT_EXACT_MATCH)
        {
            ET->addToBucket(hashFunction((*toAdd)->getWord()), *toAdd);
        }
        else
        {
            return EC_FAIL;
        }
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
}

ErrorCode removeFromIndex(const word *givenWord, QueryID queryId, MatchType givenType)
{

    // Check input
    if (givenWord == NULL || *givenWord == NULL)
    {
        throw std::invalid_argument("Got NULL pointer to word");
    }
    if (queryId <= 0)
    {
        throw std::invalid_argument("Invalid Query ID");
    }
    if (givenType != MT_EDIT_DIST && givenType != MT_EXACT_MATCH && givenType != MT_HAMMING_DIST)
    {
        throw std::invalid_argument("Invalid Matching Type");
    }

    int distance;
    int threshold = 0;
    indexNode *ix;

    try
    {

        if (givenType == MT_EDIT_DIST)
        {
            ix = editIndex;
        }
        else if (givenType == MT_HAMMING_DIST)
        {
            ix = hammingIndexes[strlen(*givenWord) - 4];
        }
        else if (givenType == MT_EXACT_MATCH)
        {
            // TODO
        }
        else
        {
            return EC_FAIL;
        }

        // Store the children of current node, if any
        indexList *currChild;

        // Nodes to be examined next
        Stack *stack = new Stack();
        indexNode *toadd;

        // Start the search with the root of the tree
        indexNode *currNode = ix;

        // Main loop
        while (currNode)
        {
            // Calculate distance from target word
            if (currNode->getMatchingType() == MT_HAMMING_DIST)
            {
                distance = hammingDistance((*currNode->getEntry())->getWord(), *givenWord);
            }
            else if (currNode->getMatchingType() == MT_EDIT_DIST)
            {
                distance = editDistance((*currNode->getEntry())->getWord(), *givenWord);
            }
            else if (currNode->getMatchingType() == MT_EXACT_MATCH)
            {
                //TODO
            }
            else
            {
                return EC_FAIL;
            }

            // Delete payload entry if found
            if ((*currNode->getEntry())->getPayload()->isEmpty() == false && distance == 0)
            {
                (*currNode->getEntry())->getPayload()->deleteNode(queryId);
                delete stack;
                return EC_SUCCESS;
            }
            // Add any applicable children to examine later
            currChild = currNode->getChildren();
            while (currChild)
            {
                // If the child meets the criteria, add it to the stack
                if ((int)(currChild->getDistanceFromParent()) >= distance - threshold && (int)(currChild->getDistanceFromParent()) <= distance + threshold)
                {
                    toadd = currChild->getNode();
                    stack->add(&toadd);
                }
                else if ((int)(currChild->getDistanceFromParent()) > distance + threshold)
                {
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
    else
    {
        this->entry = input;
        this->next = oldHead;
    }
}
void stackNode ::pop(indexNode **content, stackNode **newHead)
{
    // Point the given pointers to the content and the next node respectively and delete this node
    *newHead = this->next;
    *content = this->entry;
    delete this;
}

stackNode *stackNode::getNext()
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
    else
    {
        this->head = new stackNode(*input, this->head);
    }
}
indexNode *Stack ::pop()
{
    indexNode *nodeToReturn;
    if (this->head)
    {
        this->head->pop(&nodeToReturn, &this->head);
    }
    else
    {
        nodeToReturn = NULL;
    }
    return nodeToReturn;
}

int Stack ::getSize()
{
    int count = 0;
    stackNode *currNode = this->head;
    while (currNode)
    {
        count++;
        currNode = currNode->getNext();
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////

indexList ::indexList(entry **content, unsigned int distance, MatchType matchingMetric, QueryID id, unsigned int threshold, indexList *next)
{
    // Check input
    if (content == NULL || *content == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    if (matchingMetric != MT_HAMMING_DIST && matchingMetric != MT_EDIT_DIST)
    {
        throw std::invalid_argument("Invalid Distance Metric");
    }
    if (distance <= 0)
    {
        throw std::invalid_argument("Invalid Distance Value");
    }
    // Point to the next list node and set distance from parrent for this node
    this->next = next;
    this->distanceFromParent = distance;
    // Create the index node with the given content
    this->node = new indexNode(content, id, threshold, matchingMetric);
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

unsigned int indexList ::getDistanceFromParent() const
{
    return this->distanceFromParent;
}

int indexList ::addToList(entry **content, unsigned int distance, QueryID id, unsigned int threshold)
{
    if (content == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    if (distance <= 0)
    {
        throw std::invalid_argument("Invalid Distance Value");
    }

    indexList *next = this->next;
    if (distance == this->distanceFromParent)
    { // If we have the same distance as this node, push lower in the tree
        this->node->addEntry(content, id, threshold);
    }
    else if (this->next)
    { // If we are not in the last node
        if (distance >= this->next->getDistanceFromParent())
        { // If the next node has distance lower than the input, pass input along
            this->next->addToList(content, distance, id, threshold);
        }
        else
        { // Else if the next node has higher distance, create a new list node between this one and the next
            this->next = new indexList(content, distance, this->node->getMatchingType(), id, threshold, next);
        }
    }
    else
    { // If there is no next node, create a new node after this
        this->next = new indexList(content, distance, this->node->getMatchingType(), id, threshold, next);
    }
    return 0;
}

void indexList ::printList(int depth)
{
    for (int i = 0; i <= depth * 3 + 1; i++)
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

////////////////////////////////////////////////////////////////////////////////////
payloadNode ::payloadNode(QueryID id, unsigned int threshold, payloadNode *next)
{

    if (id <= 0)
    {
        throw std::invalid_argument("Got Invalid ID");
    }
    if (threshold < 0)
    {
        throw std::invalid_argument("Got Invalid Threshold");
    }

    this->id = id;
    this->threshold = threshold;
    this->next = next;
}

const QueryID payloadNode ::getId() const
{
    return this->id;
}

const unsigned int payloadNode ::getThreshold() const
{
    return this->threshold;
}

payloadNode *payloadNode ::getNext()
{
    return this->next;
}

void payloadNode ::setNext(payloadNode *newNext)
{
    this->next = newNext;
}

void payloadNode ::addNode(QueryID id, unsigned int threshold)
{
    if (id <= 0)
    {
        throw std::invalid_argument("Got Invalid ID");
    }
    if (threshold < 0)
    {
        throw std::invalid_argument("Got Invalid Threshold");
    }

    payloadNode *temp;
    if (this->next == NULL)
    {
        this->next = new payloadNode(id, threshold);
    }
    else if (this->next->getId() > id)
    {
        temp = this->next;
        this->next = new payloadNode(id, threshold, temp);
    }
    else if (this->next->getId() == id)
    { /////////////////////
    }
    else
    {
        this->next->addNode(id, threshold);
    }
}

payload ::payload()
{
    this->head = NULL;
}

payload ::~payload()
{
    payloadNode *prev, *curr = this->head;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->getNext();
        delete prev;
        prev = NULL;
    }
}

void payload ::deleteNode(QueryID id)
{

    if (id <= 0)
    {
        throw std::invalid_argument("Got Invalid ID");
    }

    payloadNode *temp, *curr = this->head;
    if (this->head == NULL)
    {
        std::cout << "ID NOT FOUND\n";
    }
    else if (this->head != NULL)
    {
        if (head->getId() == id)
        {
            temp = this->head;
            this->head = temp->getNext();
            delete temp;
            if (this->head != NULL)
            {
                curr = head->getNext();
                temp = this->head;
            }
            else
            {
                curr = NULL;
            }
        }
    }
    while (curr != NULL)
    {
        if (curr->getId() == id)
        {
            temp->setNext(curr->getNext());
            delete curr;
            curr = temp->getNext();
            break;
        }
        else
        {
            temp = curr;
            curr = curr->getNext();
        }
    }
}

bool payload ::isEmpty()
{
    return (this->head == NULL);
}

payloadNode *payload ::getHead()
{
    return this->head;
}

void payload ::insertNode(QueryID id, unsigned int threshold)
{

    if (id <= 0)
    {
        throw std::invalid_argument("Got Invalid ID");
    }
    if (threshold < 0)
    {
        throw std::invalid_argument("Got Invalid Threshold");
    }

    payloadNode *temp;
    if (this->isEmpty())
    {
        this->head = new payloadNode(id, threshold);
    }
    else if (this->head->getId() > id)
    {
        temp = this->head;
        this->head = new payloadNode(id, threshold, temp);
    }
    else if (this->head->getId() == id)
    {
        throw std::invalid_argument("Query already in payload");
    }
    else
    {
        this->head->addNode(id, threshold);
    }
}

/////////////////////////////////////////////////////////////////////////
