#include "../include/structs.hpp"

Query :: Query(QueryID id, char * words, MatchType match_type, unsigned int match_dist)
{
    this->words = new char[MAX_QUERY_LENGTH]();                                // allocate memory and set to zero, to avoid junk
    int i = 0, length = 0;
    const char* c = " ";                                                       // delimiter
    word token = strtok((char *)words, c);

    while (token != NULL)                               // get all words from input but not more than MAX_QUERY_WORDS
    {
        length = strlen(token) + 1;                                            // length of each word
        words += length;                                                       // pointer to input
        if (*words == '\0')                                                    // no more words in input
        {
            memcpy(this->words + (MAX_WORD_LENGTH + 1)*i, token, length); 
            break;
        }
        memcpy(this->words + (MAX_WORD_LENGTH + 1)*i, token, length); 
        i++;    
        token = strtok(NULL, c);
    }
    this->entriesNum = i;                                                     // save number of entries
    this->matchedEntries = new bool[this->entriesNum];                          // every cell corresponds to an entry

    for (i = 0; i < this->entriesNum; i++)
    {
        this->matchedEntries[i] = false;                                        // set by default to false
    }
    this->id = id;
    this->match_type = match_type;
    this->match_dist = match_dist;
    //std::cout << "Query with id = " << this->id << " is created!" << std::endl;
}

void Query :: printQuery() const                                               // for debugging
{
    std::cout << "-------------------" << std::endl;
    std::cout << "Print query words :" << std::endl;
    for (int i = 0; i < MAX_QUERY_WORDS; i++)
    {
        if (*(this->words + (MAX_WORD_LENGTH + 1) * i) == '\0')
        {
            return;
        }
        std::cout << this->words + (MAX_WORD_LENGTH + 1)*i << std::endl;
        
    }
    std::cout << "-------------------" << std::endl;
}
const QueryID Query :: getId() const                                                             
{
    return this->id;
}
const MatchType Query :: getMatchingType() const                                                             
{
    return this->match_type;
}
const unsigned int Query :: getMatchingDistance() const                                                             
{
    return this->match_dist;
}
const word Query :: getWord(int word_num) const                             // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]                                      
{
    if (word_num >= this->entriesNum)                                        // out of range   
    {
        //std::cout << "Sorry, index out of range." << std::endl;
        return NULL;
    }
    const word ptr = (this->words + word_num * (MAX_WORD_LENGTH + 1));      // find the word
    if (*ptr == '\0')                                                       // no more words
    {
        //std::cout << "Sorry, index out of range." << std::endl;
        return NULL;
    }
    return ptr;
}
void Query :: setTrue(const word entry_word)                                // entry matched with a doc word
{
    for (int i = 0; i < this->entriesNum; i++)
    {
        if (strcmp(this->getWord(i), entry_word) == 0)
        {
            this->matchedEntries[i] = true;
            break;
        }
    }
}
void Query :: setFalse()                                                    // set to default to start a new search with new doc
{
    for (int i = 0; i < this->entriesNum; i++)
    {
        this->matchedEntries[i] = false;
    }
}
bool Query :: matched()                                                     // check if all entries matched with doc words
{
    int sum = 0;
    for (int i = 0; i < this->entriesNum; i++)
    {
        sum += this->matchedEntries[i];
    }
    return (sum == this->entriesNum);
}
void Query :: printMatchedEntries()                                         // for debugging
{
    for (int i = 0; i < this->entriesNum; i++)
    {
        std::cout << this->matchedEntries[i] << std::endl;
    }
}
bool* Query :: getMatchedEntries()
{
    return this->matchedEntries;
}

const int Query :: getWordNum() const
{
    return this->entriesNum;
}

const char* Query :: getText() const                                        // for debugging
{
    return this->words;
}
Query :: ~Query()
{
    delete[] this->matchedEntries;
    delete[] this->words;
    //std::cout << "Query with id = " << this->id << " is deleted!" << std::endl;
}    


////////////////////////////////////////////////////////////////////////////////////



