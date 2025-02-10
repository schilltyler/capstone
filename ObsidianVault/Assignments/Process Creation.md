
# Homework: Process Creation and IO Redirection (aarch64)

**Note:** This assignment is designed for the **aarch64** architecture. You should test your code using a proper aarch64 environment or an emulator (e.g., using `qemu-aarch64 -L /usr/aarch64-linux-gnu/ ...` as shown in the sample commands).

Your assignment is to create a program that spawns a child process to run a specified executable while capturing its output (both **stdout** and **stderr**) in the parent process. You will implement **four versions** of this program using different IO redirection techniques. (In real-world scenarios, you’d almost always use a pipe—but this assignment is for practice and learning different methods.)

Before you start coding, review the following resources:

- **The Friendly Manual** (for general guidance)
- System call manuals:
    - `man execve`
    - `man fork`
    - `man pipe`
    - `man openat`
    - `man socket`
    - `man mmap`
    - `man wait`
    - `man memfd_create`

---

## Program Requirements

1. **Command-Line Arguments:**
    
    - **`argv[1]`**: The full path to the executable you want to run (e.g., `/bin/echo` or `/bin/ls`).
    - **`argv[2] ... argv[n]`**: Additional arguments to pass to the executable (e.g., `"-la"`, `"/tmp"`).
2. **Process Creation:**
    
    - Use `fork()` to create a child process.
    - In the child process, use `execve()` to run the executable specified in `argv[1]` (with its arguments).
3. **Output Redirection:**
    
    - **Goal:** Redirect the child’s **stdout** and **stderr** so that the parent process can capture and later print them.
    - **Key Hint:** Use `dup2()` (or `dup`) to duplicate file descriptors. For instance, if you want the child’s output to go to a pipe’s write end, you would call:
        
        ```c
        dup2(pipe_fd, STDOUT_FILENO);
        dup2(pipe_fd, STDERR_FILENO);
        ```
        
        This replaces the standard output and error descriptors with your chosen file descriptor.

---

## Implement Four Methods of IO Redirection

You will write **four separate versions** of your program, each using a different redirection mechanism:

### 1. **Pipe**

- **Purpose:**  
    Pipes are the standard kernel-provided mechanism for redirecting output, creating a unidirectional communication channel between processes.
- **README:**
    - Create a pipe using `pipe()`.
    - After forking, in the child process, use `dup2()` to redirect **stdout** and **stderr** to the pipe’s write end.
    - In the parent process, read the output from the pipe’s read end.
![[fork_pipe.excalidraw.svg|500px]]
- **Sample Command and Output:**
    
    ```bash
    qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_pipe /bin/ls "/proc/self/fd" "-la"
    ```
    
    ```plaintext
    Child Output: total 0
    dr-x------ 2 user user  4 Feb  3 21:06 .
    dr-xr-xr-x 9 user user  0 Feb  3 21:06 ..
    lrwx------ 1 user user 64 Feb  3 21:06 0 -> /dev/pts/7
    l-wx------ 1 user user 64 Feb  3 21:06 1 -> pipe:[1248666]
    l-wx------ 1 user user 64 Feb  3 21:06 2 -> pipe:[1248666]
    lr-x------ 1 user user 64 Feb  3 21:06 3 -> /proc/176445/fd
    ```
    

---

### 2. **File**

- **Purpose:**  
    This method demonstrates how to use file redirection by capturing output in a temporary file.
- **README:**
    - Open or create a temporary file (e.g., `/tmp/foo.txt`).
    - In the child process, use `dup2()` to redirect **stdout** and **stderr** to the file descriptor for the temporary file.
    - After the child process completes, the parent opens and reads the file to display the output.
- **Sample Command and Output:**
    
    ```bash
    qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_file /bin/ls "/proc/self/fd" "-la"
    ```
    
    ```plaintext
    Child Output: total 0
    dr-x------ 2 user user  4 Feb  3 21:19 .
    dr-xr-xr-x 9 user user  0 Feb  3 21:19 ..
    lrwx------ 1 user user 64 Feb  3 21:19 0 -> /dev/pts/7
    l-wx------ 1 user user 64 Feb  3 21:19 1 -> /tmp/foo.txt
    l-wx------ 1 user user 64 Feb  3 21:19 2 -> /tmp/foo.txt
    lr-x------ 1 user user 64 Feb  3 21:19 3 -> /proc/178089/fd
    ```
    

