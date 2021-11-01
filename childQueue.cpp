#include "childQueue.hpp"
#include "index.hpp"
using namespace std;
childQueue::childQueue(class index* input,childQueue* ptrToNext){
    this->entry = input;
    this->next = ptrToNext;
}
void childQueue::pop(class index** content, childQueue** newHead){
    *newHead = this->next;
    *content = this->entry;
    delete this;
}

queueHandler::queueHandler(){
    this->headPTR = NULL;
    this->tempPTR = NULL;
}
int queueHandler::enqueue(class index** input){
    if (this->headPTR){
        this->headPTR = new childQueue(*input, this->tempPTR);
        this->tempPTR = this->headPTR;
    }else{
        this->headPTR = new childQueue(*input);
        this->tempPTR = this->headPTR;
    }
    return 0;
}
int queueHandler::dequeue(class index** nodeToReturn){
    if (this->headPTR){
        this->headPTR->pop(nodeToReturn, &this->tempPTR);
        this->headPTR = this->tempPTR;
    }else{
        *nodeToReturn = NULL;
    }
    return 0;
}


