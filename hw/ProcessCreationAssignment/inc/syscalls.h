#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

typedef long ssize_t;
typedef int pid_t;

// File descriptor constants
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// Common flags/constants
#define AT_FDCWD -100

#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR 0x0002
#define O_CREAT 0x0040
#define O_TRUNC 0x0200

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define PROT_NONE 0x0
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4

#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20

#define WIFEXITED(status) (((status) & 0x7f) == 0)
#define WEXITSTATUS(status) (((status) >> 8) & 0xff)

#define SIGCHLD 17

#define AF_UNIX 1
#define SOCK_STREAM 1

#define MFD_CLOEXEC 0x0001

// System call numbers for aarch64
#define SYS_read 63
#define SYS_write 64
#define SYS_openat 56
#define SYS_close 57
#define SYS_statx 291
#define SYS_lseek 62
#define SYS_mmap 222
#define SYS_munmap 215
#define SYS_mprotect 226
#define SYS_exit 93
#define SYS_clone 220
#define SYS_execve 221
#define SYS_wait4 260
#define SYS_pipe2 59
#define SYS_dup3 24
#define SYS_socketpair 199
#define SYS_memfd_create 279
#define SYS_ftruncate 46
#define SYS_unlinkat 35
#define SYS_brk 214

// Generic syscall helpers
static inline long syscall0(long n) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0");
    __asm__ __volatile__("svc #0" : "=r"(x0) : "r"(x8) : "memory");
    return x0;
}

static inline long syscall1(long n, long a0) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    __asm__ __volatile__("svc #0" : "+r"(x0) : "r"(x8) : "memory");
    return x0;
}

static inline long syscall2(long n, long a0, long a1) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    __asm__ __volatile__("svc #0" : "+r"(x0) : "r"(x8), "r"(x1) : "memory");
    return x0;
}

static inline long syscall3(long n, long a0, long a1, long a2) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    __asm__ __volatile__("svc #0"
                         : "+r"(x0)
                         : "r"(x8), "r"(x1), "r"(x2)
                         : "memory");
    return x0;
}

static inline long syscall4(long n, long a0, long a1, long a2, long a3) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    register long x3 __asm__("x3") = a3;
    __asm__ __volatile__("svc #0"
                         : "+r"(x0)
                         : "r"(x8), "r"(x1), "r"(x2), "r"(x3)
                         : "memory");
    return x0;
}

static inline long syscall5(long n, long a0, long a1, long a2, long a3, long a4) {
    register long x8 __asm__("x8") = n;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    register long x3 __asm__("x3") = a3;
    register long x4 __asm__("x4") = a4;
    __asm__ __volatile__("svc #0"
                         : "+r"(x0)
                         : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4)
                         : "memory");
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
    __asm__ __volatile__("svc #0"
                         : "+r"(x0)
                         : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
                         : "memory");
    return x0;
}

// Friendly wrappers
static inline long sys_read(int fd, void *buf, unsigned long count) {
    return syscall3(SYS_read, fd, (long)buf, count);
}

static inline long sys_write(int fd, const void *buf, unsigned long count) {
    return syscall3(SYS_write, fd, (long)buf, count);
}

static inline long sys_openat(int dirfd, const char *pathname, int flags, int mode) {
    return syscall4(SYS_openat, dirfd, (long)pathname, flags, mode);
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

static inline long sys_ftruncate(int fd, unsigned long length) {
    return syscall2(SYS_ftruncate, fd, length);
}

static inline long sys_unlinkat(int dirfd, const char *pathname, int flags) {
    return syscall3(SYS_unlinkat, dirfd, (long)pathname, flags);
}

static inline long sys_unlink(const char *pathname) {
    return sys_unlinkat(AT_FDCWD, pathname, 0);
}

static inline long sys_pipe2(int pipefd[2], int flags) {
    return syscall2(SYS_pipe2, (long)pipefd, flags);
}

static inline long sys_pipe(int pipefd[2]) {
    return sys_pipe2(pipefd, 0);
}

static inline long sys_dup3(int oldfd, int newfd, int flags) {
    return syscall3(SYS_dup3, oldfd, newfd, flags);
}

static inline long sys_dup2(int oldfd, int newfd) {
    return sys_dup3(oldfd, newfd, 0);
}

static inline long sys_socketpair(int domain, int type, int protocol, int sv[2]) {
    return syscall4(SYS_socketpair, domain, type, protocol, (long)sv);
}

static inline long sys_memfd_create(const char *name, unsigned int flags) {
    return syscall2(SYS_memfd_create, (long)name, flags);
}

static inline pid_t sys_fork(void) {
    long ret = syscall5(SYS_clone, SIGCHLD, 0, 0, 0, 0);
    return (pid_t)ret;
}

static inline long sys_wait4(pid_t pid, int *status, int options, void *rusage) {
    return syscall4(SYS_wait4, pid, (long)status, options, (long)rusage);
}

static inline pid_t sys_waitpid(pid_t pid, int *status, int options) {
    return (pid_t)sys_wait4(pid, status, options, 0);
}

static inline long sys_execve(const char *pathname, char *const argv[], char *const envp[]) {
    return syscall3(SYS_execve, (long)pathname, (long)argv, (long)envp);
}

static inline void sys_exit(int status) {
    syscall1(SYS_exit, status);
    __builtin_unreachable();
}

#endif // SYSCALLS_H
