#ifndef TEST_H
#define TEST_H

#include <iostream>

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        tests_run++; \
        if (cond) { \
            tests_passed++; \
            std::cout << "PASS: " << (msg) << "\n"; \
        } else { \
            std::cout << "FAIL: " << (msg) << " (line " << __LINE__ << ")\n"; \
        } \
    } while (0)

#define TEST_SUMMARY() \
    do { \
        std::cout << "\n" << tests_passed << "/" << tests_run << " tests passed\n"; \
        return (tests_passed == tests_run) ? 0 : 1; \
    } while (0)

#endif /* TEST_H */
