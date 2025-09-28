#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <linux/elf.h> // For NT_PRSTATUS
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/uio.h> // For struct iovec
#include <sys/user.h> // For user_pt_regs (may be needed depending on exact headers)
#include <sys/wait.h>
#include <unistd.h>

// Define AArch64 register structure. Matches kernel definition.
struct user_pt_regs {
  uint64_t regs[31]; // x0 - x30 (x30 is LR)
  uint64_t sp;
  uint64_t pc;
  uint64_t pstate;
};

// Function Prototypes (To be implemented by the student)
void print_regs(const char *msg, struct user_pt_regs *regs);
int ptrace_getregs(pid_t pid, struct user_pt_regs *regs);
int ptrace_setregs(pid_t pid, struct user_pt_regs *regs);
void print_status(int status);
unsigned long get_remote_libc_base(pid_t pid);
unsigned long get_local_libc_base();
unsigned long remote_call(pid_t pid, unsigned long func, unsigned long arg0,
                          unsigned long arg1, unsigned long arg2,
                          unsigned long arg3, unsigned long arg4,
                          unsigned long arg5);
int inject_so(pid_t target_pid, const char *so_path);

// --- Implementation Starts Here ---

// Task 1: Basic ptrace Wrappers and Helpers
void print_regs(const char *msg, struct user_pt_regs *regs) {
  printf("==== %s ====\n", msg);
  // TODO: Implement printing of relevant registers (x0-x7, LR, SP, PC, PSTATE)
  // Example for first few registers:
  // for (int i = 0; i < 8; i++) {
  //     printf("x%d: 0x%lx ", i, regs->regs[i]);
  // }
  // printf("\n");
  // printf("lr (x30): 0x%lx\n", regs->regs[30]);
  // printf("sp: 0x%lx\n", regs->sp);
  // printf("pc: 0x%lx\n", regs->pc);
  // printf("pstate: 0x%lx\n", regs->pstate);

  printf("  [!] print_regs: Not fully implemented yet.\n"); // Placeholder
  printf("==== End %s ====\n", msg);
}

int ptrace_getregs(pid_t pid, struct user_pt_regs *regs) {
  // TODO: Implement using ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov)
  // struct iovec iov = {regs, sizeof(*regs)};
  fprintf(stderr, "[!] ptrace_getregs: Not implemented yet.\n"); // Placeholder
  errno = ENOSYS; // Indicate not implemented
  return -1;      // Placeholder
}

int ptrace_setregs(pid_t pid, struct user_pt_regs *regs) {
  // TODO: Implement using ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov)
  // struct iovec iov = {regs, sizeof(*regs)};
  fprintf(stderr, "[!] ptrace_setregs: Not implemented yet.\n"); // Placeholder
  errno = ENOSYS; // Indicate not implemented
  return -1;      // Placeholder
}

void print_status(int status) {
  // TODO: Implement using WIFSTOPPED, WSTOPSIG, WIFEXITED, WEXITSTATUS etc.
  printf("[*] Wait status: 0x%x (print_status not fully implemented)\n",
         status); // Placeholder
}

// Task 2: Address Calculation
unsigned long get_remote_libc_base(pid_t pid) {
  // TODO: Implement parsing /proc/<pid>/maps
  // Look for the line containing "libc" and "r-xp" permission.
  // Extract the starting address.
  fprintf(stderr,
          "[!] get_remote_libc_base: Not implemented yet.\n"); // Placeholder
  return 0;                                                    // Placeholder
}

unsigned long get_local_libc_base() {
  // TODO: Implement using dladdr() on a known libc function (e.g., mmap).
  // Dl_info info;
  // if (dladdr((void *)mmap, &info)) { return (unsigned long)info.dli_fbase; }
  fprintf(stderr,
          "[!] get_local_libc_base: Not implemented yet.\n"); // Placeholder
  return 0;                                                   // Placeholder
}

