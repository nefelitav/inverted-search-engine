#include "../include/thread.hpp"
void *threadMain(void *arg)
{
    std::cout << "Started Thread\n";
    std::cout << "Thread Done\n";
    return NULL;
}

///////////////////////////////// Queue /////////////////////////////////
Queue::Queue(Job *job)
{
    this->next = NULL;
    this->job = job;
}

Queue *Queue::getNext()
{
    return this->next;
}
Job *Queue::getJob()
{
    return this->job;
}

void Queue::setNext(Queue *newNext)
{
    this->next = newNext;
}
///////////////////////////////// Job /////////////////////////////////

Job::Job(ErrorCode (*job)(void *), void *args)
{
    this->job = job;
    this->args = args;
}

fptr Job::getFunc()
{
    return this->job;
}

void *Job::getArgs()
{
    return this->args;
}

///////////////////////////////// JobScheduler /////////////////////////////////

JobScheduler::JobScheduler(int execution_threads)
{
    std::cout<<"Created Scheduler\n";
    globalExit = false;
    this->execution_threads = execution_threads;
    this->q = NULL;
    this->back = NULL;
    this->tids = new pthread_t[execution_threads];
    for (int i = 0; i < this->execution_threads; i++)
    {
        pthread_create(&(this->tids[i]), NULL, &threadMain, NULL);
    }
}

JobScheduler::~JobScheduler()
{
    globalExit = true;
    for (int i = 0; i < this->execution_threads; i++)
    {
        pthread_join(this->tids[i],NULL);
        std::cout<<"Killed Thread\n";
    }
    delete[] this->tids;
}

int JobScheduler::submit_job(Job *j)
{
    if (this->back == NULL)
    {
        this->back = new Queue(j);
        this->q = back;
    }
    else
    {
        Queue *newQueueNode = new Queue(j);
        this->back->setNext(newQueueNode);
    }
    return 0;
}

Job *JobScheduler::popJob()
{
    Queue *temp;
    if (this->q == NULL)
    {
        return NULL;
    }
    else
    {
        return this->q->getJob();
        if (this->q == this->back)
        {
            delete this->q;
            this->q = NULL;
            this->back = NULL;
        }
        temp = this->q;
        this->q = this->q->getNext();
        delete temp;
    }
}