---

### 3. **UNIX Domain Socket**

- **Purpose:**  
    UNIX domain sockets provide a file-based inter-process communication  mechanism. They work like network sockets but are used for local (on the same machine) communication. They are also a lot faster/efficient and don't go through the network cad
- **README:**
    - Create a socket pair using `socketpair()`.
    - In the child process, use `dup2()` to redirect **stdout** and **stderr** to one end of the socket.
    - The parent process reads the output from the other end of the socket.
- **Sample Command and Output:**
    
    ```bash
    qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_unix_sock /bin/ls "/proc/self/fd" "-la"
    ```
    
    ```plaintext
    total 0
    dr-x------ 2 user user  4 Feb  3 21:34 .
    dr-xr-xr-x 9 user user  0 Feb  3 21:34 ..
    lrwx------ 1 user user 64 Feb  3 21:34 0 -> /dev/pts/7
    lrwx------ 1 user user 64 Feb  3 21:34 1 -> socket:[1285659]
    lrwx------ 1 user user 64 Feb  3 21:34 2 -> socket:[1285659]
    lr-x------ 1 user user 64 Feb  3 21:34 3 -> /proc/180937/fd
    ```
    

---

### 4. **Shared Memory (memfd + mmap)**

- **Purpose:**  
    Shared memory allows multiple processes to access the same memory region. With `memfd_create()` and `mmap()`, you create an anonymous memory file that acts as a shared buffer.
- **README:**
    - In the child process, use `memfd_create()` to create an anonymous memory file.
    - Map this file into memory using `mmap()`.
    - Use `dup2()` to redirect **stdout** and **stderr** to the file descriptor associated with this shared memory.
    - The parent process maps the same memory region and reads the output.
    - **Note:** You may assume that the child’s output will be no more than 1MB.
- **Sample Command and Output:**
    
    ```bash
    qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/exec_memfd /bin/ls "/proc/self/fd" "-la"
    ```
    
    ```plaintext
    total 0
    dr-x------ 2 user user  4 Feb  3 21:40 .
    dr-xr-xr-x 9 user user  0 Feb  3 21:40 ..
    lrwx------ 1 user user 64 Feb  3 21:40 0 -> /dev/pts/7
    lrwx------ 1 user user 64 Feb  3 21:40 1 -> /memfd:output_buffer (deleted)
    lrwx------ 1 user user 64 Feb  3 21:40 2 -> /memfd:output_buffer (deleted)
    lr-x------ 1 user user 64 Feb  3 21:40 3 -> /proc/182948/fd
    ```
    

---

## Summary of IO Redirection Methods

- **Pipe:**  
    _Best practice for simple IO redirection._ Uses a kernel buffer to transfer data from the child (writing) to the parent (reading).  
    **Key Function:** Use `dup2()` to redirect output to the pipe’s write end.
    
- **File:**  
    _Simple and persistent method._ The child writes to a temporary file; the parent reads from it afterward.  
    **Key Function:** Use `dup2()` to replace **stdout** and **stderr** with the file descriptor for the temporary file.
    
- **UNIX Domain Socket:**  
    _Socket-based IPC for local communication._ Uses a file-based address to pass data between processes.  
    **Key Function:** Use `dup2()` to redirect output to the socket file descriptor.
    
- **Shared Memory (memfd + mmap):**  
    _Efficient and fast._ Both processes share a memory buffer (up to 1MB for this assignment).  
    **Key Function:** Use `dup2()` to redirect output to the file descriptor obtained from `memfd_create()`.
    

---

**Additional Reminder:**  
For any redirection method, `dup2()` (or `dup`) is essential for replacing the standard output/error file descriptors with your chosen communication channel. This ensures that when the child writes to **stdout** or **stderr**, the output is sent to your selected destination (pipe, file, socket, or shared memory).
Also note that `dup2` is in libc eventually makes its way into a syscall, not necessarily called dup2 the same way that `open` is actually a wrapper around `openat`
