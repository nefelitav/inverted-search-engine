#ifndef STRUCTS
#define STRUCTS

#include <iostream>
#include <cstring>
#include "core.h"

typedef char* word;

class Query {
    private:
        char* words;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)            
        QueryID id;
    public:
        Query(const char * words, int id);
        void printQuery() const;                                                            
        const word getWord(int word_num) const;                                                         
        char* getText() const;
        ~Query();
          
};

class Document {
    private:
        char* text;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)   
        DocID id;
    public:
        Document(const char * words, int id); 
        void printDocument() const;
        const word getWord(int word_num) const;                      
        char* getText() const;
        ~Document();
};

class entry {
    private:
        word keyword;
        void* payload;
        entry* next;
    public:
        entry(const word keyword);
        const word getWord() const;
        void* getPayload() const;
        entry* getNext() const;
        void setNext(entry* e);
        ~entry();
};

class entry_list {
    private:
        entry * head;
        int entryNum;
    public:
        entry_list();
        unsigned int getEntryNum() const;
        entry* getHead() const;
        void addEntry(entry * new_entry);
        entry* getNext(entry* e) const;
        ~entry_list();
};


ErrorCode create_entry(const word* w, entry** e);
ErrorCode destroy_entry(entry *e);
ErrorCode create_entry_list(entry_list** el);
ErrorCode destroy_entry_list(entry_list* el);
unsigned int get_number_entries(const entry_list* el);
ErrorCode add_entry(entry_list* el, entry* e);
entry* get_first(const entry_list* el);
entry* get_next(const entry_list* el, entry* e);


#endif