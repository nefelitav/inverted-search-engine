#ifndef THREAD
#define THREAD

#include <iostream>
#include <pthread.h>
#include "core.h"
#define NUM_THREADS 10

typedef ErrorCode (*fptr)(void *param);

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
        jobNode *firstNode;
        jobNode *lastNode;
        unsigned currSize;
    public:
        JobQueue();
        int getSize();
        bool isEmpty();
        void push(Job *job);
        jobNode *pop();
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
        int execute_all_jobs();
        int wait_all_tasks_finish();
        const int getThreads() const;
        JobQueue * getQueue();
        pthread_t * getThreadIds();
        ~JobScheduler();
};

ErrorCode simpleJob(void *givenInt);
void* threadMain(void *arg);

extern JobScheduler *scheduler;
extern bool globalExit;
extern pthread_mutex_t queueLock;
extern pthread_mutex_t emptyLock;
extern pthread_cond_t emptyQueueCond;

#endif // THREAD