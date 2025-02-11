#include "test_framework.h"

// **Extern Test Suites**
extern TestCase vec_tests[];
extern TestCase mat_tests[];
//extern void run_benchmarks();

// **Manually specify the size**
extern int vec_tests_count;
extern int mat_tests_count;

int main() {
    printf("\n🚀 Running Lumen Unit Tests...\n");

    RUN_TESTS("Vec Tests", vec_tests, vec_tests_count);
    RUN_TESTS("Matrix Tests", mat_tests, mat_tests_count);
    
   //run_benchmarks();
    
    return 0;
}
