#include <cstring>
#include "test.h"
#include "../crib.h"

int main()
{
    std::cout << "=== Crib Tests ===\n";

    /* simple valid crib at position 0: ABCDE / "abc" -> A=a B=b C=c */
    Crib crib("ABCDE", "abc", 0);
    TEST_ASSERT(crib.is_possible() == 1,    "simple crib at pos 0 is possible");
    TEST_ASSERT(crib.get_position() == 0,   "initial position is 0");
    Fixed_Key fk = crib.get_fixed_key();
    TEST_ASSERT(fk.get_pt('A') == 'a',      "crib: A->a");
    TEST_ASSERT(fk.get_pt('B') == 'b',      "crib: B->b");
    TEST_ASSERT(fk.get_pt('C') == 'c',      "crib: C->c");

    /* move_right to position 1: crib "abc" over BCD */
    crib.move_right();
    TEST_ASSERT(crib.is_possible() == 1,    "crib still possible after move_right");
    TEST_ASSERT(crib.get_position() == 1,   "position is 1 after move_right");

    /* move_left back to position 0 */
    crib.move_left();
    TEST_ASSERT(crib.get_position() == 0,   "position is 0 after move_left");

    /* contradictory crib: AABCD / "xy"
       first 'A' is assigned to 'x', second 'A' must be 'y' – contradiction */
    Crib crib2("AABCD", "xy", 0);
    TEST_ASSERT(crib2.is_possible() == 0,   "contradictory crib is not possible");

    /* ct_has_changed */
    TEST_ASSERT(!crib.ct_has_changed("ABCDE"), "ct_has_changed: false for same text");
    TEST_ASSERT( crib.ct_has_changed("XBCDE"), "ct_has_changed: true for different text");

    /* ct_has_changed must compare against the raw original CT (including spaces/punct),
       not the processed alpha-only version – this guards against the CribDialog bug where
       get_ct() was used instead of get_original_ct() when rebuilding a Crib on each
       keystroke, causing original_ct_ to lose its spaces and making ct_has_changed()
       return true next time on_cribbing was called with the raw CT */
    Crib crib_raw("AB CD", "ab", 0);
    TEST_ASSERT(!crib_raw.ct_has_changed("AB CD"), "ct_has_changed: false for raw CT with spaces");
    TEST_ASSERT( crib_raw.ct_has_changed("ABCD"),  "ct_has_changed: true when spaces stripped from raw CT");

    /* crib longer than or equal to ct at right edge should not be possible */
    Crib crib3("ABC", "abcd", 0);   /* crib longer than ct */
    TEST_ASSERT(crib3.is_possible() == 0,   "crib longer than ct is not possible");

    TEST_SUMMARY();
}
