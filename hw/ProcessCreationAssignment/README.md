# Process Creation & I/O Redirection Assignment

## Overview

In this assignment, you will build **four small utilities** that spawn a child process, execute an arbitrary program, and capture its combined stdout/stderr output via different redirection mechanisms. This assignment reinforces understanding of:

- **Process creation** using `fork()` and `exec()`
- **File descriptor manipulation** and I/O redirection
- **Inter-process communication** (pipes, sockets, shared memory)
- **Low-level system programming** without libc

All binaries must:

- Target **aarch64** architecture
- Issue **direct Linux system calls** (no glibc)
- Link as **static PIE** executables using the provided `_start` entry point

By completing this assignment, you will:

1. Understand how processes are created and managed in Linux
2. Master file descriptor redirection for capturing process output
3. Implement IPC using pipes, files, UNIX domain sockets, and shared memory
4. Write low-level C code that interfaces directly with the Linux kernel
5. Debug and test cross-compiled aarch64 binaries using QEMU

---

## Assignment Structure

### Provided Files (DO NOT MODIFY)

- **`inc/syscalls.h`** – System call wrappers for aarch64 using inline assembly (`svc` instruction)
  - File operations: `sys_read`, `sys_write`, `sys_openat`, `sys_close`, `sys_lseek`
  - Process operations: `sys_fork`, `sys_execve`, `sys_waitpid`, `sys_exit`
  - IPC operations: `sys_pipe`, `sys_dup2`, `sys_socketpair`, `sys_memfd_create`
  - Memory operations: `sys_mmap`, `sys_munmap`, `sys_ftruncate`

- **`inc/runtime.h` / `src/runtime.c`** – Minimal runtime helpers
  - `memcpy`, `memset`, `strlen`
  - `mini_printf` – printf-like function (supports `%s`, `%d`, `%c`)
  - `write_str`, `write_char` – simple output functions
  - `die`, `die_with_status` – error handling

