#include <stdio.h>
#include <linux/unistd.h> // Include syscall numbers
//#include <sys/syscall.h>  // Include syscall function declaration

int main() {
    printf("SYS_read: %d\n", __NR_read);
    printf("SYS_write: %d\n", __NR_write);
    printf("SYS_open: %d\n", __NR_openat);
    printf("SYS_close: %d\n", __NR_close);
    return 0;
}

