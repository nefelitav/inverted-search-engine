#include "structs.hpp"
#include "functions.hpp"
#include "index.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include "./include/core.h"

int main()
{
    /*
    std::string line;
    std::ifstream myfile ("input.txt");
    char *q_words;
    int count = 0;
    if (myfile.is_open())
    {
        char * token;
        while ( std::getline (myfile, line) )
        {
            q_words = new char[MAX_QUERY_LENGTH];
            strcpy(q_words, line.c_str());
            std::cout << q_words << std::endl;
            Query q(q_words, count);
            delete[] q_words;
            count ++;

            // token = strtok ((char*)line.c_str()," ");
            // while( token != NULL ) {
            //     cout << token << endl;
            //     token = strtok(NULL, " ");
            }
        }
        myfile.close();
    }
    */
    char *q_words = new char[MAX_QUERY_LENGTH];
    memcpy(q_words, "hello\0world\0how\0are\0you\0", MAX_QUERY_LENGTH);
    Query q(q_words, 1);
    q.printQuery();
    std::cout << q.getText() << std::endl;
    std::cout << q.getWord(3) << std::endl;
    delete[] q_words;
    const char* s = "hello\0 world\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0";
    int i = 0;
    while(s[i] != '\0' || s[i+1] != '\0')
    {
        i++;
    }
    char *d_words = new char[i + 1 + 1]; // 1 -> for the two /0 in the end
    memcpy(d_words, s, i + 1 + 1); 
    Document d(d_words, 1);
    Deduplication(&d);
    d.printDocument();
    std::cout << d.getText() << std::endl;
    std::cout << d.getWord(4) << std::endl;

    const word w = d.getWord(1);
    const word w2 = d.getWord(2);

    entry* e = NULL;
    entry* e2 = NULL;
    entry_list* el = NULL;
    
    create_entry_list(&el);
    create_entry(&w, &e);
    create_entry(&w2, &e2);

    add_entry(el, e);
    add_entry(el, e2);

    std::cout << e2->getWord() << std::endl;
    std::cout << get_number_entries(el) << std::endl;
    std::cout << get_first(el)->getWord() << std::endl;
    std::cout << get_next(el, e2)->getWord() << std::endl;

    //destroy_entry(e);
    //destroy_entry(e2);
    destroy_entry_list(el);
    delete[] d_words;


}