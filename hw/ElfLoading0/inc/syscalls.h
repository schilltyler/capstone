#ifndef SYSCALLS_H
#define SYSCALLS_H

// aarch64 Linux syscall numbers
#define SYS_read 63
#define SYS_write 64
#define SYS_openat 56
#define SYS_close 57
#define SYS_lseek 62
#define SYS_mmap 222
#define SYS_munmap 215
#define SYS_mprotect 226
#define SYS_brk 214
#define SYS_exit 93

// AT_FDCWD for openat
#define AT_FDCWD -100

// Open flags
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

// SEEK flags
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// mmap flags
#define PROT_NONE 0x0
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4

#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_FIXED 0x10

#define MAP_FAILED ((void *) -1)

// Generic syscall wrappers using inline assembly
static inline long syscall0(long n) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0");
    __asm__ __volatile__(
        "svc #0"
        : "=r"(x0)
        : "r"(x8)
        : "memory"
    );
    return x0;
}

static inline long syscall1(long n, long a0) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    __asm__ __volatile__(
        "svc #0"
        : "+r"(x0)
        : "r"(x8)
        : "memory"
    );
    return x0;
}

static inline long syscall2(long n, long a0, long a1) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    __asm__ __volatile__(
        "svc #0"
        : "+r"(x0)
        : "r"(x8), "r"(x1)
        : "memory"
    );
    return x0;
}

static inline long syscall3(long n, long a0, long a1, long a2) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    __asm__ __volatile__(
        "svc #0"
        : "+r"(x0)
        : "r"(x8), "r"(x1), "r"(x2)
        : "memory"
    );
    return x0;
}

static inline long syscall4(long n, long a0, long a1, long a2, long a3) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    register long x3 __asm__("x3") = a3;
    __asm__ __volatile__(
        "svc #0"
        : "+r"(x0)
        : "r"(x8), "r"(x1), "r"(x2), "r"(x3)
        : "memory"
    );
    return x0;
}

static inline long syscall5(long n, long a0, long a1, long a2, long a3, long a4) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    register long x3 __asm__("x3") = a3;
    register long x4 __asm__("x4") = a4;
    __asm__ __volatile__(
        "svc #0"
        : "+r"(x0)
        : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4)
        : "memory"
    );
    return x0;
}

static inline long syscall6(long n, long a0, long a1, long a2, long a3, long a4, long a5) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    register long x3 __asm__("x3") = a3;
    register long x4 __asm__("x4") = a4;
    register long x5 __asm__("x5") = a5;
    __asm__ __volatile__(
        "svc #0"
        : "+r"(x0)
        : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
        : "memory"
    );
    return x0;
}

// Friendly wrapper functions
static inline long sys_read(int fd, void *buf, unsigned long count) {
    return syscall3(SYS_read, fd, (long)buf, count);
}

static inline long sys_write(int fd, const void *buf, unsigned long count) {
    return syscall3(SYS_write, fd, (long)buf, count);
}

static inline long sys_openat(int dirfd, const char *pathname, int flags) {
    return syscall3(SYS_openat, dirfd, (long)pathname, flags);
}

static inline long sys_close(int fd) {
    return syscall1(SYS_close, fd);
}

static inline long sys_lseek(int fd, long offset, int whence) {
    return syscall3(SYS_lseek, fd, offset, whence);
}

static inline void *sys_mmap(void *addr, unsigned long length, int prot, int flags, int fd, long offset) {
    return (void *)syscall6(SYS_mmap, (long)addr, length, prot, flags, fd, offset);
}

static inline long sys_munmap(void *addr, unsigned long length) {
    return syscall2(SYS_munmap, (long)addr, length);
}

static inline long sys_mprotect(void *addr, unsigned long len, int prot) {
    return syscall3(SYS_mprotect, (long)addr, len, prot);
}

static inline void *sys_brk(void *addr) {
    return (void *)syscall1(SYS_brk, (long)addr);
}

static inline void sys_exit(int status) {
    syscall1(SYS_exit, status);
    __builtin_unreachable();
}

#endif /* SYSCALLS_H */
