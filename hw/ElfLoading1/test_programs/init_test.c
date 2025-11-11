#include <stdio.h>

// Test 3: Initialization array test
// Tests: .init_array execution order

__attribute__((constructor))
void init1() {
    printf("[INIT 1] First constructor function called\n");
}

__attribute__((constructor))
void init2() {
    printf("[INIT 2] Second constructor function called\n");
}

__attribute__((constructor))
void init3() {
    printf("[INIT 3] Third constructor function called\n");
}

int main() {
    printf("[MAIN] Main function called\n");
    printf("\nIf you see this, .init_array was executed correctly!\n");
    return 0;
}