- **`src/start.S`** – Custom `_start` entry point (replaces glibc's `_start`)

- **`Makefile`** – Cross-compilation build system

### Files to Implement

You must implement the following four programs in `src/`:

| File | Technique | Description |
|------|-----------|-------------|
| **`exec_pipe.c`** | Anonymous pipe | Use `pipe()` to create a unidirectional IPC channel. Redirect both stdout and stderr to the pipe's write end in the child. |
| **`exec_file.c`** | Temporary file | Child writes to `/tmp/process_creation_output.txt`. Parent waits for child to finish, then reads and displays the file contents. |
| **`exec_unix_sock.c`** | UNIX domain socket | Use `socketpair(AF_UNIX, SOCK_STREAM)` to create a bidirectional IPC channel. Parent reads from one end while child writes to the other. |
| **`exec_memfd.c`** | Shared memory | Use `memfd_create()` to create an anonymous file descriptor backed by shared memory. Redirect stdout/stderr to this fd, then read back after child exits. |

---

## Implementation Requirements

### General Requirements (All Programs)

Each program must:

1. **Accept command-line arguments**
   - `argv[1]` = path to executable (e.g., `/bin/echo`, `/bin/sh`)
   - `argv[2...]` = arguments to pass to the child process
   - If `argc < 2`, print usage and exit

2. **Fork a child process**
   - Use `sys_fork()` to create a child process
   - Check for errors (`fork()` returns -1 on failure)

3. **Set up I/O redirection in the child**
   - Redirect **both** `STDOUT_FILENO` (1) and `STDERR_FILENO` (2) to the appropriate file descriptor
   - Use `sys_dup2(old_fd, new_fd)` to duplicate file descriptors
   - Close unnecessary file descriptors before calling `exec()`

4. **Execute the target program in the child**
   - Use `sys_execve(argv[1], &argv[1], envp)` to replace the child process
   - Note: `&argv[1]` passes the command with its arguments
   - If `execve()` fails, call `die("execve failed")`

5. **Capture and display output in the parent**
   - Read all output from the IPC mechanism
   - Print each chunk prefixed with `"Child Output: "`
   - Use the provided helper functions like `forward_pipe_output()`, `stream_file()`, etc.

6. **Wait for child completion**
   - Use `sys_waitpid(pid, &status, 0)` to wait for the child
   - Check exit status using `WIFEXITED()` and `WEXITSTATUS()` macros
   - If child exits with non-zero status, print the status

7. **Clean up resources**
   - Close all open file descriptors
   - Remove temporary files (for `exec_file.c`)
   - Properly handle errors and cleanup on failure

### Program-Specific Requirements

#### 1. `exec_pipe.c` – Anonymous Pipe

**Key concepts:**

- A pipe is a unidirectional data channel created with `pipe(pipefd)`
- `pipefd[0]` = read end, `pipefd[1]` = write end
- Child writes to `pipefd[1]`, parent reads from `pipefd[0]`

**Implementation steps:**

1. Create pipe: `sys_pipe(pipefd)`
2. Fork the process
3. **In child:**
   - Close read end (`pipefd[0]`)
   - Redirect stdout and stderr to write end (`pipefd[1]`)
   - Close original write end
   - Execute target program
4. **In parent:**
   - Close write end (`pipefd[1]`)
   - Read from `pipefd[0]` in a loop until EOF
   - Print output with `mini_printf("Child Output: %s", buffer)`
   - Close read end
   - Wait for child

**Expected behavior:**

```bash
$ qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_pipe /bin/echo "Hello World"
Child Output: Hello World
```

---

#### 2. `exec_file.c` – Temporary File

**Key concepts:**

- Child writes stdout/stderr to a file on disk
- Parent reads the file after child terminates
- File path: `/tmp/process_creation_output.txt`

**Implementation steps:**

1. Create/open file: `sys_openat(AT_FDCWD, "/tmp/process_creation_output.txt", O_RDWR | O_CREAT | O_TRUNC, 0600)`
2. Fork the process
3. **In child:**
   - Redirect stdout and stderr to the file descriptor
   - Close original file descriptor
   - Execute target program
4. **In parent:**
   - Close the file descriptor (child has a copy)
   - Wait for child to complete
   - Reopen file for reading
   - Read entire file and print contents
   - Close and delete file: `sys_unlink("/tmp/process_creation_output.txt")`

**Expected behavior:**

```bash
$ qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_file /bin/sh -c "echo test"
Child Output: test
```

---

#### 3. `exec_unix_sock.c` – UNIX Domain Socket

**Key concepts:**

- UNIX domain sockets provide bidirectional IPC
- `socketpair()` creates a connected pair of sockets
- Works like a bidirectional pipe

**Implementation steps:**

1. Create socket pair: `sys_socketpair(AF_UNIX, SOCK_STREAM, 0, sv)`
2. Fork the process
3. **In child:**
   - Close parent's end (`sv[0]`)
   - Redirect stdout and stderr to `sv[1]`
   - Close original `sv[1]`
   - Execute target program
4. **In parent:**
   - Close child's end (`sv[1]`)
   - Read from `sv[0]` until EOF
   - Print output
   - Close `sv[0]`
   - Wait for child

**Expected behavior:**

```bash
$ qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_unix_sock /bin/echo "Socket test"
Child Output: Socket test
```

---

#### 4. `exec_memfd.c` – Shared Memory (Advanced)

**Key concepts:**

- `memfd_create()` creates an anonymous file in memory (no disk I/O)
- The file descriptor can be used like a regular file
- Must allocate space with `ftruncate()` before writing
- Use `lseek()` to rewind before reading

**Implementation steps:**

1. Create memfd: `sys_memfd_create("output_buffer", 0)`
2. Allocate space: `sys_ftruncate(memfd, BUFFER_SIZE)` where `BUFFER_SIZE = 1024 * 1024`
3. Fork the process
4. **In child:**
   - Redirect stdout and stderr to memfd
   - Close original memfd descriptor
   - Execute target program
5. **In parent:**
   - Wait for child to complete
   - Rewind file: `sys_lseek(memfd, 0, SEEK_SET)`
   - Read entire buffer and print contents
   - Close memfd

**Expected behavior:**

```bash
$ qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_memfd /bin/sh -c "printf 'Memory test'"
Child Output: Memory test
```

---

## Building and Testing

### Prerequisites

You need the following tools installed:

- **aarch64 cross-compiler**: `aarch64-linux-gnu-gcc`
- **QEMU user-mode emulator**: `qemu-aarch64` or `qemu-aarch64-static`
- **aarch64 rootfs**: Usually at `/usr/aarch64-linux-gnu/` or `/opt/aarch64-rootfs/`

On Ubuntu/Debian:

```bash
sudo apt-get install gcc-aarch64-linux-gnu qemu-user-static
```

### Build Commands

```bash
# Build all binaries (creates bin/ directory with executables)
make CROSS_COMPILE=aarch64-linux-gnu-

# Build a specific binary
make CROSS_COMPILE=aarch64-linux-gnu- bin/exec_pipe

# Clean build artifacts
make clean
```

### Testing Locally

**Run automated tests:**

```bash
make test CROSS_COMPILE=aarch64-linux-gnu- QEMU=qemu-aarch64-static ROOTFS=/usr/aarch64-linux-gnu
```

**Run individual binaries:**

```bash
# Simple echo test
qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_pipe /bin/echo "Hello World"

# Test with both stdout and stderr
qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_pipe /bin/sh -c "printf 'out'; printf 'err' 1>&2"

# Test all four implementations
qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_pipe /bin/ls /tmp
qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_file /bin/ls /tmp
qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_unix_sock /bin/ls /tmp
qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_memfd /bin/ls /tmp
```

### Verify Binary Properties

Your binaries must be **static PIE** executables with **no dynamic linker**:

```bash
# Check that binary is PIE (Position Independent Executable)
aarch64-linux-gnu-readelf -h bin/exec_pipe | grep 'Type:.*DYN'

# Verify no interpreter segment (should print nothing)
aarch64-linux-gnu-readelf -l bin/exec_pipe | grep INTERP
```

---

## Submission

Create your submission archive:

```bash
make submission.zip
```

This creates `submission.zip` containing:

- `src/` directory with all `.c` and `.S` files
- `inc/` directory with all `.h` files
- `Makefile`

Upload `submission.zip` to Gradescope.

---

## Grading

The autograder will:

1. **Compile your code** using `make CROSS_COMPILE=aarch64-linux-gnu-`
2. **Verify binary properties**:
   - Must be static PIE (Type: DYN)
   - Must have no PT_INTERP segment (no dynamic linker)
3. **Test functionality**: Run each binary with a test payload:

   ```bash
   /bin/sh -c "printf 'STDOUT test\\n'; printf 'STDERR test\\n' 1>&2"
   ```

4. **Check output**: Both "STDOUT test" and "STDERR test" must appear in the output

Each binary is worth 25% of the grade.

---

## Debugging Tips

### Common Issues

1. **"fork failed" or "pipe failed"**
   - Check return values: negative = error
   - Make sure you're calling syscalls correctly

2. **Output not captured**
   - Verify you're redirecting **both** stdout (fd 1) and stderr (fd 2)
   - Ensure you close the correct end of pipes/sockets in each process

3. **"execve failed"**
   - Check that the path to the executable is correct
   - Ensure you're passing `&argv[1]` (not `argv`) to execve

4. **Segmentation fault**
   - You may be accessing memory incorrectly
   - Check buffer sizes and pointer arithmetic
   - Make sure you null-terminate strings before using with `mini_printf`

5. **Binary has interpreter segment**
   - Make sure you're using the provided Makefile
   - Don't modify CFLAGS/LDFLAGS that control `-static-pie` and `-nostdlib`

### Debugging with QEMU

Run with strace to see system calls:

```bash
qemu-aarch64 -L /usr/aarch64-linux-gnu/ -strace bin/exec_pipe /bin/echo "test"
```

### Understanding the Code Flow

For each program, the typical flow is:

1. **Setup** (in parent): Create IPC mechanism (pipe/file/socket/memfd)
2. **Fork**: Split into two processes
3. **Child**: Redirect I/O → Execute target program
4. **Parent**: Read output → Wait for child → Cleanup
5. **Exit**: Return success (0) or failure (non-zero)

---

## Resources

- **Linux System Call Reference**: `man 2 <syscall>` (e.g., `man 2 fork`, `man 2 pipe`)
- **Process Creation Notes**: See `Process Creation.md` in the repository root
- **AArch64 Calling Convention**: Parameters in x0-x5, syscall number in x8
- **File Descriptor Basics**: 0=stdin, 1=stdout, 2=stderr

---
