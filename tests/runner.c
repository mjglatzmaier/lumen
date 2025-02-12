#include "test_framework.h"

// **Extern Test Suites**
extern TestCase vec_tests[];
extern TestCase mat_tests[];
extern TestCase mimalloc_tests[];
extern TestCase gen_alloc_tests[];

// **Manually specify the size**
extern int vec_tests_count;
extern int mat_tests_count;
extern int mimalloc_tests_count;
extern int gen_alloc_tests_count;

int main() {
    printf("\n🚀 Running Lumen Unit Tests...\n");

    RUN_TESTS("Vec Tests", vec_tests, vec_tests_count);
    RUN_TESTS("Matrix Tests", mat_tests, mat_tests_count);
    RUN_TESTS("Mimalloc Tests", mimalloc_tests, mimalloc_tests_count);
    RUN_TESTS("Default alloc Tests", gen_alloc_tests, gen_alloc_tests_count);
    return 0;
}
