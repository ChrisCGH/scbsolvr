#include <cmath>
#include <cstring>
#include "test.h"
#include "../mono_solver.h"

/* N_Graph::len_ must be defined exactly once per binary */
int N_Graph::len_ = 4;

int main()
{
    std::cout << "=== Mono_Solver Static Tests ===\n";

    /* phi: empty string */
    TEST_ASSERT(Mono_Solver::phi("") == 0.0,
                "phi: empty string returns 0");

    /* phi: single character -> 0  (n*(n-1) = 0) */
    TEST_ASSERT(Mono_Solver::phi("a") == 0.0,
                "phi: single char returns 0");

    /* phi: all identical -> 1.0  ("aaaa": 4*3 / 4*3) */
    double phi_all_same = Mono_Solver::phi("aaaa");
    TEST_ASSERT(fabs(phi_all_same - 1.0) < 1e-9,
                "phi: all-same letters returns 1.0");

    /* phi: all distinct once -> 0  (each freq=1, contribution = 1*0 = 0) */
    TEST_ASSERT(Mono_Solver::phi("abcdefghijklmnopqrstuvwxyz") == 0.0,
                "phi: all-distinct letters returns 0");

    /* phi: non-alpha characters are ignored */
    double phi_letters = Mono_Solver::phi("aaaa");
    double phi_mixed   = Mono_Solver::phi("a a a a");   /* spaces ignored */
    TEST_ASSERT(fabs(phi_letters - phi_mixed) < 1e-9,
                "phi: non-alpha characters are ignored");

    /* reformat: plain and cipher same length */
    const char* r1 = Mono_Solver::reformat("hello", "HELLO");
    TEST_ASSERT(strcmp(r1, "hello") == 0,
                "reformat: all-alpha cipher preserves plain");

    /* reformat: non-alpha in cipher are copied literally */
    const char* r2 = Mono_Solver::reformat("helloworld", "HELLO WORLD");
    TEST_ASSERT(strcmp(r2, "hello world") == 0,
                "reformat: non-alpha in cipher copied as-is");

    TEST_SUMMARY();
}
