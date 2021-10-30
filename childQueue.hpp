#ifndef CHILD_QUEUE
#define CHILD_QUEUE

#include "index.hpp"

class childQueue{
    private:
        class index* entry;
        childQueue* next;
    public:
        childQueue(class index* input, childQueue* ptrToNext = nullptr);
        void pop(class index** content, childQueue** newHead);
};

#endif // CHILD_QUEUE