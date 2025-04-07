
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <sys/types.h>

// System call numbers
#define SYS_openat 56
#define SYS_close 57
#define SYS_read 63
#define SYS_write 64
#define SYS_mmap 222
#define SYS_mprotect 226
#define SYS_lseek 62
#define SYS_getpid 172

#define SYS_memfd_create 279
// Base syscall functions with different argument counts
// 0-argument system call
static inline long syscall0(long num) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0");
  asm volatile("svc 0" : "=r"(x0) : "r"(x8) : "memory", "cc");
  return x0;
}

// 1-argument system call
static inline long syscall1(long num, long a) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  asm volatile("svc 0"
               : "+r"(x0) // x0 will contain result
               : "r"(x8)
               : "memory", "cc");
  return x0;
}

// 2-argument system call
static inline long syscall2(long num, long a, long b) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  asm volatile("svc 0"
               : "+r"(x0) // x0 will contain result
               : "r"(x1), "r"(x8)
               : "memory", "cc");
  return x0;
}

// 3-argument system call
static inline long syscall3(long num, long a, long b, long c) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  asm volatile("svc 0"
               : "+r"(x0) // x0 will contain result
               : "r"(x1), "r"(x2), "r"(x8)
               : "memory", "cc");
  return x0;
}

// 4-argument system call
static inline long syscall4(long num, long a, long b, long c, long d) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  register long x3 asm("x3") = d;
  asm volatile("svc 0"
               : "+r"(x0) // x0 will contain result
               : "r"(x1), "r"(x2), "r"(x3), "r"(x8)
               : "memory", "cc");
  return x0;
}

// 5-argument system call
static inline long syscall5(long num, long a, long b, long c, long d, long e) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  register long x3 asm("x3") = d;
  register long x4 asm("x4") = e;
  asm volatile("svc 0"
               : "+r"(x0) // x0 will contain result
               : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x8)
               : "memory", "cc");
  return x0;
}

// 6-argument system call
static inline long syscall6(long num, long a, long b, long c, long d, long e,
                            long f) {
  register long x8 asm("x8") = num;
  register long x0 asm("x0") = a;
  register long x1 asm("x1") = b;
  register long x2 asm("x2") = c;
  register long x3 asm("x3") = d;
  register long x4 asm("x4") = e;
  register long x5 asm("x5") = f;
  asm volatile("svc 0"
               : "+r"(x0) // x0 will contain result
               : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8)
               : "memory", "cc");
  return x0;
}

// Typed system call interfaces

/**
 * Open a file relative to a directory file descriptor
 *
 * @param dirfd Directory file descriptor
 * @param pathname Path to the file
 * @param flags Open flags
 * @param mode Creation mode
 * @return File descriptor or error code
 */
static inline long sys_openat(int dirfd, const char *pathname, int flags,
                              mode_t mode) {
  return syscall4(SYS_openat, (long)dirfd, (long)pathname, (long)flags,
                  (long)mode);
}

/**
 * Close a file descriptor
 *
 * @param fd File descriptor to close
 * @return 0 on success, error code on failure
 */
static inline long sys_close(int fd) { return syscall1(SYS_close, (long)fd); }

/**
 * Read from a file descriptor
 *
 * @param fd File descriptor
 * @param buf Buffer to read into
 * @param count Number of bytes to read
 * @return Number of bytes read or error code
 */
static inline long sys_read(int fd, void *buf, size_t count) {
  return syscall3(SYS_read, (long)fd, (long)buf, (long)count);
}

/**
 * Write to a file descriptor
 *
 * @param fd File descriptor
 * @param buf Buffer to write from
 * @param count Number of bytes to write
 * @return Number of bytes written or error code
 */
static inline long sys_write(int fd, const void *buf, size_t count) {
  return syscall3(SYS_write, (long)fd, (long)buf, (long)count);
}

/**
 * Map files or devices into memory
 *
 * @param addr Suggested address for the mapping
 * @param length Length of the mapping
 * @param prot Memory protection flags
 * @param flags Mapping flags
 * @param fd File descriptor
 * @param offset Offset in the file
 * @return Mapped address or error code
 */
static inline void *sys_mmap(void *addr, size_t length, int prot, int flags,
                             int fd, off_t offset) {
  return (void *)syscall6(SYS_mmap, (long)addr, (long)length, (long)prot,
                          (long)flags, (long)fd, (long)offset);
}

/**
 * Change memory protection
 *
 * @param addr Start of memory region
 * @param len Length of memory region
 * @param prot New protection flags
 * @return 0 on success, error code on failure
 */
static inline long sys_mprotect(void *addr, size_t len, int prot) {
  return syscall3(SYS_mprotect, (long)addr, (long)len, (long)prot);
}

/**
 * Reposition read/write file offset
 *
 * @param fd File descriptor
 * @param offset Offset in bytes
 * @param whence Reference position for offset
 * @return New offset or error code
 */
static inline off_t sys_lseek(int fd, off_t offset, int whence) {
  return syscall3(SYS_lseek, (long)fd, (long)offset, (long)whence);
}

/**
 * Get process ID
 *
 * @return Process ID of the calling process
 */
static inline pid_t sys_getpid(void) { return (pid_t)syscall0(SYS_getpid); }

static inline int sys_memfd_create(const char *name, unsigned int flags) {
  return syscall2(SYS_memfd_create, (long)name, (long)flags);
}

#endif /* SYSCALL_H */
