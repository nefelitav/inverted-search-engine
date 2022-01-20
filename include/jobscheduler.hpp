#ifndef THREAD
#define THREAD

#include <iostream>
#include <pthread.h>
#include "utilities.hpp"
#include "core.h"

#define NUM_THREADS 3

class QueryTable;

typedef enum {
    MATCH_DOCS,
    QUERIES_CREATION,
    QUERIES_DELETION
} WaitTask;

typedef enum
{
    START_QUERY,
    END_QUERY,
    MATCH_DOCUMENT,
    INITIALIZE_INDEX
} OperationType;

typedef ErrorCode (*fptr)(void *param);

// these classes help represent the arguments of each job as a pointer to void
class MatchDocumentArgs
{
private:
    DocID doc_id;
    char doc_str[MAX_DOC_LENGTH];

public:
    MatchDocumentArgs(DocID doc_id, const char *doc_str);
    const DocID getDocId() const;
    const char *getDocStr() const;
};

class StartQueryArgs
{
private:
    QueryID query_id;
    char query_str[MAX_QUERY_LENGTH];
    MatchType match_type;
    unsigned int match_dist;

public:
    StartQueryArgs(QueryID query_id, const char *query_str, MatchType match_type, unsigned int match_dist);
    const QueryID getQueryId() const;
    const char *getQueryStr() const;
    const MatchType getMatchType() const;
    const unsigned int getMatchDistance() const;
};

class EndQueryArgs
{
    private:
        QueryID query_id;
    public:
        EndQueryArgs(QueryID query_id);
        const QueryID getQueryId() const;
};

class Job
{
private:
    ErrorCode (*job)(void *);
    void *args;

public:
    Job(ErrorCode (*job)(void *), void *args);
    void *getArgs();
    fptr getFunc();
};

class jobNode
{
private:
    Job *job;
    jobNode *next;

public:
    jobNode(Job *job);
    jobNode *getNext();
    void setNext(jobNode *newJob);
    Job *getJob();
    ~jobNode();
};

class JobQueue
{
private:
    jobNode *first;
    jobNode *last;
    unsigned int currSize;

public:
    JobQueue();
    unsigned int getSize();
    jobNode *getFirst();
    jobNode *getLast();
    bool isEmpty();
    void push(Job *job);
    jobNode *pop();
    void printQueue();
    ~JobQueue();
};

class JobScheduler
{
private:
    int execution_threads;
    JobQueue *q;
    pthread_t *tids;
    QueryTable** localQT;

public:
    JobScheduler(int execution_threads); // initialize_scheduler
    int submit_job(Job *job);
    int wait_all_tasks_finish(WaitTask task);
    const int getThreads() const;
    JobQueue *getQueue();
    pthread_t *getThreadIds();
    int convertId(pthread_t givenID);
    QueryTable* getQueryTable(int threadId);
    void cloneLocalQT();
    ~JobScheduler();
};

ErrorCode MatchDocumentJob(void *args);
ErrorCode StartQueryJob(void *args);
ErrorCode EndQueryJob(void *args);
void *execute_all_jobs(void *arg);

extern JobScheduler *scheduler;
extern bool globalExit;
extern int unfinishedDocs;
extern int unfinishedQueries;
extern int queriesToDelete;
extern OperationType previousOperation;

// parallelization of match document
extern pthread_mutex_t resultLock;            // protecting resultList
extern pthread_mutex_t queueLock;             // protecting JobQueue
extern pthread_mutex_t previousOperationLock; // protecting previousOperation global var
extern pthread_cond_t queueEmptyCond;         // condition of empty JobQueue
extern pthread_cond_t resEmptyCond;           // condition of empty resultList

// parallelization of start/end query
extern pthread_mutex_t entrylistLock;          // protecting entylist
extern pthread_mutex_t unfinishedQueriesLock;  // protecting unfinishedQueries global var
extern pthread_cond_t unfinishedQueriesCond;   // condition of not yet inititialized queries
extern pthread_mutex_t queriesToDeleteLock;    // protecting queriesToDelete global var
extern pthread_cond_t queriesToDeleteCond;     // condition of not yet deleted queries
extern pthread_mutex_t editLock;               // protecting edit index
extern pthread_mutex_t exactLock;              // protecting entry hash table
extern pthread_mutex_t hammingLock[27];        // protecting hamming indices

#endif // THREAD