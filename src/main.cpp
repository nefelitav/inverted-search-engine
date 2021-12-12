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
    std::string line;
    std::ifstream myfile;
    myfile.open("./data/input.txt");
    char *q_words;
    char *d_words;
    int q_count = 0, d_count = 0, doc = 0;

    InitializeIndex();
    if (myfile.is_open())
    {
        while (std::getline(myfile, line)) // read each line
        {
            if (line.empty()) // no more queries
            {
                if (doc == 1) // end of file
                {
                    break;
                }
                doc = 1; // reading documents from now on
                continue;
            }
            if (doc == 0)
            {
                q_words = new char[strlen(line.c_str()) + 1]();
                strcpy(q_words, line.c_str());
                StartQuery(q_count, q_words, MT_HAMMING_DIST, 0);
                delete[] q_words;
                q_count++;
            }
            else
            {
                //editIndex->printTree();
                d_words = new char[strlen(line.c_str()) + 1]();
                strcpy(d_words, line.c_str());
                MatchDocument(d_count, d_words);
                delete[] d_words;
                d_count++;
            }
        }
        myfile.close();
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
        return -1;
    }
    // EndQuery(0);
    // EndQuery(1);
    // EndQuery(2);
    // EndQuery(3);
    // EndQuery(4);

    // ET->printTable();
    // QT->printTable();

    // const word w = d[0]->getWord(1);
    // const word w2 = d[0]->getWord(2);
    // entry *e = NULL;
    // entry *e2 = NULL;
    // entry_list *el = NULL;
    // entry_list *result = NULL;
    // create_entry_list(&el);
    // create_entry(&w, &e);
    // create_entry(&w2, &e2);
    // add_entry(el, e);
    // add_entry(el, e2);
    // std::cout << e->getWord() << std::endl;
    // std::cout << e2->getWord() << std::endl;
    // std::cout << get_number_entries(el) << std::endl;
    // std::cout << get_first(el)->getWord() << std::endl;
    // std::cout << get_next(el, e2)->getWord() << std::endl;

    // addToIndex(&e, 1, MT_EDIT_DIST, 1);
    // addToIndex(&e2, 1, MT_EDIT_DIST, 1);
    // // Create the list that will store the search results
    // create_entry_list(&result);

    // // The search term
    // const word search_term = (char *)"ipsum";

    // //Search the tree
    // lookup_entry_index(&search_term, result, MT_EDIT_DIST);

    // // Present the result(s)
    // entry *result_node = result->getHead();
    // std::cout << "Search Results for " << search_term << ":\n";
    // while (result_node)
    // {
    //     std::cout << result_node->getWord() << "\n";
    //     result_node = result_node->getNext();
    // }

    // // Destroy_entries and entrylist
    // destroy_entry_list(el);
    // destroy_entry_list(result);

    DestroyIndex();
}