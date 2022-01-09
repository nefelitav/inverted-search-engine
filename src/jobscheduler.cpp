#include "../include/jobscheduler.hpp"
#include "../include/structs.hpp"
#include "../include/utilities.hpp"
#include <sys/types.h>
#include <cstring>

pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueEmptyCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t resEmptyCond = PTHREAD_COND_INITIALIZER;

MatchDocumentArgs ::MatchDocumentArgs(DocID doc_id, const char *doc_str)
{
    this->doc_id = doc_id;
    memcpy(this->doc_str, doc_str, MAX_DOC_LENGTH);
}

const DocID MatchDocumentArgs ::getDocId() const
{
    return this->doc_id;
}

const char *MatchDocumentArgs ::getDocStr() const
{
    return this->doc_str;
}

ErrorCode MatchDocumentJob(void *args)
{
    Document *d = new Document(((MatchDocumentArgs *)args)->getDocId(), (char *)(((MatchDocumentArgs *)args)->getDocStr())); // create document
    DocTable *DT;
    DT = new DocTable(((MatchDocumentArgs *)args)->getDocId());
    DocumentDeduplication(d, DT); // deduplicate document
    DT->wordLookup();             // check for match
    delete d;
    delete DT;
    delete (MatchDocumentArgs *)args;

    return EC_SUCCESS;
}

ErrorCode simpleJob(void *givenInt)
{
    int *modInt = (int *)givenInt;
    std::cout << *modInt << "\n";
    delete modInt;
    return EC_SUCCESS;
}

void *threadMain(void *arg)
{
    jobNode *job = NULL;
    while (!globalExit)
    {
        pthread_mutex_lock(&queueLock);
        while (scheduler->getQueue()->isEmpty()) // Wait while JobQueue is empty
        {
            if (globalExit) // If program exited while this thread was waiting, exit
            {
                pthread_mutex_unlock(&queueLock);
                return NULL;
            }
            pthread_cond_wait(&queueEmptyCond, &queueLock);
        }
        job = scheduler->getQueue()->pop(); // Execute Job
        pthread_mutex_unlock(&queueLock);
        job->getJob()->getFunc()(job->getJob()->getArgs());

        delete job;
    }
    return NULL;
}

///////////////////////////////// Job /////////////////////////////////

Job ::Job(ErrorCode (*job)(void *), void *args)
{
    this->job = job;
    this->args = args;
}

fptr Job ::getFunc()
{
    return this->job;
}

void *Job ::getArgs()
{
    return this->args;
}

///////////////////////////////// jobNode /////////////////////////////////

jobNode ::jobNode(Job *job)
{
    this->job = job;
    this->next = NULL;
}

jobNode *jobNode ::getNext()
{
    return this->next;
}

void jobNode ::setNext(jobNode *newJob)
{
    this->next = newJob;
}

Job *jobNode::getJob()
{
    return this->job;
}

jobNode ::~jobNode()
{
    delete this->job;
}

///////////////////////////////// jobQueue /////////////////////////////////

JobQueue ::JobQueue()
{
    this->first = NULL;
    this->last = NULL;
    this->currSize = 0;
}

jobNode *JobQueue ::getFirst()
{
    return this->first;
}

jobNode *JobQueue ::getLast()
{
    return this->last;
}

int JobQueue ::getSize()
{
    return this->currSize;
}

bool JobQueue ::isEmpty()
{
    return (this->currSize == 0);
}

void JobQueue ::push(Job *job)
{
    if (this->last == NULL)
    {
        // empty queue
        this->last = new jobNode(job);
        this->first = this->last;
    }
    else
    {
        jobNode *newJob = new jobNode(job);
        this->last->setNext(newJob);
        this->last = this->last->getNext();
    }
    this->currSize++;
}

jobNode *JobQueue ::pop()
{
    if (this->currSize == 0)
    {
        return NULL;
    }
    jobNode *toReturn = this->first;
    if (this->first == this->last)
    {
        this->last = NULL;
    }
    this->first = this->first->getNext();
    this->currSize--;
    return toReturn;
}

void JobQueue ::printQueue()
{
    jobNode *curr = this->first;
    std::cout << "############################################" << std::endl;
    while (curr != NULL)
    {
        jobNode *next = curr->getNext();
        std::cout << ((MatchDocumentArgs *)(curr->getJob()->getArgs()))->getDocId() << std::endl;
        curr = next;
    }
    std::cout << "############################################" << std::endl;
}

JobQueue ::~JobQueue()
{
    jobNode *curr = this->first;
    while (curr != NULL)
    {
        jobNode *next = curr->getNext();
        delete curr;
        curr = next;
    }
}

//////////////////////// JobScheduler ////////////////////////////

JobScheduler ::JobScheduler(int execution_threads)
{
    globalExit = false;
    this->q = new JobQueue();
    this->execution_threads = execution_threads;
    this->tids = new pthread_t[execution_threads];
    for (int i = 0; i < this->execution_threads; i++)
    {
        pthread_create(&this->tids[i], NULL, &threadMain, NULL);
    }
}

int JobScheduler ::submit_job(Job *job)
{
    this->q->push(job);
    return 0;
}

const int JobScheduler ::getThreads() const
{
    return this->execution_threads;
}

JobQueue *JobScheduler ::getQueue()
{
    return this->q;
}

pthread_t *JobScheduler ::getThreadIds()
{
    return this->tids;
}

JobScheduler ::~JobScheduler()
{
    for (int i = 0; i < this->execution_threads; i++)
    {
        pthread_join(this->tids[i], NULL);
    }
    delete this->q;
    delete[] this->tids;
}