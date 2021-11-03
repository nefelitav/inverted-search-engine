#include "tests.hpp"

void test_exact_match(void)
{
    TEST_ASSERT(exactMatch("hello","hello") == 0);
}
// void test_create_hash_table(void) {}

TEST_LIST = {
    {"test_exact_match", test_exact_match} ,
    //{ "test_create_hash_table", test_create_hash_table },
    { NULL, NULL }
};