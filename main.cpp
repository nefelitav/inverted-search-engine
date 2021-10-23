#include "structs.hpp"

int main()
{
    char *words = new char[MAX_QUERY_LENGTH];
    memcpy(words, "hello\0world\0how\0are\0you\0", MAX_QUERY_LENGTH);
    Query q(words, 1);
    q.printQuery();
    delete[] words;

    char *words2 = new char[strlen("hello world how are you\0") + 1 + 1]; // 1 -> for the two /0 in the end
    memcpy(words2, "hello world how are you\0", strlen("hello world how are you\0") + 1 + 1);
    Document d(words2, 2, strlen("hello world how are you\0") + 1 + 1);
    d.printDocument();
    delete[] words2;

    
    // const char * f = "hello world wup all ok friend wup olleh";
    // Deduplication(f);
}