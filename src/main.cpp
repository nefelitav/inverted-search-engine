#include "../include/structs.hpp"
#include "../include/utilities.hpp"
#include "../include/index.hpp"
#include "../include/core.h"
#include <fstream>
#include <string>
#include <iostream>

int main()
{
    Query** q = new Query*[NUM_QUERIES];                         // array of queries - static 
    Document** d = new Document*[NUM_DOCS];                      // array of docs - static 
    std::string line;
    std::ifstream myfile;
    myfile.open("./data/input.txt");
    char *q_words;
    char *d_words;
    int q_count = 0, d_count = 0, doc = 0;
    HashTable* HT;
    if (myfile.is_open())
    {
        while ( std::getline(myfile, line) )                    // read each line 
        {
            if ( line.empty() )                                 // no more queries
            {
                if (doc == 1)                                   // end of file
                {
                    break;
                }
                doc = 1;                                        // reading documents from now on
                continue;
            }
            if (doc == 0)
            {
                q_words = new char[strlen(line.c_str()) + 1]();       
                strcpy(q_words, line.c_str());
                q[q_count] = new Query(q_words, q_count);               // new query
                //q[q_count]->printQuery();
                delete[] q_words;
                q_count ++;
            }
            else
            {
                HT = new HashTable();
                d_words = new char[strlen(line.c_str()) + 1]();       
                strcpy(d_words, line.c_str());
                d[d_count] = new Document(d_words, d_count);            // new doc
                //d[d_count]->printDocument();
                Deduplication(d[d_count], HT)->printTable();
                delete[] d_words;
                delete HT;
                d_count ++;
            }
            
        }
        myfile.close();
    }
    else
    {
        std::cout << "Unable to open file" << std::endl; 
        return -1;
    }
    // delete queries and documents
    for (int i = 0; i < NUM_QUERIES; i++)
    {
        delete q[i];
    }
    delete[] q;
    for (int i = 0; i < NUM_DOCS; i++)
    {
        delete d[i];
    }
    delete[] d;
}