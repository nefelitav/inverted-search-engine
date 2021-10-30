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

