#include "syscalls.h"

// Simple hello world test program that uses direct syscalls only
// This is a minimal static PIE that the loader can execute

void _start(void) {
    const char *message = "Hello, World!\n";

    // Calculate string length
    const char *p = message;
    unsigned long len = 0;
    while (*p++) len++;

    // Write to stdout using direct syscall
    sys_write(1, message, len);

    // Exit successfully
    sys_exit(0);
}
