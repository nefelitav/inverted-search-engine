#include "../include/structs.hpp"
#include "../include/core.h"
#include "../include/utilities.hpp"
#include "../include/index.hpp"
#include "../include/jobscheduler.hpp"

// some globals
entry_list *EntryList;
QueryTable *QT;
EntryTable *ET;
indexNode *editIndex;
indexNode **hammingIndexes;
void *exactHash;
result *resultList;
bool globalExit;
JobScheduler *scheduler;
int unfinishedDocs = 0;
int unfinishedQueries = 0;

pthread_mutex_t resultLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t unfinishedQueriesLock = PTHREAD_MUTEX_INITIALIZER;

ErrorCode InitializeIndex()
{
    try
    {
        pthread_mutex_lock(&queueLock);        
        unfinishedDocs = 0;
        scheduler = new JobScheduler(NUM_THREADS);
        create_entry_list(&EntryList);        // Create the global entry list
        QT = new QueryTable();                // Create the global query and entry hash tables
        ET = new EntryTable();
        editIndex = new indexNode(NULL);      // Create the edit index with a null node

        hammingIndexes = new indexNode *[27]; // Create the 27 hamming indexes for each word length from 4 to 31
        for (int i = 0; i < 27; i++)
        {
            hammingIndexes[i] = new indexNode(NULL, 1, 0, MT_HAMMING_DIST);
        }
        exactHash = NULL;
        pthread_mutex_unlock(&queueLock);
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
        pthread_mutex_lock(&unfinishedQueriesLock);
        unfinishedQueries++;
        pthread_mutex_unlock(&unfinishedQueriesLock);
        StartQueryArgs *args = new StartQueryArgs(query_id, query_str, match_type, match_dist); // create a class with the job args
        ErrorCode (*job)(void *) = StartQueryJob;
        Job *jobToAdd = new Job(job, (void *)args); // create job
        pthread_mutex_lock(&queueLock);
        scheduler->submit_job(jobToAdd); // push job to queue
        pthread_mutex_unlock(&queueLock);
        pthread_cond_signal(&queueEmptyCond); // no more empty -> can continue executing jobs
    
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
        unfinishedDocs++;
        pthread_mutex_lock(&unfinishedQueriesLock);
        if (unfinishedQueries != 0)
        {
            pthread_cond_wait(&unfinishedQueriesCond, &unfinishedQueriesLock);
        }
        pthread_mutex_unlock(&unfinishedQueriesLock);
        
        MatchDocumentArgs *args = new MatchDocumentArgs(doc_id, doc_str); // create a class with the job args
        ErrorCode (*job)(void *) = MatchDocumentJob;
        Job *jobToAdd = new Job(job, (void *)args); // create job
        pthread_mutex_lock(&queueLock);
        scheduler->submit_job(jobToAdd); // push job to queue
        pthread_mutex_unlock(&queueLock);
        pthread_cond_signal(&queueEmptyCond); // no more empty -> can continue executing jobs

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
    pthread_mutex_lock(&resultLock);
    if (unfinishedDocs != 0 || resultList == NULL) // wait until all match document jobs are done and the result list is not empty
    {
        pthread_cond_wait(&resEmptyCond, &resultLock);
    }
    result *temp = NULL;
    if (resultList == NULL)
    {
        pthread_mutex_unlock(&resultLock);
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
        pthread_mutex_unlock(&resultLock);
        return EC_SUCCESS;
    }
}

ErrorCode EndQuery(QueryID query_id)
{
    try
    {
        pthread_mutex_lock(&unfinishedQueriesLock);  // wait until all start query jobs are done -> otherwise we might delete something that is not allocated yet
        if (unfinishedQueries != 0)
        {
            pthread_cond_wait(&unfinishedQueriesCond, &unfinishedQueriesLock);
        }
        pthread_mutex_unlock(&unfinishedQueriesLock);
        pthread_mutex_lock(&queryTableLock);        // protect query table, which is a global class
        QT->deleteQuery(query_id);                  // delete query from hash table and its entries from index
        pthread_mutex_unlock(&queryTableLock);
        
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
        globalExit = true;
        // unblock all threads blocked on the condition variables
        pthread_cond_broadcast(&queueEmptyCond);
        pthread_cond_broadcast(&resEmptyCond);
        pthread_cond_broadcast(&unfinishedQueriesCond);
        delete editIndex;
        for (int i = 0; i < 27; i++)
        {
            delete hammingIndexes[i];
        }
        delete[] hammingIndexes;
        destroy_entry_list(EntryList);
        delete QT;
        delete ET;
        delete scheduler;
        return EC_SUCCESS;
    }
    catch (const std::exception &_)
    {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}