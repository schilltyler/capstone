// ldso.c
#define _GNU_SOURCE
#include <dlfcn.h>
#include <link.h>
#include <stdio.h>

typedef int (*my_printf_t)(const char *format, ...);

int main(void) {
    return 0;
}