// Task 3: Remote Function Call
unsigned long remote_call(pid_t pid, unsigned long func, unsigned long arg0,
                          unsigned long arg1, unsigned long arg2,
                          unsigned long arg3, unsigned long arg4,
                          unsigned long arg5) {
  struct user_pt_regs regs, original_regs;
  int status;
  unsigned long retval = 0;
  unsigned long ret_addr = 0;
  long orig_word = 0;
  long brk_instr = 0xd4200000; // brk #0

  fprintf(stderr, "[!] remote_call: Not implemented yet.\n"); // Placeholder

  // Steps:
  // 1. ptrace_getregs(pid, &original_regs)
  // 2. memcpy(&regs, &original_regs, sizeof(regs))
  // 3. ret_addr = original_regs.regs[30] (LR)
  // 4. orig_word = ptrace(PTRACE_PEEKTEXT, pid, ret_addr, NULL)
  // 5. ptrace(PTRACE_POKETEXT, pid, ret_addr, brk_instr)
  // 6. Setup call registers in 'regs': pc=func, regs[0-5]=args,
  // regs[30]=ret_addr
  // 7. ptrace_setregs(pid, &regs)
  // 8. ptrace(PTRACE_CONT, pid, NULL, NULL)
  // 9. waitpid(pid, &status, 0)
  // 10. Check status (should be stopped by SIGTRAP at ret_addr)
  // 11. ptrace_getregs(pid, &regs)
  // 12. retval = regs.regs[0]
  // 13. ptrace(PTRACE_POKETEXT, pid, ret_addr, orig_word) // Restore
  // instruction
  // 14. ptrace_setregs(pid, &original_regs) // Restore original state
  // 15. Return retval

  // Remember error checking for all ptrace calls and waitpid!

  errno = ENOSYS;
  return (unsigned long)-1; // Placeholder for error
}

// Task 4: Shared Object Injection Logic
int inject_so(pid_t target_pid, const char *so_path) {
  int status;
  long ret = -1; // Default to failure
  unsigned long remote_libc = 0;
  unsigned long local_libc = 0;
  void *local_dlopen = NULL;
  unsigned long remote_dlopen = 0;
  void *local_mmap = NULL;
  unsigned long remote_mmap = 0;
  unsigned long remote_buf = 0;
  size_t so_len = strlen(so_path) + 1;
  unsigned long handle = 0;

  printf("[+] Attaching to process %d\n", target_pid);
  if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1) {
    perror("[-] ptrace ATTACH");
    return -1;
  }
  waitpid(target_pid, &status, 0);
  print_status(status);
  if (!WIFSTOPPED(status)) {
    fprintf(stderr, "[-] Process not stopped after attach\n");
    goto detach; // Use goto for cleanup on failure
  }

  fprintf(stderr,
          "[!] inject_so: Core logic not implemented yet.\n"); // Placeholder

  // Steps:
  // 1. Get local and remote libc base addresses
  // 2. Calculate remote dlopen and mmap addresses
  // 3. Use remote_call to invoke mmap in target -> get remote_buf address
  // 4. Check if remote_buf is valid
  // 5. Write so_path to remote_buf using PTRACE_POKETEXT (word by word)
  // 6. Use remote_call to invoke dlopen(remote_buf, flags) in target
  // 7. Check the returned handle
  // 8. Set ret = 0 on success

  // Remember error checking!

detach: // Label for cleanup
  printf("[+] Detaching from process %d\n", target_pid);
  if (ptrace(PTRACE_DETACH, target_pid, NULL, NULL) == -1) {
    perror("[-] ptrace DETACH");
    // If detach fails, the overall operation likely failed anyway
    ret = -1;
  }
  return ret;
}

// --- Main Function (Provided) ---
int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <shared_object.so> <pid>\n", argv[0]);
    return 1;
  }
  const char *so_path = argv[1];
  pid_t target_pid =
      atoi(argv[2]); // Consider using strtol for better error checking

  if (target_pid <= 0) {
    fprintf(stderr, "Invalid PID: %s\n", argv[2]);
    return 1;
  }

  // Check if the .so file exists and is accessible (optional but good practice)
  if (access(so_path, R_OK) != 0) {
    perror("[-] Cannot access shared object file");
    fprintf(stderr, "    Path: %s\n", so_path);
    return 1;
  }

  printf("[*] Injector starting...\n");
  printf("[*] Target PID: %d\n", target_pid);
  printf("[*] Shared object path: %s\n", so_path);

  int result = inject_so(target_pid, so_path);

  if (result == 0) {
    printf("[+] Shared object injection completed successfully.\n");
  } else {
    fprintf(stderr, "[-] Shared object injection failed.\n");
  }

  return (result == 0) ? 0 : 1;
}
