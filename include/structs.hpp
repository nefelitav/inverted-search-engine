#ifndef STRUCTS
#define STRUCTS

#include <iostream>
#include <cstring>
#include "core.h"
#include "index.hpp"

typedef char* word;
class payloadNode;

class Query {
    private:
        char* words;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)            
        QueryID id;
        MatchType match_type;
        unsigned int match_dist;
    public:
        Query(QueryID id, char * words, MatchType match_type, unsigned int match_dist);
        void printQuery() const;                                                            
        const word getWord(int word_num) const;                                                         
        const char* getText() const;
        const QueryID getId() const;                                                             
        const int get_word_num() const;
        const MatchType getMatchingType() const;                                                             
        const unsigned int getMatchingDistance() const;                                                             
        ~Query();
          
};

class Document {
    private:
        char* text;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)   
        DocID id;
    public:
        Document(char * words, QueryID id); 
        void printDocument() const;
        const word getWord(int word_num) const;                      
        char* getText() const;
        const int get_word_num() const;
        ~Document();
};

class entry {
    private:
        word keyword;
        payloadNode* payload;
        entry* next;
    public:
        entry(const word keyword);
        const word getWord() const;
        payloadNode* getPayload() const;
        entry* getNext() const;
        void setNext(entry* e);
        void addToPayload(QueryID id, unsigned int threshold);
        bool EmptyPayload();
        void deletePayloadNode(QueryID id);
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
        void printList();    
        void removeEntry(entry * toRemove);
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
