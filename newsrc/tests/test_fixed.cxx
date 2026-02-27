#include "test.h"
#include "../fixed.h"

int main()
{
    std::cout << "=== Fixed_Key Tests ===\n";

    /* default: nothing fixed */
    Fixed_Key fk;
    TEST_ASSERT(fk.number_fixed() == 0,  "default: number_fixed is 0");
    TEST_ASSERT(!fk.is_set('a'),          "default: 'a' is not set");
    TEST_ASSERT(fk.get_ct('a') == ' ',   "default: get_ct('a') returns space");
    TEST_ASSERT(fk.get_pt('A') == ' ',   "default: get_pt('A') returns space");

    /* set 'a' -> 'A' */
    fk.set('a', 'A');
    TEST_ASSERT(fk.is_set('a'),           "is_set true after set('a','A')");
    TEST_ASSERT(fk.get_ct('a') == 'A',   "get_ct('a') returns 'A'");
    TEST_ASSERT(fk.get_pt('A') == 'a',   "get_pt('A') returns 'a'");
    TEST_ASSERT(fk.number_fixed() == 1,  "number_fixed == 1 after one set");

    /* set 'b' -> 'B' */
    fk.set('b', 'B');
    TEST_ASSERT(fk.number_fixed() == 2,  "number_fixed == 2 after two sets");

    /* clear single letter */
    fk.clear('A');
    TEST_ASSERT(!fk.is_set('a'),          "is_set false after clear('A')");
    TEST_ASSERT(fk.number_fixed() == 1,  "number_fixed == 1 after one clear");

    /* clear all */
    fk.clear();
    TEST_ASSERT(fk.number_fixed() == 0,  "number_fixed == 0 after clear()");

    /* re-assigning the same CT letter moves the PT binding */
    fk.set('a', 'A');
    fk.set('b', 'A');  /* 'A' is re-bound from 'a' to 'b' */
    TEST_ASSERT(!fk.is_set('a'),          "old PT unset after CT re-bind");
    TEST_ASSERT(fk.is_set('b'),           "new PT set after CT re-bind");
    TEST_ASSERT(fk.get_pt('A') == 'b',   "get_pt returns new PT after re-bind");

    TEST_SUMMARY();
}
