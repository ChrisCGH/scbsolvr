#include <cstring>
#include "test.h"
#include "../mono.h"

int main()
{
    std::cout << "=== Mono Tests ===\n";

    char outtext[50];

    /* identity key: 'A'->a, 'B'->b, ... */
    Mono m;
    m.decode("ABCDE", outtext);
    TEST_ASSERT(strcmp(outtext, "abcde") == 0,
                "decode with identity key");

    /* ROT1 key: A->b, B->c, ..., Z->a */
    m.set_key("bcdefghijklmnopqrstuvwxyza");
    m.decode("ABC", outtext);
    TEST_ASSERT(strcmp(outtext, "bcd") == 0,
                "decode with ROT1 key");

    /* get_key: encoding alphabet for ROT1
       key_[25]='a' => CT[0]='Z', key_[0]='b' => CT[1]='A', ...
       so get_key() returns "ZABCDEFGHIJKLMNOPQRSTUVWXY" */
    const char* key = m.get_key();
    TEST_ASSERT(strlen(key) == 26,    "get_key returns 26-char string");
    TEST_ASSERT(key[0] == 'Z',        "get_key: first char is Z for ROT1");
    TEST_ASSERT(key[1] == 'A',        "get_key: second char is A for ROT1");

    /* decode single character */
    m.set_key("abcdefghijklmnopqrstuvwxyz");
    m.decode("Z", outtext);
    TEST_ASSERT(strcmp(outtext, "z") == 0,
                "decode single character");

    TEST_SUMMARY();
}
