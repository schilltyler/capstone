#include <stdio.h>
#include <math.h>

// Test 2: Program using libm (math library)
// Tests: Multiple dependencies (libc + libm), GOT relocations for data

int main() {
    printf("Math Library Test\n");
    printf("==================\n");

    double x = 2.0;
    printf("sqrt(%.1f) = %.6f\n", x, sqrt(x));

    double angle = M_PI / 2.0;
    printf("sin(PI/2) = %.6f\n", sin(angle));
    printf("cos(PI/2) = %.6f\n", cos(angle));

    double base = 2.71828;
    double exponent = 2.0;
    printf("pow(%.5f, %.1f) = %.6f\n", base, exponent, pow(base, exponent));

    printf("\nAll math functions resolved correctly!\n");
    return 0;
}
