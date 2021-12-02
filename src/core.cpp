#include "../include/structs.hpp"
#include "../include/core.h"
#include "../include/utilities.hpp"
#include "../include/index.hpp"

// some globals
entry_list* EntryList; 
QueryTable* QT;
indexNode* editIndex;
indexNode** hammingIndexes;
void* exactHash;

ErrorCode InitializeIndex(){
    try{
        create_entry_list(&EntryList);
        QT = new QueryTable();
        // editIndex = new indexNode(NULL);
        // hammingIndexes = new indexNode*[27];
        // for (int i = 0; i < 27; i++) {
        //     hammingIndexes[i] = new indexNode(NULL, MT_HAMMING_DIST);
        // }
        exactHash = NULL;
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
    
}

ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
{
    try {
        //HashTable* QHT;
        //QHT = new HashTable();
        Query* q = new Query(query_id, (char*)query_str, match_type, match_dist);
        QT->addToBucket(hashFunctionById(query_id), q);
        //QueryDeduplication(q, QHT);
        //QHT->addWordsToIndex();
        //QHT->printTable();
        int i = 0;
        entry* e = NULL;
        word w = q->getWord(0);
        if (w == NULL)
        {
            return EC_FAIL;
        }
        while (w != NULL)
        {   
            create_entry(&w, &e);
            add_entry(EntryList, e);
            //add entry to index    
            addToIndex(&e, query_id, match_type);
            i++;
            w = q->getWord(i);
        }
        //delete QHT;
        //delete q;                       // delete querytable instead
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}
                
ErrorCode DestroyIndex()
{
    try {
        delete QT;
        destroy_entry_list(EntryList);
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode EndQuery(QueryID query_id)
{
    try {
        QT->deleteQuery(query_id);
        return EC_SUCCESS;
    } catch (const std::exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}
// ErrorCode MatchDocument(DocID         doc_id,
                        // const char*   doc_str);
// ErrorCode GetNextAvailRes(DocID*         p_doc_id,
                        // unsigned int*  p_num_res,
                        // QueryID**      p_query_ids);