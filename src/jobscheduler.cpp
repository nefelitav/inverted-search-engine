#include "../include/jobscheduler.hpp"
#include "../include/structs.hpp"
#include "../include/utilities.hpp"
#include <sys/types.h>
#include <cstring>

// initialize mutexes, condition variables
pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t entrylistLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueEmptyCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t resEmptyCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t unfinishedQueriesCond = PTHREAD_COND_INITIALIZER;

///////////////////////////////// Match Document Job /////////////////////////////////

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
    DocTable *DT = new DocTable(((MatchDocumentArgs *)args)->getDocId());
    DocumentDeduplication(d, DT); // deduplicate document
    DT->wordLookup();             // check for match
    delete d;
    delete DT;
    delete (MatchDocumentArgs *)args;
    return EC_SUCCESS;
}

///////////////////////////////// Start Query Job /////////////////////////////////

StartQueryArgs ::StartQueryArgs(QueryID query_id, const char *query_str, MatchType match_type, unsigned int match_dist)
{
    this->query_id = query_id;
    memcpy(this->query_str, query_str, MAX_QUERY_LENGTH);
    this->match_type = match_type;
    this->match_dist = match_dist;
}

const QueryID StartQueryArgs ::getQueryId() const
{
    return this->query_id;
}

const char *StartQueryArgs ::getQueryStr() const
{
    return this->query_str;
}

const MatchType StartQueryArgs ::getMatchType() const
{
    return this->match_type;
}

const unsigned int StartQueryArgs ::getMatchDistance() const
{
    return this->match_dist;
}

ErrorCode StartQueryJob(void *args)
{

    Query *q = new Query(((StartQueryArgs *)args)->getQueryId(), (char *)(((StartQueryArgs *)args)->getQueryStr()), ((StartQueryArgs *)args)->getMatchType(), ((StartQueryArgs *)args)->getMatchDistance()); // create query
    QT->addToBucket(hashFunctionById(((StartQueryArgs *)args)->getQueryId()), q); // store it in hash table
    int i = 0;
    entry *tempEntry;
    word w = q->getWord(0); // get every entry of query
    if (w == NULL)
    {
        return EC_FAIL;
    }
    while (w != NULL)
    {
        create_entry(&w, &tempEntry);
        pthread_mutex_lock(&entrylistLock); // protect entry list which is a global class
        EntryList->addEntry(tempEntry);
        pthread_mutex_unlock(&entrylistLock);
        tempEntry->addToPayload(((StartQueryArgs *)args)->getQueryId(), ((StartQueryArgs *)args)->getMatchDistance());
        addToIndex(tempEntry, ((StartQueryArgs *)args)->getQueryId(), ((StartQueryArgs *)args)->getMatchType(), ((StartQueryArgs *)args)->getMatchDistance()); // add entry to the appropriate index
        i++;
        w = q->getWord(i);
    }

    pthread_mutex_lock(&unfinishedQueriesLock);
    unfinishedQueries--; // start query procedure is done for this query and we can move on to MatchDocument or EndQuery
    if (unfinishedQueries == 0)
    {
        pthread_cond_signal(&unfinishedQueriesCond); // all queries are done and we can move on
    }
    pthread_mutex_unlock(&unfinishedQueriesLock);
    delete (StartQueryArgs *)args;
    return EC_SUCCESS;
}

///////////////////////////////// Thread Main /////////////////////////////////

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
                //std::cout << "Thread Done\n";
                return NULL;
            }
            pthread_cond_wait(&queueEmptyCond, &queueLock);
        }
        job = scheduler->getQueue()->pop(); // Execute Job
        pthread_mutex_unlock(&queueLock);
        job->getJob()->getFunc()(job->getJob()->getArgs());

        delete job;
    }
    //std::cout << "Thread Done\n";
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

unsigned int JobQueue ::getSize()
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
    this->q = new JobQueue(); // create queue
    this->execution_threads = execution_threads;
    this->tids = new pthread_t[execution_threads];
    this->localQT = new QueryTable *[this->execution_threads];
    for (int i = 0; i < this->execution_threads; i++) // create threads
    {
        this->localQT[i] = NULL;
        pthread_create(&this->tids[i], NULL, &threadMain, NULL);
    }
}

int JobScheduler ::submit_job(Job *job)
{
    pthread_mutex_lock(&queueLock);
    this->q->push(job);
    pthread_cond_signal(&queueEmptyCond); // no more empty -> can continue executing jobs
    pthread_mutex_unlock(&queueLock);
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

QueryTable *JobScheduler::getQueryTable(int id)
{
    return this->localQT[id];
}

int JobScheduler::convertId(pthread_t givenID)
{
    for (int i = 0; i < this->execution_threads; i++)
    {
        if (this->tids[i] == givenID)
        {
            return i;
        }
    }
    return -1;
}

void JobScheduler::cloneLocalQT()
{
    for (int i = 0; i < this->execution_threads; i++)
    {
        delete this->localQT[i];
        this->localQT[i] = QT->cloneQueryTable();
    }
}

JobScheduler ::~JobScheduler()
{
    for (int i = 0; i < this->execution_threads; i++)
    {
        pthread_join(this->tids[i], NULL);
        delete this->localQT[i];
    }
    delete this->q;
    delete[] this->tids;
    delete[] this->localQT;
}