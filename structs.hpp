#include <iostream>
#include <cstring>
#include "core.h"

using namespace std;

#ifndef STRUCTS
#define STRUCTS

typedef char* word;

class Query {
    private:
        char* words;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)            
        QueryID id;
    public:
        Query(const char * words, int id);
        void printQuery();                                                            
        const word getWord(int word_num);                                                         
        char* getText();
        ~Query();
          
};

class Document {
    private:
        char* text;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)   
        DocID id;
    public:
        Document(const char * words, int id); 
        void printDocument();
        const word getWord(int word_num);                      
        char* getText();
        ~Document();
};

class entry {
    private:
        word keyword;
        void* payload;
        entry* next;
    public:
        entry(const word keyword);
        const word getWord();
        void* getPayload();
        entry* getNext();
        void setNext(entry* e);
        ~entry();
};

class entry_list {
    private:
        entry * head;
        int entryNum;
    public:
        entry_list();
        unsigned int getEntryNum();
        entry* getHead();
        void addEntry(entry * new_entry);
        entry* getNext(entry* e);
        ~entry_list();
};


ErrorCode create_entry(const word* w, entry** e);
ErrorCode destroy_entry(entry *e);
ErrorCode create_entry_list(entry_list** el);
ErrorCode destroy_entry_list(entry_list* el);
unsigned int get_number_entries(entry_list* el);
ErrorCode add_entry(entry_list* el, entry* e);
entry* get_first(entry_list* el);
entry* get_next(entry_list* el, entry* e);

/*
ErrorCode build_entry_index(const entry_list* el, MatchType type, index* ix);
ErrorCode lookup_entry_index(const word* w, index* ix, int threshold, entry_list* result);
ErrorCode destroy_entry_index(index* ix);
*/

#endif