Document :: Document(DocID id, char * words)  
{
    this->text = new char[MAX_DOC_LENGTH]();                                // allocate memory and set to zero, to avoid junk
    int i = 0, length = 0;
    const char* c = " ";                                                    // delimiter
    word token = strtok((char *)words, c);
    while (token != NULL)                              // get all words from input but not more than MAX_QUERY_WORDS
    {
        length = strlen(token) + 1;                                         // length of each word
        words += length;                                                    // pointer to input
        if (*words == '\0')                                                 // no more words in input
        {
            memcpy(this->text + (MAX_WORD_LENGTH + 1)*i, token, length);                                             
            break;
        }
        memcpy(this->text + (MAX_WORD_LENGTH + 1)*i, token, length);                                             
        i++;  
        token = strtok(NULL, c);
    }
    this->id = id;
    //std::cout << "Document with id = " << this->id << " is created!" << std::endl;

}
void Document :: printDocument() const                                          // for debugging 
{
    std::cout << "-------------------" << std::endl;
    std::cout << "Print text words :" << std::endl;

    for (int i = 0; i < MAX_DOC_WORDS; i++)
    {
        if (*(this->text + (MAX_WORD_LENGTH + 1) * i) == '\0')
        {
            return;
        }
        std::cout << this->text + (MAX_WORD_LENGTH + 1)*i << std::endl;
        
    }
}
const word Document :: getWord(int word_num) const                              // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]    
{
    if (word_num >= MAX_DOC_WORDS)                                              // out of range
    {
        //cout << "Sorry, index out of range." << endl;
        return NULL;
    }
    const word ptr = (this->text + word_num * (MAX_WORD_LENGTH + 1));           // find the word  
    if (*ptr == '\0')
    {
        //cout << "Sorry, index out of range." << endl;
        return NULL;
    }
    return ptr;
}
const int Document :: getWordNum() const
{
    int count = 0;
    for (int i = 0; i < MAX_DOC_WORDS; i++)
    {
        if (*(this->text + (MAX_WORD_LENGTH + 1) * i) == '\0')
        {
            return count;
        }
        count++;
        
    }
    return count;
}
char* Document :: getText() const
{
    return this->text;
}
Document :: ~Document()
{
    delete[] this->text;
    //std::cout << "Document with id = " << this->id << " is deleted!" << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////


payloadNode :: payloadNode(QueryID id, unsigned int threshold, payloadNode* next) 
{

    this->id = id;
    this->threshold = threshold;
    this->next = next;
}

const QueryID payloadNode :: getId() const
{
    return this->id;
}

const unsigned int payloadNode :: getThreshold() const
{
    return this->threshold;
}

payloadNode *payloadNode :: getNext()
{
    return this->next;
}

void payloadNode :: setNext(payloadNode *newNext)
{
    this->next = newNext;
}


entry :: entry(const word keyword)
{
    if (keyword == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    this->keyword = new char[MAX_WORD_LENGTH];
    strcpy(this->keyword, keyword);
    this->payload = NULL;
    this->next = NULL;
    //std::cout << "Entry is created!" << std::endl;
}

const word entry :: getWord() const
{
    return this->keyword;
}

entry* entry :: getNext() const
{
    return this->next;
}

void entry :: setNext(entry* e)
{
    if (this->next == NULL)
    {
        this->next = e;
    }
}

payloadNode* entry :: getPayload() const
{
    return this->payload;
}

void entry :: addToPayload(QueryID id, unsigned int threshold)
{
    if (this->emptyPayload()) 
    {
        this->payload = new payloadNode(id, threshold);
    } 
    else 
    {   
        payloadNode* new_head = new payloadNode(id, threshold);
        payloadNode* old_head = this->payload;
        new_head->setNext(old_head);
        this->payload = new_head;
    } 
}

void entry :: printPayload()
{
    payloadNode* curr = this->payload;
    while (curr != NULL) 
    {
        std::cout << curr->getId() << " ";
        curr = curr->getNext();
    }
    std::cout << std::endl;
}

bool entry :: emptyPayload() 
{
    return (this->payload == NULL);
}

void entry :: deletePayloadNode(QueryID id) 
{
    payloadNode *curr = this->payload;
    payloadNode *next = curr->getNext();
    if (curr->getId() == id)
    {
        delete curr;
        this->payload = next;
        return;
    }
    while (curr != NULL)
    {
        if (curr->getNext()->getId() == id)
        {
            curr->setNext(curr->getNext()->getNext());
            delete curr->getNext();
            return;
        }
        curr = curr->getNext();
    }
}

entry :: ~entry()
{
    //std::cout <<"------------------------------" << this->keyword << std::endl;
    payloadNode *prev,*curr = this->payload;    // delete payload
    while (curr != NULL) 
    {
        prev = curr;
        curr = curr->getNext();
        delete prev;
        prev = NULL;
    }
    delete[] this->keyword;                     // delete word
    //std::cout << "Entry is deleted!" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////


entry_list :: entry_list()
{
    this->head = NULL;
    this->entryNum = 0;
    //std::cout << "Entry list is created!" << std::endl;
}
unsigned int entry_list :: getEntryNum() const
{
    return this->entryNum;
}
entry* entry_list :: getHead() const
{
    return this->head;
}
void entry_list :: addEntry(entry * new_entry)
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
    this->entryNum++;
}
void entry_list :: printList()                        
{
    entry * curr = this->head;                               
    entry * next = NULL;

    while (curr != NULL)                                        
    {
        next = curr->getNext();                                 
        std::cout << curr->getWord() << std::endl;                                    
        curr->printPayload();
        curr = next;                                            
    }
}

entry* entry_list :: getNext(entry* e) const
{
    return e->getNext();
}

void entry_list :: removeEntry(entry * toRemove)
{
    entry * curr = this->head;                               
    entry * next = NULL;

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

}
entry_list :: ~entry_list()
{
    //std::cout << "Entry list is deleted!" << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////


ErrorCode create_entry(const word* w, entry** e)
{
    try 
    {
        *e = new entry(*w);
        //cout << (**e).getWord() << endl;
        return EC_SUCCESS;
    } 
    catch (const std::exception& _) 
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
    catch (const std::exception& _) 
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode create_entry_list(entry_list** el)
{
    try 
    {
        *el = new entry_list();
        return EC_SUCCESS;
    } 
    catch (const std::exception& _) 
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry_list(entry_list* el)                        // first delete entries and then list
{
    try 
    {
        entry * curr = el->getHead();                               // pointer to head of list
        entry * next = NULL;

        while (curr != NULL)                                        // if null -> end of list
        {
            next = curr->getNext();                                 // save next entry
            destroy_entry(curr);                                    // delete entry
            curr = next;                                            // go to next entry
        }
        delete el;                                                  // delete entrylist
        el = NULL;
        return EC_SUCCESS;
    } 
    catch (const std::exception& _) 
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

unsigned int get_number_entries(const entry_list* el)
{
    return el->getEntryNum();
}

ErrorCode add_entry(entry_list* el, entry* e)
{
    try 
    {
        el->addEntry(e);
        return EC_SUCCESS;
    } 
    catch (const std::exception& _) 
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

entry* get_first(const entry_list* el)
{
    return el->getHead();
}

entry* get_next(const entry_list* el, entry* e)
{
    return el->getNext(e);
}

