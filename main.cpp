#include "structs.hpp"

int main()
{
    const char *w = "hello\0world\0how\0are\0you\0";
    Query q(w, 1);
    q.printQuery();

    const char *w2 = "hello world how are you\0";
    Document d(w2, 2);
    d.printDocument();
    
    // const char * f = "hello world wup all ok friend wup olleh";
    // Deduplication(f);
}