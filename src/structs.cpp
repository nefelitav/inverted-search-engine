#include "../include/structs.hpp"
Query :: Query(QueryID id, char * words, MatchType match_type, unsigned int match_dist)
{
    if (words == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    this->words = new char[MAX_QUERY_LENGTH]();                                // allocate memory and set to zero, to avoid junk
    int i = 0, length = 0;
    const char* c = " ";                                                       // delimiter
    word token = strtok((char *)words, c);

    while (i < MAX_QUERY_WORDS && token != NULL)                               // get all words from input but not more than MAX_QUERY_WORDS
    {
        length = strlen(token) + 1;                                            // length of each word
        words += length;                                                       // pointer to input
        if (*words == '\0')                                                    // no more words in input
        {
            if (length < MAX_WORD_LENGTH && length > MIN_WORD_LENGTH)
            {
                memcpy(this->words + (MAX_WORD_LENGTH + 1)*i, token, length); 
            }
            break;
        }
        if (length < MAX_WORD_LENGTH && length > MIN_WORD_LENGTH)
        {
            memcpy(this->words + (MAX_WORD_LENGTH + 1)*i, token, length); 
            i++;    
        }
        token = strtok(NULL, c);
    }
    this->id = id;
    this->match_type = match_type;
    this->match_dist = match_dist;

    //std::cout << "Query with id = " << this->id << " is created!" << std::endl;
}
void Query :: printQuery() const                                               // for debugging reasons
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
    if (word_num >= MAX_QUERY_WORDS)                                        // out of range   
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

const int Query :: get_word_num() const
{
    int count = 0;
    for (int i = 0; i < MAX_QUERY_WORDS; i++)
    {
        if (*(this->words + (MAX_WORD_LENGTH + 1) * i) == '\0')
        {
            return count;
        }
        count++;
        
    }
    return count;
}

const char* Query :: getText() const                                        // for debugging reasons
{
    return this->words;
}
Query :: ~Query()
{
    delete[] this->words;
    //std::cout << "Query with id = " << this->id << " is deleted!" << std::endl;
}    


////////////////////////////////////////////////////////////////////////////////////



Document :: Document(char * words, QueryID id)  
{
    if (words == NULL)
    {
        throw std::invalid_argument("Got NULL pointer");
    }
    this->text = new char[MAX_DOC_LENGTH]();                                // allocate memory and set to zero, to avoid junk
    int i = 0, length = 0;
    const char* c = " ";                                                    // delimiter
    word token = strtok((char *)words, c);
    while (i < MAX_DOC_WORDS && token != NULL)                              // get all words from input but not more than MAX_QUERY_WORDS
    {
        length = strlen(token) + 1;                                         // length of each word
        words += length;                                                    // pointer to input
        if (*words == '\0')                                                 // no more words in input
        {
            if (length < MAX_WORD_LENGTH && length > MIN_WORD_LENGTH)
            {
                memcpy(this->text + (MAX_WORD_LENGTH + 1)*i, token, length);                                             
            }
            break;
        }
        if (length < MAX_WORD_LENGTH && length > MIN_WORD_LENGTH)
        {
            memcpy(this->text + (MAX_WORD_LENGTH + 1)*i, token, length);                                             
            i++;  
        }
        token = strtok(NULL, c);
    }
    this->id = id;
    //std::cout << "Document with id = " << this->id << " is created!" << std::endl;

}
void Document :: printDocument() const
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
const int Document :: get_word_num() const
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
    payloadNode* temp;
    if (this->EmptyPayload()) 
    {
        this->payload = new payloadNode(id, threshold);
    } 
    else if (this->payload->getId() > id) 
    {
        temp = this->payload;
        this->payload = new payloadNode(id, threshold, temp);
    } 
    else if (this->payload->getId() == id) 
    {
        throw std::invalid_argument("Query already in payload");
    } 
    else {
        this->payload->addNode(id,threshold);
    }
}

bool entry :: EmptyPayload() 
{
    return (this->payload == NULL);
}

void entry :: deletePayloadNode(QueryID id) 
{
    payloadNode* temp, *curr = this->payload;
    if (this->payload == NULL) 
    {
        std::cout<<"ID NOT FOUND\n";
    } else if (this->payload != NULL) 
    {
        if (payload->getId() == id) 
        {
            temp = this->payload;
            this->payload = temp->getNext();
            delete temp;
            if (this->payload != NULL) 
            {
                curr = payload->getNext();
                temp = this->payload;
            } else {
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
        } else {
            temp = curr;
            curr = curr->getNext();
        }
    }
}

entry :: ~entry()
{
    payloadNode *prev,*curr = this->payload;
    while (curr != NULL) 
    {
        prev = curr;
        curr = curr->getNext();
        delete prev;
        prev = NULL;
    }
    delete[] this->keyword;
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
        //cout << this->head->getWord() << endl;
    }
    else
    {
        new_entry->setNext(this->head);
        this->head = new_entry;
        //cout << this->head->getWord() << endl;
        //cout << this->head->getNext()->getWord() << endl;
    }
    this->entryNum++;

}
void entry_list :: printList()                        
{
    entry * curr = this->head;                               
    entry * next = NULL;

    while (curr != NULL)                                        
    {
        next = curr->getNext();                                 // save next entry
        std::cout << curr->getWord() << std::endl;                                    // print entry
        curr = next;                                            // go to next entry
    }
}

entry* entry_list :: getNext(entry* e) const
{
    return e->getNext();
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

