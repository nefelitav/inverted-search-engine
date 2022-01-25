#include "../include/structs.hpp"
#include "../include/core.h"
#include "../include/jobscheduler.hpp"
Query ::Query(QueryID id, char *words, MatchType match_type, unsigned int match_dist)
{
    int i = 0, length = 0;
    char* strTokSavePtr = NULL;
    word token = strtok_r((char *)words, " ", &strTokSavePtr); // used instead of strtok, since it is thread safe
    while (token != NULL)           // get all words from input 
    {
        length = strlen(token) + 1; // length of each word
        words += length;            // pointer to input
        if (*words == '\0')         // no more words in input
        {
            memcpy(this->words + (MAX_WORD_LENGTH + 1) * i, token, length);
            ++i;
            break;
        }
        memcpy(this->words + (MAX_WORD_LENGTH + 1) * i, token, length);
        ++i;
        token = strtok_r(NULL, " ", &strTokSavePtr);
    }
    this->entriesNum = i;                              // save number of entries
    this->matchedEntries = new bool[this->entriesNum]; // every cell corresponds to an entry
    memset(this->matchedEntries, 0, this->entriesNum * (sizeof(bool)));

    this->id = id;
    this->match_type = match_type;
    this->match_dist = match_dist;
}

const QueryID Query ::getId() const
{
    return this->id;
}
const MatchType Query ::getMatchingType() const
{
    return this->match_type;
}
const unsigned int Query ::getMatchingDistance() const
{
    return this->match_dist;
}
const word Query ::getWord(int word_num) const // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]
{
    if (word_num >= this->entriesNum) // out of range
    {
        //std::cout << "Sorry, index out of range." << std::endl;
        return NULL;
    }
    const word ptr = (word)(this->words + word_num * (MAX_WORD_LENGTH + 1)); // find the word
    if (*ptr == '\0')                                                        // no more words
    {
        //std::cout << "Sorry, index out of range." << std::endl;
        return NULL;
    }
    return ptr;
}
void Query ::setTrue(const word entry_word) // entry matched with a doc word
{
    for (int i = 0; i < this->getWordNum(); ++i)
    {
        if (!strcmp(entry_word, this->getWord(i)))
        {
            this->matchedEntries[i] = 1;
        }
    }
}
void Query ::setFalse() // set to default to start a new search with new doc
{
    memset(this->matchedEntries, 0, this->entriesNum * (sizeof(bool)));
}
bool Query ::matched() // check if all entries matched with doc words
{
    int sum = 0;
    for (int i = 0; i < this->entriesNum; ++i)
    {
        sum += this->matchedEntries[i];
    }
    return (sum == this->entriesNum);
}
void Query ::printMatchedEntries() // for debugging
{
    for (int i = 0; i < this->entriesNum; ++i)
    {
        std::cout << this->matchedEntries[i] << std::endl;
    }
}
bool *Query ::getMatchedEntries()
{
    return this->matchedEntries;
}

const int Query ::getWordNum() const
{
    return this->entriesNum;
}

Query ::Query(const Query &oldQuery) // copy constructor used to have a local query table which helps in parallelization
{
    this->id = oldQuery.id;
    this->match_dist = oldQuery.match_dist;
    this->match_type = oldQuery.match_type;
    this->entriesNum = oldQuery.entriesNum;
    this->matchedEntries = new bool[this->entriesNum];
    memset(this->matchedEntries, 0, this->entriesNum * (sizeof(bool)));
    memcpy(this->words, oldQuery.words, MAX_QUERY_LENGTH);
}

