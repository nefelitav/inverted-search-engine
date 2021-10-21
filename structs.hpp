#include <iostream>
#include <string.h>
#include "core.h"
using namespace std;

#ifndef STRUCTS
#define STRUCTS

class Query {
    private:
        char query_words[MAX_QUERY_LENGTH];                   
        int actualSize;
        QueryID id;
    public:
        Query(const char * words, int id)
        {
            this->actualSize = 0;
            for (int i = 0; i < MAX_QUERY_LENGTH; i++)
            {   
                if (words[i-1] + words[i] == 0) 
                {
                    memcpy(this->query_words, words,this->actualSize);
                    break;
                }
                this->actualSize ++;
            }
            this->id = id;
            cout << "Query with id = " << this->id << " is created!" << endl;
        }
        void printQuery()
        {
            for (int j = 0; j < this->actualSize; j++)
            {
                cout << this->query_words[j] << endl;
            }
        }
        int getQuerySize()
        {
            return this->actualSize;
        }
        ~Query()
        {
            cout << "Query with id = " << this->id << " is deleted!" << endl;
        }    
};

class Document {
    private:
        char text[MAX_DOC_LENGTH - MAX_DOC_WORDS];          // this struct wont be saving the spaces obviously , so i remove them
        int actualSize;
        DocID id;
    public:
        Document(const char * words, int id)
        {
            this->actualSize = 0;
            for (int i = 0; i < MAX_DOC_LENGTH; i++)
            {   
                if (words[i-1] + words[i] == 0) 
                {
                    break;
                }
                this->actualSize ++;
                if (words[i] == 0)
                {
                    this->text[i] = '\0';                   // instead of spaces i save \0 in my array
                    continue;
                }
                this->text[i] = words[i];
            }
            this->id = id;
            cout << "Document with id = " << this->id << " is created!" << endl;

        }
        void printDocument()
        {
            for (int j = 0; j < this->actualSize; j++)
            {
                cout << this->text[j] << endl;
            }
        }
        int getDocumentSize()
        {
            return this->actualSize;
        }
        ~Document()
        {
            cout << "Document with id = " << this->id << " is deleted!" << endl;

        }

};

#endif
