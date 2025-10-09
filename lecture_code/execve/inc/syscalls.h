// Build (cross): aarch64-linux-gnu-gcc -nostdlib -static -fno-pie
// -Wl,--build-id=none -s -o execve_raw execve_syscall_aarch64.c Run with qemu:
// qemu-aarch64 -L /usr/aarch64-linux-gnu ./execve_raw Build (native on arm64):
// gcc -nostdlib -static -fno-pie -Wl,--build-id=none -s -o execve_raw
// execve_syscall_aarch64.c

#include <sys/types.h>
// AArch64 Linux syscall numbers
#define SYS_exit 93
#define SYS_execve 221
#define SYS_write 64

// Minimal raw-syscall helpers (AArch64: x8 = nr, x0..x5 = args, ret in x0).
static inline long syscall1(long nr, long a0) {
  register long x8 asm("x8") = nr;
  register long x0 asm("x0") = a0;
  asm volatile("svc #0" : "+r"(x0) : "r"(x8) : "memory", "cc");
  return x0;
}

static inline long syscall3(long nr, long a0, long a1, long a2) {
  register long x8 asm("x8") = nr;
  register long x0 asm("x0") = a0;
  register long x1 asm("x1") = a1;
  register long x2 asm("x2") = a2;
  asm volatile("svc #0"
               : "+r"(x0)
               : "r"(x8), "r"(x1), "r"(x2)
               : "memory", "cc");
  return x0;
}

static inline long sys_exit(int code) {
  syscall1(SYS_exit, (long)code);
  __builtin_unreachable();
}

static inline long sys_execve(const char *path, char *const argv[],
                              char *const envp[]) {
  return syscall3(SYS_execve, (long)path, (long)argv, (long)envp);
}

static inline ssize_t sys_write(int fd, const void *buf, size_t count) {
  return (ssize_t)syscall3(SYS_write, fd, (long)buf, count);
}
