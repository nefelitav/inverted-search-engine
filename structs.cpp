#include "structs.hpp"


Query :: Query(const char * words, int id)
{
    this->words = new char[MAX_QUERY_LENGTH]();                                // allocate memory and set to zero
    //memset(this->words, 0, MAX_QUERY_LENGTH);                                // set to zero, to avoid junk
    const char * ptr = words;                                                  // pointer to input 
    for (int i = 0; i < MAX_QUERY_WORDS; i++)                                 
    {
        memcpy(this->words + (MAX_WORD_LENGTH + 1)*i, ptr, strlen(ptr) + 1);  // copy from input to struct
        ptr += (strlen(ptr) + 1);                                             // move pointer
        if (*(ptr - 1) == '\0' && *ptr == '\0')                               // no more words in input
        {
            break;
        }
    }
    this->id = id;
    std::cout << "Query with id = " << this->id << " is created!" << std::endl;
}
void Query :: printQuery() const                                               // for debugging reasons
{
    char * ptr = this->words;
    std::cout << "-------------------" << std::endl;
    std::cout << "Print query words :" << std::endl;
    for (int i = 0; i < MAX_QUERY_WORDS; i++)
    {
        std::cout << ptr << std::endl;
        ptr += (MAX_WORD_LENGTH + 1);                                       // go to next word
        if (i != MAX_QUERY_WORDS - 1)                                       // if last word, exit
        {
            if (*(ptr - 1) == '\0' && *(ptr) == '\0')                       // no more words
            {
                break;
            }
        }
        
    }
    std::cout << "-------------------" << std::endl;
}
const word Query :: getWord(int word_num) const                  // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]                                      
{
    if (word_num >= MAX_QUERY_WORDS)                                         // out of range   
    {
        std::cout << "Sorry, index out of range." << std::endl;
        return NULL;
    }
    const word ptr = (this->words + word_num * (MAX_WORD_LENGTH + 1));      // find the word
    if (*ptr == '\0')                                                       // no more words
    {
        std::cout << "Sorry, index out of range." << std::endl;
        return NULL;
    }
    return ptr;
}

char* Query :: getText() const
{
    return this->words;
}
Query :: ~Query()
{
    delete[] this->words;
    std::cout << "Query with id = " << this->id << " is deleted!" << std::endl;
}    


///////////////////////////////////////////////////////////////////////////////////////////////////////



Document :: Document(const char * words, int id)  
{
    this->text = new char[MAX_DOC_LENGTH]();                                   // allocate memory and set to zero
    //memset(this->text, 0, MAX_DOC_LENGTH);                                   // set to zero, to avoid junk
    const char * ptr = words;
    for (int i = 0; i < MAX_DOC_WORDS; i++)
    {
        memcpy(this->text + (MAX_WORD_LENGTH + 1)*i, ptr, strlen(ptr) + 1);   // copy from input to struct
        ptr += (strlen(ptr) + 1 + 1);                                         // go to next word
        if (*(ptr - 1) == '\0' && *(ptr - 2) == '\0')                         // no more words in input
        {
            break;
        }
    }
    this->id = id;
    std::cout << "Document with id = " << this->id << " is created!" << std::endl;

}
void Document :: printDocument() const
{
    char * ptr = this->text;
    std::cout << "-------------------" << std::endl;
    std::cout << "Print text words :" << std::endl;
    for (int i = 0; i < MAX_DOC_WORDS; i++)                                 
    {
        std::cout << ptr << std::endl;
        ptr += (MAX_WORD_LENGTH + 1);                                         // move pointer
        if (*(ptr - 1) == '\0' && *(ptr) == '\0')                             // no more words
        {
            break;
        }
    }
    std::cout << "-------------------" << std::endl;
}
const word Document :: getWord(int word_num) const                 // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]    
{
    if (word_num >= MAX_DOC_WORDS)                                                // out of range
    {
        //cout << "Sorry, index out of range." << endl;
        return NULL;
    }
    const word ptr = (this->text + word_num * (MAX_WORD_LENGTH + 1));            // find the word          
    if (*ptr == '\0')
    {
        //cout << "Sorry, index out of range." << endl;
        return NULL;
    }
    return ptr;
}
char* Document :: getText() const
{
    return this->text;
}
Document :: ~Document()
{
    delete[] this->text;
    std::cout << "Document with id = " << this->id << " is deleted!" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////



entry :: entry(const word keyword)
{
    this->keyword = new char[MAX_WORD_LENGTH];
    strcpy(this->keyword, keyword);
    this->payload = NULL;
    this->next = NULL;
    std::cout << "Entry is created!" << std::endl;
}
const word entry :: getWord() const
{
    return this->keyword;
}
void* entry :: getPayload() const
{
    return this->payload;
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
entry :: ~entry()
{
    delete[] this->keyword;
    std::cout << "Entry is deleted!" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////



entry_list :: entry_list()
{
    this->head = NULL;
    this->entryNum = 0;
    std::cout << "Entry list is created!" << std::endl;
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
entry* entry_list :: getNext(entry* e) const
{
    return e->getNext();
}
entry_list :: ~entry_list()
{
    std::cout << "Entry list is deleted!" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////




ErrorCode create_entry(const word* w, entry** e)
{
    try {
        *e = new entry(*w);
        //cout << (**e).getWord() << endl;
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry(entry *e)
{
    try {
        delete e;
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode create_entry_list(entry_list** el)
{
    try {
        *el = new entry_list();
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry_list(entry_list* el) // first delete entries and then list
{
    try {
        entry * curr = el->getHead();       // pointer to head of list
        entry * next = NULL;

        while ( curr != NULL)               // if null -> end of list
        {
            next = curr->getNext();         // save next entry
            destroy_entry(curr);            // delete entry
            curr = next;                    // go to next entry
        }
        delete el;                          // delete entrylist
        return EC_SUCCESS;
    } catch (const std::exception& _) {
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
    try {
        el->addEntry(e);
        return EC_SUCCESS;
    } catch (const std::exception& _) {
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

