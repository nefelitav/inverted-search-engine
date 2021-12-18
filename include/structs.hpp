#ifndef STRUCTS
#define STRUCTS

#include <cstring>
#include "index.hpp"

typedef char* word;
class payloadNode;

class Query {
    private:
        //char* words;                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)            
        char words[MAX_QUERY_LENGTH];
        QueryID id;
        MatchType match_type;
        unsigned int match_dist;
        bool* matchedEntries;
        int entriesNum;
    public:
        Query(QueryID id, char * words, MatchType match_type, unsigned int match_dist);
        void printQuery() const;                                                            
        const word getWord(int word_num) const;                                                         
        const QueryID getId() const;                                                             
        const int getWordNum() const;
        const MatchType getMatchingType() const;                                                             
        const unsigned int getMatchingDistance() const;     
        void setTrue(const word entry_word);
        void setFalse();
        void printMatchedEntries();
        bool* getMatchedEntries();
        bool matched();
        ~Query();
          
};

class Document {
    private:
        char* text[MAX_DOC_LENGTH];                 // saving words in a 1d array instead of a 2d to save time (contiguous allocation)   
        DocID id;
    public:
        Document(DocID id, char * words); 
        void printDocument() const;
        const word getWord(int word_num) const;                      
        ~Document();
};


class payloadNode {
    private:
        QueryID id;
        unsigned int threshold;
        payloadNode* next;
    public:
        payloadNode(QueryID id, unsigned int threshold, payloadNode* next = NULL);
        const QueryID getId() const;
        const unsigned int getThreshold() const;
        payloadNode* getNext();
        void setNext(payloadNode* newNext);
};


class entry {
    private:
        char keyword[MAX_WORD_LENGTH];
        payloadNode* payload;
        entry* next;
    public:
        entry(const word keyword);
        const word getWord() const;
        payloadNode* getPayload() const;
        entry* getNext() const;
        void setNext(entry* e);
        void addToPayload(QueryID id, unsigned int threshold);
        bool emptyPayload();
        void printPayload();
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
