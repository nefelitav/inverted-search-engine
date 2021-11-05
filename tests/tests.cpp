#include "tests.hpp"

void test_Query(void)
{
    char *q_words = new char[MAX_QUERY_LENGTH];
    memcpy(q_words, "hello\0world\0how\0are\0you\0", MAX_QUERY_LENGTH);
    Query q(q_words, 1);
    TEST_ASSERT((*(q.getText()) == 'h'));
    TEST_ASSERT(strcmp(q.getWord(0),"hello") == 0);
    TEST_ASSERT(strcmp(q.getWord(1),"world") == 0);
    TEST_ASSERT(strcmp(q.getWord(2),"how") == 0);
    TEST_ASSERT(strcmp(q.getWord(3),"are") == 0);

    delete[] q_words;
}

void test_Document(void)
{
    const char* s = "hello\0 world\0 how\0 are\0 you\0";
    int i = 0;
    while(s[i] != '\0' || s[i+1] != '\0')
    {
        i++;
    }
    char *d_words = new char[i + 1 + 1]; // 1 -> for the two /0 in the end
    memcpy(d_words, s, i + 1 + 1); 
    Document d(d_words, 1);
    TEST_ASSERT(strcmp(d.getText(),"hello") == 0);
    TEST_ASSERT(strcmp(d.getWord(0),"hello") == 0);
    TEST_ASSERT(strcmp(d.getWord(1),"world") == 0);
    TEST_ASSERT(strcmp(d.getWord(2),"how") == 0);
    TEST_ASSERT(strcmp(d.getWord(3),"are") == 0);

    delete[] d_words;

}

// void test_create_hash_table(void) {}

TEST_LIST = {
    {"test_Query", test_Query} ,
    {"test_Document", test_Document} ,
    //{ "test_create_hash_table", test_create_hash_table },
    { NULL, NULL }
};