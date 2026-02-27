#include <cstring>
#include "test.h"
#include "../hash.h"

int main()
{
    std::cout << "=== Hash Table Tests ===\n";

    /* construct */
    hash_table table;
    construct_table(&table, 200);
    TEST_ASSERT(table.size == 200,       "construct_table sets size");
    TEST_ASSERT(table.table != NULL,     "construct_table allocates table");

    /* insert + lookup */
    char key1[]  = "hello";
    char data1[] = "world";
    insert(key1, data1, &table);
    TEST_ASSERT(lookup(key1, &table) == (void*)data1,
                "lookup finds inserted key");

    /* lookup missing key */
    char missing[] = "missing";
    TEST_ASSERT(lookup(missing, &table) == NULL,
                "lookup returns NULL for missing key");

    /* del then lookup */
    del(key1, &table);
    TEST_ASSERT(lookup(key1, &table) == NULL,
                "lookup returns NULL after del");

    /* duplicate insert returns old data and updates entry */
    char key2[]  = "foo";
    char data2[] = "bar";
    char data3[] = "baz";
    insert(key2, data2, &table);
    void* old_data = insert(key2, data3, &table);
    TEST_ASSERT(old_data == (void*)data2,
                "insert returns old data on duplicate key");
    TEST_ASSERT(lookup(key2, &table) == (void*)data3,
                "lookup returns updated data after re-insert");

    free_table(&table, NULL);

    TEST_SUMMARY();
}