Query ::~Query()
{
    delete[] this->matchedEntries;
    //std::cout << "Query with id = " << this->id << " is deleted!" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////

Document ::Document(DocID id, char *words)
{
    int i = 0, length = 0;
    this->wordCount = 0;
    char* strTokSavePtr = NULL;
    word token = strtok_r((char *)words, " ", &strTokSavePtr);
    while (token != NULL) // get all words from input 
    {
        length = strlen(token) + 1; // length of each word
        words += length;            // pointer to input
        if (*words == '\0')         // no more words in input
        {
            memcpy(this->text + (MAX_WORD_LENGTH + 1) * i, token, length);
            ++i;
            break;
        }
        memcpy(this->text + (MAX_WORD_LENGTH + 1) * i, token, length);
        ++i;
        token = strtok_r(NULL, " ", &strTokSavePtr);
    }
    this->id = id;
    this->wordCount = i;
    //std::cout << "Document with id = " << this->id << " is created!" << std::endl;
}

const word Document ::getWord(int word_num) const // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]
{
    if (word_num >= this->wordCount) // out of range
    {
        //cout << "Sorry, index out of range." << endl;
        return NULL;
    }
    const word ptr = (const word)(this->text + word_num * (MAX_WORD_LENGTH + 1)); // find the word
    if (*ptr == '\0')
    {
        //cout << "Sorry, index out of range." << endl;
        return NULL;
    }
    return ptr;
}

////////////////////////////////////////////////////////////////////////////////////

payloadNode ::payloadNode(QueryID id, unsigned int threshold, payloadNode *next)
{

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

entry ::entry(const word keyword)
{
    if (keyword == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    strcpy(this->keyword, keyword);
    this->payload = NULL;
    this->next = NULL;
    //std::cout << "Entry is created!" << std::endl;
}

const word entry ::getWord() const
{
    return (const word)this->keyword;
}

entry *entry ::getNext() const
{
    return this->next;
}

void entry ::setNext(entry *e)
{
    if (this->next == NULL)
    {
        this->next = e;
    }
}

payloadNode *entry ::getPayload() const
{
    return this->payload;
}

void entry ::addToPayload(QueryID id, unsigned int threshold)
{
    payloadNode *curr;
    payloadNode *new_node;
    if (this->emptyPayload())
    {
        this->payload = new payloadNode(id, threshold);
    }
    else
    {
        if (this->payload->getThreshold() < threshold)
        {
            curr = this->payload;
            new_node = new payloadNode(id, threshold);
            new_node->setNext(curr);
            this->payload = new_node;
            return;
        }
        curr = this->payload;
        while (curr->getNext())
        {
            if (curr->getNext()->getThreshold() > threshold)
            {
                curr = curr->getNext();
            }
            else
            {
                break;
            }
        }
        new_node = new payloadNode(id, threshold);
        new_node->setNext(curr->getNext());
        curr->setNext(new_node);
    }
}

void entry ::printPayload()
{
    payloadNode *curr = this->payload;
    while (curr != NULL)
    {
        std::cout << curr->getId() << " ";
        curr = curr->getNext();
    }
    std::cout << std::endl;
}

bool entry ::emptyPayload()
{
    return (this->payload == NULL);
}

void entry ::deletePayloadNode(QueryID id)
{
    payloadNode *curr = this->payload;
    payloadNode *next = curr->getNext();
    payloadNode *temp;
    if (curr->getId() == id)
    {
        delete curr;
        this->payload = next;
        return;
    }
    while (curr != NULL)
    {
        if (curr->getNext() != NULL)
        {
            if (curr->getNext()->getId() == id)
            {
                temp = curr->getNext();
                curr->setNext(curr->getNext()->getNext());
                delete temp;
                return;
            }
        }
        curr = curr->getNext();
    }
}

entry ::~entry()
{
    //std::cout <<"------------------------------" << this->keyword << std::endl;
    payloadNode *prev, *curr = this->payload; // delete payload
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->getNext();
        delete prev;
        prev = NULL;
    }
    //std::cout << "Entry is deleted!" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////

entry_list ::entry_list()
{
    this->head = NULL;
    this->entryNum = 0;
    //std::cout << "Entry list is created!" << std::endl;
}
unsigned int entry_list ::getEntryNum() const
{
    return this->entryNum;
}
entry *entry_list ::getHead() const
{
    return this->head;
}
void entry_list ::addEntry(entry *new_entry)
{
    if (new_entry == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    if (this->head == NULL)
    {
        this->head = new_entry;
    }
    else
    {
        new_entry->setNext(this->head);
        this->head = new_entry;
    }
    ++this->entryNum;
}
void entry_list ::printList()
{
    entry *curr = this->head;
    entry *next = NULL;

    while (curr != NULL)
    {
        next = curr->getNext();
        std::cout << curr->getWord() << std::endl;
        curr->printPayload();
        curr = next;
    }
}

entry *entry_list ::getNext(entry *e) const
{
    return e->getNext();
}

void entry_list ::removeEntry(entry *toRemove)
{
    entry *curr = this->head;
    entry *next = NULL;

    if (curr == toRemove)
    {
        delete toRemove;
        toRemove = NULL;
    }
    while (curr != NULL)
    {
        next = curr->getNext();
        if (next == toRemove)
        {
            curr->setNext(next->getNext());
            delete next;
            next = NULL;
            return;
        }
        curr = next;
    }
    --this->entryNum;
}


////////////////////////////////////////////////////////////////////////////////////

ErrorCode create_entry(const word *w, entry **e)
{
    try
    {
        *e = new entry(*w);
        //cout << (**e).getWord() << endl;
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry(entry *e)
{
    try
    {
        if (e != NULL)
        {
            delete e;
            e = NULL;
        }
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode create_entry_list(entry_list **el)
{
    try
    {
        *el = new entry_list();
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry_list(entry_list *el) // first delete entries and then list
{
    try
    {
        entry *curr = el->getHead(); // pointer to head of list
        entry *next = NULL;

        while (curr != NULL) // delete entries
        {
            next = curr->getNext(); 
            destroy_entry(curr);   
            curr = next;            
        }
        delete el; // delete entrylist
        el = NULL;
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

unsigned int get_number_entries(const entry_list *el)
{
    return el->getEntryNum();
}

ErrorCode add_entry(entry_list *el, entry *e)
{
    try
    {
        el->addEntry(e);
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

entry *get_first(const entry_list *el)
{
    return el->getHead();
}

entry *get_next(const entry_list *el, entry *e)
{
    return el->getNext(e);
}