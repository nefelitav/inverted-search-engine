#ifndef THREAD
#define THREAD

#include <iostream>
#include <pthread.h>
#include "core.h"
#define NUM_THREADS 4

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
        const char* getDocStr() const;
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
        const char* getQueryStr() const;
        const MatchType getMatchType() const;
        const unsigned int getMatchDistance() const;
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
        jobNode* getNext();
        void setNext(jobNode* newJob);
        Job* getJob();
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
        jobNode * getFirst();
        jobNode * getLast();
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

    public:
        JobScheduler(int execution_threads);
        int submit_job(Job *job);
        const int getThreads() const;
        JobQueue * getQueue();
        pthread_t * getThreadIds();
        ~JobScheduler();
};

ErrorCode MatchDocumentJob(void* args);       
ErrorCode StartQueryJob(void *args);
void* threadMain(void *arg);

extern JobScheduler *scheduler;
extern bool globalExit;
extern int unfinishedDocs;
extern int unfinishedQueries;

// parallelization of match document
extern pthread_mutex_t resultLock;             // Mutex protecting resultList
extern pthread_mutex_t queueLock;              // Mutex protecting JobQueue
extern pthread_cond_t queueEmptyCond;          // Condition Variable for empty JobQueue
extern pthread_cond_t resEmptyCond;            // Condition Variable for empty resultList

// parallelization of start query
extern pthread_mutex_t editLock;               // Mutex protecting edit index
extern pthread_mutex_t exactLock;              // Mutex protecting exact table
extern pthread_mutex_t hammingLock[27];        // Mutex protecting hamming indices
extern pthread_mutex_t entrylistLock;          // Mutex protecting entylist
extern pthread_mutex_t queryTableLock;         // Mutex protecting queryTable
extern pthread_mutex_t unfinishedQueriesLock;  // Mutex protecting unfinishedQueries
extern pthread_cond_t unfinishedQueriesCond;   // Condition Variable for unfinished queries

#endif // THREAD