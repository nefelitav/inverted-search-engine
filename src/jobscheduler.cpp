#include "../include/jobscheduler.hpp"

pthread_mutex_t queueLock;
pthread_mutex_t emptyLock;
pthread_cond_t emptyQueueCond = PTHREAD_COND_INITIALIZER;

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
    std::cout << "Started Thread\n";
    while (1)
    { // Bad practice, TODO replace with do while...

        pthread_mutex_lock(&queueLock);
        
        if (scheduler->getQueue()->isEmpty() && globalExit)
        {
            pthread_mutex_unlock(&queueLock);
            break;
        }
        if (scheduler->getQueue()->isEmpty())
        {
            pthread_cond_wait(&emptyQueueCond, &emptyLock);
        }
        else
        {
            job = scheduler->getQueue()->pop();
            job->getJob()->getFunc()(job->getJob()->getArgs());
            pthread_cond_signal(&emptyQueueCond);
        }

        pthread_mutex_unlock(&queueLock);
        delete job;
    }
    std::cout << "Thread Done\n";
    return NULL;
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
    this->firstNode = NULL;
    this->lastNode = NULL;
    this->currSize = 0;
}

int JobQueue ::getSize()
{
    return currSize;
}

bool JobQueue ::isEmpty()
{
    return (currSize == 0);
}

void JobQueue ::push(Job *job)
{
    if (this->lastNode == NULL)
    {
        // empty queue
        this->lastNode = new jobNode(job);
        this->firstNode = this->lastNode;
    }
    else
    {
        jobNode *newJob = new jobNode(job);
        this->lastNode->setNext(newJob);
        this->lastNode = this->lastNode->getNext();
    }
    this->currSize++;
}

jobNode *JobQueue ::pop()
{
    if (this->currSize == 0)
    {
        return NULL;
    }
    jobNode *toReturn = this->firstNode;
    this->firstNode = this->firstNode->getNext();
    this->currSize--;
    return toReturn;
}

JobQueue ::~JobQueue()
{
    jobNode *curr = this->firstNode;
    while (curr != NULL)
    {
        jobNode *next = curr->getNext();
        delete curr;
        curr = next;
    }
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

//////////////////////// JobScheduler ////////////////////////////

JobScheduler ::JobScheduler(int execution_threads)
{
    std ::cout << "Created Scheduler\n";
    globalExit = false;
    this->q = new JobQueue();
    this->execution_threads = execution_threads;
    this->tids = new pthread_t[execution_threads];
    for (int i = 0; i < this->execution_threads; i++)
    {
        pthread_create(&(this->tids[i]), NULL, &threadMain, NULL);
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
    globalExit = true;
    for (int i = 0; i < this->execution_threads; i++)
    {
        pthread_join(this->tids[i], NULL);
        std ::cout << "Killed Thread\n";
    }
    delete this->q;
    delete[] this->tids;
}