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

class Queue
{
private:
    Queue *next;
    Job *job;

public:
    Queue(Job *job);
    Queue *getNext();
    Job *getJob();
    void setNext(Queue *newNext);
};

class JobScheduler
{
private:
    int execution_threads;
    Queue *q;
    Queue *back;
    pthread_t *tids;

public:
    JobScheduler(int execution_threads);
    ~JobScheduler();
    int submit_job(Job *j);
    int execute_all_jobs();
    int wait_all_tasks_finish();
    Job *popJob();
};

void* threadMain(void *arg);

extern JobScheduler *scheduler;
extern bool globalExit;

#endif // THREAD