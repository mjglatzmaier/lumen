#include "test_framework.h"

// **Extern Test Suites**
extern TestCase vec_tests[];
extern TestCase mat_tests[];
extern TestCase mimalloc_tests[];
extern TestCase gen_alloc_tests[];
extern TestCase frame_alloc_tests[];
extern TestCase stack_alloc_tests[];
extern TestCase cont_da_tests[];
extern TestCase cont_da_tests[];
extern TestCase pool_alloc_tests[];
extern TestCase job_scheduling_tests[];

// **Manually specify the size**
extern int vec_tests_count;
extern int mat_tests_count;
extern int mimalloc_tests_count;
extern int gen_alloc_tests_count;
extern int frame_alloc_tests_count;
extern int stack_alloc_tests_count;
extern int cont_da_tests_count;
extern int pool_alloc_tests_count;
extern int job_scheduling_tests_count;

int main() {
    printf("\n🚀 Running Lumen Unit Tests...\n");

    RUN_TESTS("Vec Tests", vec_tests, vec_tests_count);
    RUN_TESTS("Matrix Tests", mat_tests, mat_tests_count);
    RUN_TESTS("Mimalloc Tests", mimalloc_tests, mimalloc_tests_count);
    RUN_TESTS("Default alloc Tests", gen_alloc_tests, gen_alloc_tests_count);
    RUN_TESTS("Frame Allocator Tests", frame_alloc_tests, frame_alloc_tests_count);
    RUN_TESTS("Stack Allocator Tests", stack_alloc_tests, stack_alloc_tests_count);
    RUN_TESTS("Pool Allocator Tests", pool_alloc_tests, pool_alloc_tests_count);
    RUN_TESTS("ContDA Tests", cont_da_tests, cont_da_tests_count);
    RUN_TESTS("Scheduling Tests", job_scheduling_tests, job_scheduling_tests_count);

    return 0;
}
