#include "../include/structs.hpp"
#include "../include/core.h"
#include "../include/utilities.hpp"
#include "../include/index.hpp"

// some globals
entry_list *EntryList;
QueryTable *QT;
EntryTable *ET;
indexNode *editIndex;
indexNode **hammingIndexes;
void *exactHash;
result* resultList;

ErrorCode InitializeIndex()
{
    try
    {
        create_entry_list(&EntryList);
        QT = new QueryTable();
        ET = new EntryTable();
        editIndex = new indexNode(NULL);
        hammingIndexes = new indexNode*[27];
        for (int i = 0; i < 27; i++)
        {
            hammingIndexes[i] = new indexNode(NULL,1 ,0, MT_HAMMING_DIST);
        }
        exactHash = NULL;
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
}

ErrorCode StartQuery(QueryID query_id, const char *query_str, MatchType match_type, unsigned int match_dist)
{
    try
    {
        Query *q = new Query(query_id, (char *)query_str, match_type, match_dist);  // create query
        QT->addToBucket(hashFunctionById(query_id), q);                             // store it in hash table
        int i = 0;
        entry *tempEntry;
        word w = q->getWord(0);                                                     // get every entry of query
        if (w == NULL)
        {
            return EC_FAIL;
        }
        while (w != NULL)
        {
            create_entry(&w, &tempEntry);
            EntryList->addEntry(tempEntry);
            tempEntry->addToPayload(query_id, match_dist);
            addToIndex(tempEntry, query_id, match_type, match_dist);                // add entry to the appropriate index
            i++;
            w = q->getWord(i);
        }
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode DestroyIndex()
{
    try
    {
        delete editIndex;
        for (int i = 0; i < 27; i++)
        {
            delete hammingIndexes[i];
        }
        delete[] hammingIndexes;
        destroy_entry_list(EntryList);
        delete QT;
        delete ET;
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode EndQuery(QueryID query_id)
{
    try
    {
        QT->deleteQuery(query_id);      // delete query from hash table and its entries from index
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode MatchDocument(DocID doc_id, const char *doc_str)
{
    
    try
    {   
        Document *d = new Document(doc_id, (char *)doc_str);    // create document
        DocTable *DT;
        DT = new DocTable(doc_id);
        DocumentDeduplication(d, DT);                           // deduplicate document
        DT->wordLookup();                                       // check for match
        delete d;
        delete DT;
        
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}
ErrorCode GetNextAvailRes(DocID *p_doc_id, unsigned int *p_num_res, QueryID **p_query_ids)
{
    result *temp = NULL;
    if (resultList == NULL)
    {
        return EC_NO_AVAIL_RES;
    }
    else
    {
        *p_doc_id = resultList->getDocID();
        *p_num_res = resultList->getNumRes();
        *p_query_ids = resultList->getQueries();
        temp = resultList;
        resultList = resultList->getNext();
        delete temp;
        return EC_SUCCESS;
    }
}