#include "../include/structs.hpp"
#include "../include/utilities.hpp"
#include "../include/index.hpp"
#include "../include/core.h"
#include <fstream>
#include <string>
#include <iostream>

// create documents and queries, reading words from a file
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

    const word w = d[0]->getWord(1);
    const word w2 = d[0]->getWord(2);

    entry* e = NULL;
    entry* e2 = NULL;
    entry_list* el = NULL;
    entry_list* result = NULL;

    create_entry_list(&el);
    create_entry(&w, &e);
    create_entry(&w2, &e2);

    add_entry(el, e);
    add_entry(el, e2);

    std::cout << e2->getWord() << std::endl;
    std::cout << get_number_entries(el) << std::endl;
    std::cout << get_first(el)->getWord() << std::endl;
    std::cout << get_next(el, e2)->getWord() << std::endl;

    // Create the index
    indexNode* tree;
    build_entry_index(el, MT_EDIT_DIST, &tree);

    // Create the list that will store the search results
    create_entry_list(&result);

    // The search term
    const word search_term=(char*)"ipsum";

    //Search the tree
    lookup_entry_index(&search_term, tree, 0, result);

    // Present the result(s)
    entry* result_node = result->getHead();
    std::cout<<"Search Results for "<<search_term<<":\n";
    while(result_node){
        std::cout<<result_node->getWord()<<"\n";
        result_node = result_node->getNext();
    }
    destroy_entry_index(tree);

    // Destroy_entries and entrylist
    destroy_entry_list(el);
    destroy_entry_list(result);


    // Delete queries and documents
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