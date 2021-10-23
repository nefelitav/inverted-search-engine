#include <iostream>
#include <string.h>
#include "core.h"
using namespace std;

#ifndef STRUCTS
#define STRUCTS

class Query {
    private:
        char* query_words;                   
        int actualSize;
        QueryID id;
    public:
        Query(char * words, int id)
        {
            this->actualSize = 0;
            this->query_words = new char[MAX_QUERY_LENGTH];
            for (int i = 0; i < MAX_QUERY_LENGTH - 1; i++)
            {   
                if (words[i] + words[i+1] == 0)                                      // end of text
                {
                    this->actualSize ++;
                    memcpy(this->query_words, words, this->actualSize);             // allocate memory and copy words 
                    break;
                }
                this->actualSize ++;                                                // find length of text

            }
            this->id = id;
            cout << "Query with id = " << this->id << " is created!" << endl;
        }
        void printQuery()                                                           // for debuggind reasons
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
            delete[] this->query_words;
            cout << "Query with id = " << this->id << " is deleted!" << endl;
        }    
};

class Document {
    private:
        char* text;          
        int actualSize;
        DocID id;
    public:
        Document(char * words, int id, int size)
        {
            this->actualSize = size - 1;
            this->text = new char[size - 1 - 1];                                    // dont need the two last \0 's
            for (int i = 0; i < size - 1; i++)
            {   
                if (words[i] + words[i+1] == 0)                                     // end of text
                {
                    break;
                }
                if (words[i] == 0)
                {
                    this->text[i] = '\0';                                           // instead of spaces i save \0 in my array
                    continue;
                }
                this->text[i] = words[i];
            }
            this->id = id;
            cout << "Document with id = " << this->id << " is created!" << endl;

        }
        void printDocument()
        {
            for (int j = 0; j < this->actualSize - 1; j++)                          // for debugging reasons
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
            delete[] this->text;
            cout << "Document with id = " << this->id << " is deleted!" << endl;
        }

};

#endif
