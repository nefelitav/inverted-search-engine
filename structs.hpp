#include <iostream>
#include <cstring>
#include "core.h"

using namespace std;

#ifndef STRUCTS
#define STRUCTS

class Query {
    private:
        char* words;     // saving words in a 1d array instead of a 2d to save time (contiguous allocation)            
        QueryID id;
    public:
        Query(char * words, int id)
        {
            this->words = new char[MAX_QUERY_LENGTH];                                 // allocate memory
            memset(this->words, 0, MAX_QUERY_LENGTH);                                 // set to zero, to avoid junk
            char * ptr = words;                                                       // pointer to input 
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
            cout << "Query with id = " << this->id << " is created!" << endl;
        }
        void printQuery()                                                            // for debuggind reasons
        {
            char * ptr = this->words;
            cout << "-------------------" << endl;
            cout << "Print query words :" << endl;
            for (int i = 0; i < MAX_QUERY_WORDS; i++)
            {
                cout << ptr << endl;
                ptr += (MAX_WORD_LENGTH + 1);                                       // go to next word
                if (i != MAX_QUERY_WORDS - 1)                                       // if last word, exit
                {
                    if (*(ptr - 1) == '\0' && *(ptr) == '\0')                       // no more words
                    {
                        break;
                    }
                }
                
            }
        }
        char * getWord(int word)                    // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]                                      
        {
            if (word >= MAX_QUERY_WORDS)                                            // out of range   
            {
                cout << "Sorry, index out of range." << endl;
                return NULL;
            }
            char * ptr = (this->words + word * (MAX_WORD_LENGTH + 1));              // find the word
            if (*ptr == '\0')                                                       // no more words
            {
                cout << "Sorry, index out of range." << endl;
                return NULL;
            }
            return ptr;
        }

        char* getText()
        {
            return this->words;
        }
        ~Query()
        {
            delete[] this->words;
            cout << "Query with id = " << this->id << " is deleted!" << endl;
        }    
};

class Document {
    private:
        char* text;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)   
        DocID id;
    public:
        Document(char * words, int id)  
        {
            this->text = new char[MAX_DOC_LENGTH];                                    // allocate memory
            memset(this->text, 0, MAX_DOC_LENGTH);                                    // set to zero, to avoid junk
            char * ptr = words;
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
            cout << "Document with id = " << this->id << " is created!" << endl;

        }
        void printDocument()
        {
            char * ptr = this->text;
            cout << "-------------------" << endl;
            cout << "Print text words :" << endl;
            for (int i = 0; i < MAX_DOC_WORDS; i++)                                 
            {
                cout << ptr << endl;
                ptr += (MAX_WORD_LENGTH + 1);                                         // move pointer
                if (*(ptr - 1) == '\0' && *(ptr) == '\0')                             // no more words
                {
                    break;
                }
            }
            cout << "-------------------" << endl;
        }
        char * getWord(int word)                    // array[i][j] --> arrary[i*(MAX_WORD_LENGTH + 1)]    
        {
            if (word >= MAX_DOC_WORDS)                                                // out of range
            {
                cout << "Sorry, index out of range." << endl;
                return NULL;
            }
            char * ptr = (this->text + word * (MAX_WORD_LENGTH + 1));                 // find the word          
            if (*ptr == '\0')
            {
                cout << "Sorry, index out of range." << endl;
                return NULL;
            }
            return ptr;
        }
        char* getText()
        {
            return this->text;
        }
        ~Document()
        {
            delete[] this->text;
            cout << "Document with id = " << this->id << " is deleted!" << endl;
        }
};

class entry {
    private:
        char* word;
        void* payload;
        entry* next;
    public:
        entry(char * word)
        {
            this->word = new char[MAX_WORD_LENGTH];
            strcpy(this->word, word);
            this->payload = NULL;
            this->next = NULL;
            cout << "Entry is created!" << endl;
        }
        char* getWord()
        {
            return this->word;
        }
        void* getPayload()
        {
            return this->payload;
        }
        entry* getNext()
        {
            if (this->next != NULL)
            {
                return this->next;
            }
            return NULL;
        }
        void setNext(entry* e)
        {
            if (this->next == NULL)
            {
                this->next = e;
            }
        }
        ~entry()
        {
            delete[] this->word;
            cout << "Entry is deleted!" << endl;
        }
        
};

class entry_list {
    private:
        entry * head;
        int entryNum;
    public:
        entry_list()
        {
            this->head = NULL;
            this->entryNum = 0;
            cout << "Entry list is created!" << endl;
        }
        unsigned int getEntryNum()
        {
            return this->entryNum;
        }
        entry* getHead()
        {
            return this->head;
        }
        void addEntry(entry * new_entry)
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
        }
        ~entry_list()
        {
            // entry * ptr = this->head;
            // entry * next = NULL;
            // if (ptr->getWord() != NULL)
            // {
            //     cout << "Not yet deleted" << endl;
            // }
            // while (ptr != NULL)
            // {
            //     next = ptr->getNext();
            //     ptr->~entry();
            //     ptr = next;
            // }
            cout << "Entry list is deleted!" << endl;
        }
};

typedef char* word;

ErrorCode create_entry(const word* w, entry** e);
ErrorCode destroy_entry(entry *e);
ErrorCode create_entry_list(entry_list** el);
ErrorCode destroy_entry_list(entry_list* el);
unsigned int get_number_entries(entry_list* el);
ErrorCode add_entry(entry_list* el, entry* e);
entry* get_first(entry_list* el);
entry* get_next(entry_list* el);

/*
ErrorCode build_entry_index(const entry_list* el, MatchType type, index* ix);
ErrorCode lookup_entry_index(const word* w, index* ix, int threshold, entry_list* result);
ErrorCode destroy_entry_index(index* ix);
*/

#endif
