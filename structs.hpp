#include <iostream>
#include <cstring>
#include "core.h"


using namespace std;

#ifndef STRUCTS
#define STRUCTS

class Query {
    private:
        char* words;                   
        QueryID id;
    public:
        Query(char * words, int id)
        {
            this->words = new char[MAX_QUERY_LENGTH];
            memset(this->words, 0, MAX_QUERY_LENGTH);
            char * ptr = words;
            for (int i = 0; i < MAX_QUERY_WORDS; i++)
            {
                memcpy(this->words + (MAX_WORD_LENGTH + 1)*i, ptr, strlen(ptr) + 1);
                ptr += (strlen(ptr) + 1);
                if (*(ptr - 1) == '\0' && *ptr == '\0')
                {
                    break;
                }
            }
            this->id = id;
            cout << "Query with id = " << this->id << " is created!" << endl;
        }
        void printQuery()                                                           // for debuggind reasons
        {
            char * ptr = this->words;
            cout << "-------------------" << endl;
            cout << "Print query words :" << endl;
            for (int i = 0; i < MAX_QUERY_WORDS; i++)
            {
                cout << ptr << endl;
                ptr += (MAX_WORD_LENGTH + 1);
                if (i != MAX_QUERY_WORDS - 1)
                {
                    if (*(ptr - 1) == '\0' && *(ptr) == '\0')
                    {
                        break;
                    }
                }
                
            }
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
        char* text;          
        DocID id;
    public:
        Document(char * words, int id)
        {
            this->text = new char[MAX_DOC_LENGTH];                                    // dont need the two last \0 's
            memset(this->text, 0, MAX_DOC_LENGTH);
            char * ptr = words;
            for (int i = 0; i < MAX_DOC_WORDS; i++)
            {
                memcpy(this->text + (MAX_WORD_LENGTH + 1)*i, ptr, strlen(ptr) + 1);
                ptr += (strlen(ptr) + 1 + 1);
                if (*(ptr - 1) == '\0' && *(ptr - 2) == '\0')
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
                ptr += (MAX_WORD_LENGTH + 1);
                if (*(ptr - 1) == '\0' && *(ptr) == '\0')
                {
                    break;
                }
            }
            cout << "-------------------" << endl;
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

#endif
