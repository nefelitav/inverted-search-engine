#ifndef THREAD
#define THREAD

#include <iostream>
#include <pthread.h>
#include "core.h"
#define NUM_THREADS 4

typedef ErrorCode (*fptr)(void *param);

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
        unsigned currSize;
    public:
        JobQueue();
        int getSize();
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
ErrorCode simpleJob(void *givenInt);
void* threadMain(void *arg);

extern JobScheduler *scheduler;
extern bool globalExit;
extern int unfinishedDocs;
extern pthread_mutex_t resultLock;          // Mutex protecting resultList
extern pthread_mutex_t queueLock;           // Mutex protecting JobQueue
extern pthread_mutex_t strTokLock;           // Mutex protecting strTok access during Document Creation
extern pthread_mutex_t queryTableLock;      // Mutex protecting queryTable
extern pthread_cond_t queueEmptyCond;       // Condition Variable for empty JobQueue
extern pthread_cond_t resEmptyCond;         // Condition Variable for empty resultList


#endif // THREAD