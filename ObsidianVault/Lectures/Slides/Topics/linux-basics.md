
---
<!-- slide template="[[Base Slide]]" -->
::: title

#### What Is an Operating System?

:::

- A fancy resource manager.
- Abstracts away device management (CPU, memory, I/O devices, etc.).
- The core mechanism is _virtualization_ of resources.
- Exports an interface for user applications (system calls, libraries) to interact with hardware indirectly.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Operating System Kernel

:::

- Coordinates CPU, memory, and I/O device interactions.
- Implements isolation and protection so userland processes don’t directly manipulate hardware.
- monolithic vs micro vs hybrid
- Typically monolithic in Linux, but modular (loadable kernel modules) for extensibility.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### The Linux Operating System

:::

- Open-source, community-developed.
- Multi-architecture: supports 32-bit, 64-bit x86, ARM (including AArch64), RISC-V, and more.
- Broad usage in servers, desktops, embedded systems, IoT devices, HPC, mobile (Android), and beyond.
- We’ll focus on a 64-bit ARM (ARMv8-A / AArch64) distribution Raspbery Pi OS .

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Devices That Run Linux

:::

- Desktop computers and laptops
- Cloud servers (e.g., AWS Graviton)
- Mobile devices and tablets (Android)
- Embedded and IoT platforms (Raspberry Pi, various ARM boards, embeded linux)
- Supercomputers (HPC clusters)
- Each environment has different constraints (power, screen size, memory, etc.).

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### (Some) Linux Design Principles

:::

- Open source
- Portability
- Modularity (kernel modules)
- Performance
- Security (mandatory or discretionary)
- Stability (POSIX compliance, stable user APIs)

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Extensibility

:::

- Most Linux functionality lives in kernel subsystems (VFS, networking stack, device drivers).
- Loadable kernel modules (LKMs) can be dynamically inserted/removed.
- Separation of userland and kernel helps keep user-space additions flexible.
- Linux distributions package userland programs as well, but the core is the kernel + standard libraries.
- Monolithic means one subsystem can break another :)
---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Portability

:::

- Linux runs on x86, ARM, RISC-V, PowerPC, MIPS, and more.
- Kernel code is mostly in C (with some assembly for architecture-specific functionality).
- The Hardware Abstraction is typically integrated directly in architecture subdirectories (arch/arm64, arch/x86, etc.) within the kernel.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Reliability

:::

- User applications cannot typically crash the kernel due to robust memory isolation.
- Kernel module signing and Secure Boot exist to mitigate malicious or buggy LKMs.
- However, a kernel module can still crash the system if it’s badly coded—so caution is necessary.

---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Performance

:::

::: left

- Linux emphasizes scalability across hardware from microcontrollers (embedded linux) to supercomputers.
- there are even real time variants
:::

::: right
![[stolen_meme_arch_v_win.png]]
<small>
https://preview.redd.it/k12r6k1g4ar51.png?width=1080&crop=smart&auto=webp&s=e191c362dd3c9edb63a304995571c93f4b7fcd59
</small>
:::

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Compatibility

:::

- Linux userland aims for POSIX compliance, but distributions can differ.
- The user-space ABI (Application Binary Interface) is quite stabe
	- But differences in LIBC versions, file system layouts ..etc can make creating a "build once  anywhere" binaries for distros challenging 
- The kernel internal APIs can evolve more frequently, but glibc and userland see fewer breaking changes compared to kernel modules.
- ...this is of course only in theory.
	- syscall changes, libc versions,  distro fragmentation ...etc

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Security

:::

- Separation of kernel and user mode.
- User/Group/Other permissions, SELinux or AppArmor for MAC.
- Each process has its own private virtual address space

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### The Linux Kernel

:::

- Monolithic design with dynamically loadable modules.
- Everything is represented as a file or file descriptor (including devices) in userland.
- Resource objects exist in-kernel (task_struct, inodes, etc.) but are not directly accessible to userland.
- System calls are used for interacting with the kernel from userland 

---
<!-- slide template="[[Base Slide]]" -->


::: title

#### Kernel Objects

:::

- The kernel organizes resources into structures, like `task_struct` (for processes), `file` and `inode` (for open files).
- These are not typically called “objects” in Linux, but they serve a similar purpose.
- Access from userland is via file descriptors or handles returned by syscalls (e.g., `open()`).

---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Objects and File Descriptors

:::
::: left
- A file descriptor (FD) is an integer index into a per-process table referencing kernel structures.
- This allows the kernel to abstract away direct memory management of resources.
- Processes can share FDs or pass them around, enabling resource sharing with controlled security.
:::
::: right

![[fd-syscall-meme.png]]
:::
---

## Processes

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Processes

:::

- All userland code runs inside _some_ process context.
- A process is identified by a PID (process ID).
- A process is essentially a container for resources required to run a program: memory, file descriptors, scheduling parameters, etc.
- Code actually runs via one or more threads within the process.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Threads

:::

- The actual _unit of execution_.
- Each thread has its own CPU state (registers, stack pointer).
- Linux implements threads as processes that share many resources (e.g., memory, FD table).
- On ARM64, each thread can independently run on any available CPU core.

---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Process Resources

:::

::: left

- PID (unique to each process).
- Security context (UID, GID, supplementary groups).
- Table of open file descriptors.
- A private (or shared) virtual address space.
- One or more threads.

:::

::: right
<code>
<pre>
 $ sudo ls /proc
1      16     216    309    372    42730  470	 481	50418  54663  6      64     65591  74	895	     driver	  kpagecgroup	 partitions	uptime
11     16982  22     31     373    430	  471	 483	50448  54673  60     64177  65592  75	9	     execdomains  kpagecount	 schedstat	version
117    17     232    31239  39	   431	  472	 484	50449  547    60375  64373  66	   76	asound	     fb		  kpageflags	 self		vmallocinfo
11959  179    233    31298  4	   43519  473	 485	51     54830  60440  64374  67	   77	buddyinfo    filesystems  latency_stats  slabinfo	vmstat
11960  18     24     31299  40	   44	  474	 486	52139  549    60655  64674  68	   79	bus	     fs		  loadavg	 softirqs	zoneinfo
12     18404  25     32     40683  444	  47687  49	53     55     61     65     7	   86	cgroups      interrupts   locks		 stat
120    19     26     32647  40689  45	  477	 49316	53086  56     611    65308  70	   869	cmdline      iomem	  mdstat	 swaps
12485  19054  27     32784  40737  46	  47769  49328	532    56282  61352  65490  71	   87	consoles     ioports	  meminfo	 sys
12880  19055  278    32795  40768  462	  47770  49685	538    56286  61392  65493  72	   873	cpuinfo      irq	  misc		 sysrq-trigger
13     2      29     34     41566  463	  478	 49697	539    564    614    65497  723    874	crypto	     kallsyms	  modules	 sysvipc
14     20     3      369    41605  46482  479	 5	54     567    61793  65507  726    88	devices      keys	  mounts	 thread-self
143    21     30     370    42	   46728  47983  50	540    580    61822  65543  73	   89	device-tree  key-users	  net		 timer_list
15     215    30388  371    42724  47	  480	 50389	541    59     620    65590  733    894	diskstats    kmsg	  pagetypeinfo	 tty
$ ps aux | head
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root           1  0.0  0.1 170592 11680 ?        Ss   Jan08   0:18 /sbin/init
root           2  0.0  0.0      0     0 ?        S    Jan08   0:00 [kthreadd]
root           3  0.0  0.0      0     0 ?        S    Jan08   0:00 [pool_workqueue_release]
root           4  0.0  0.0      0     0 ?        I<   Jan08   0:00 [kworker/R-rcu_g]
root           5  0.0  0.0      0     0 ?        I<   Jan08   0:00 [kworker/R-rcu_p]
root           6  0.0  0.0      0     0 ?        I<   Jan08   0:00 [kworker/R-slub_]
root           7  0.0  0.0      0     0 ?        I<   Jan08   0:00 [kworker/R-netns]
root           9  0.0  0.0      0     0 ?        I<   Jan08   0:00 [kworker/0:0H-events_highpri]
root          11  0.0  0.0      0     0 ?        I    Jan08   0:00 [kworker/u8:0-netns]
</pre>
</code>
:::

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Random Access Memory

:::

- Physical memory: a linear array of bytes.
- Logically organized into Physical Pages 
- Usually Address is Physical Page Number (PPN) + offset
- On ARM64, addresses are 64-bit but typically not all 64 bits are used (e.g., 48-bit or 52-bit virtual addressing depending on kernel config).
- Byte-addressable means each address points to an 8-bit chunk.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Virtual Memory

:::

- Each process sees its own _private virtual address space_.
- For 64-bit ARM, theoretically up to 2642^{64} bytes, though practically less.
- The kernel’s memory manager maps virtual pages to physical pages.
- The process has the illusion of a large contiguous memory region.

---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Process Virtual Memory

:::

::: left

- Linux uses page tables to map virtual to physical memory.
- Kernel and user share the same address space mapping but in separate regions (on typical 64-bit distros, top addresses for kernel, lower for user).

:::

::: right
<code>
<pre>
$ cat /proc/self/maps
5555f9310000-5555f931c000 r-xp 00000000 103:02 1777                      /usr/bin/cat
5555f932c000-5555f9330000 r--p 0000c000 103:02 1777                      /usr/bin/cat
5555f9330000-5555f9334000 rw-p 00010000 103:02 1777                      /usr/bin/cat
5555fcab8000-5555fcadc000 rw-p 00000000 00:00 0                          [heap]
7fff13b5c000-7fff13b84000 rw-p 00000000 00:00 0 
7fff13b84000-7fff13e70000 r--p 00000000 103:02 10954                     /usr/lib/locale/locale-archive
7fff13e70000-7fff13ff8000 r-xp 00000000 103:02 5543                      /usr/lib/aarch64-linux-gnu/libc.so.6
7fff13ff8000-7fff1400c000 ---p 00188000 103:02 5543                      /usr/lib/aarch64-linux-gnu/libc.so.6
7fff1400c000-7fff14010000 r--p 0018c000 103:02 5543                      /usr/lib/aarch64-linux-gnu/libc.so.6
7fff14010000-7fff14014000 rw-p 00190000 103:02 5543                      /usr/lib/aarch64-linux-gnu/libc.so.6
7fff14014000-7fff14020000 rw-p 00000000 00:00 0 
7fff1403c000-7fff14064000 r-xp 00000000 103:02 5306                      /usr/lib/aarch64-linux-gnu/ld-linux-aarch64.so.1
7fff1406c000-7fff14074000 r--p 00000000 00:00 0                          [vvar]
7fff14074000-7fff14078000 r-xp 00000000 00:00 0                          [vdso]
7fff14078000-7fff1407c000 r--p 0002c000 103:02 5306                      /usr/lib/aarch64-linux-gnu/ld-linux-aarch64.so.1
7fff1407c000-7fff14080000 rw-p 00030000 103:02 5306                      /usr/lib/aarch64-linux-gnu/ld-linux-aarch64.so.1
7fffe7464000-7fffe7488000 rw-p 00000000 00:00 0                          [stack]
</pre>
</code>

:::

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Virtual Memory Concepts

:::

- Page tables & translation walks (done by hardware MMU).
- 4 KB pages by default on many ARM64 configs, sometimes 64 KB.
- Memory protections (read, write, execute bits).
- Kernel address space vs. user address space.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Memory Layout

:::


- User space typically in lower half of the virtual address space, kernel in upper half.
- The stack grows downward, the heap grows upward.
- The binary’s code (.text), data (.data, .bss), and other segments are mapped into memory at runtime.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Memory

:::

- **Virtual**: The illusion of contiguous memory for each process.
	- **Stack**: Manages local variables and function calls (grows downward).
	- **Heap**: Dynamically allocated space (via `malloc()` / `free()` in C).

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Disk

:::

- Much slower persistent storage (eMMC, SSD, HDD).
- Cheap, large, but with high latency.
- Access typically goes through the VFS (Virtual File System).
- Common Linux filesystems: `ext4`, `xfs`, `btrfs`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Linux System Architecture

:::

- **User Processes**
- **Standard Libraries (glibc, musl, etc.)**
- **System Call Interface**
- **Kernel Subsystems** (VFS, memory manager, scheduler, networking)
- **Device Drivers**
- **(Sometimes) Hypervisor** (KVM)
- Lots of other subsystems. 

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Interacting With the Linux OS

:::

- **System calls**: The core API to talk to the kernel (e.g. `open`, `read`, `write`, `fork`, `execve`).
- **C library**: Offers convenient wrappers around syscalls.
- **POSIX** / Extended Linux APIs.
- **Direct Syscalls**: Rarely done manually in C—usually you call glibc or another library.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamically Linked Libraries (so)

:::

##### Refresh: What Is a Shared Object?

- A file with the “.so” extension that contains compiled code and exported functions.
- Dynamically loaded at runtime by the linker (ld-linux, ld.so).
- Reduces memory footprint and disk usage when multiple processes share the same library code.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Shared Objects in Linux

:::

- Typically built as ELF files with a “shared” type.
- Provide exported functions that your process can call at runtime.
- Examples: `libc.so`, `libm.so`, `libpthread.so`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### The Linux C Runtime (glibc / musl)

:::

- Most common on mainstream distros is **glibc**. Some embedded systems use **musl** or **uclibc**.
- Offers standard C library functions: I/O, string manipulation, memory allocation.
- Sits on top of the Linux syscall interface.
- Maintains compatibility across kernel versions for user apps.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Syscalls

:::

- Undocumented (or lightly documented) from the kernel perspective, but most are stable and well-known.
- Example:
    - `sys_open()` is the kernel function behind `open()` in glibc.
    - `sys_clone()` behind `pthread_create()` or `fork()` depending on usage.
- Typically invoked from user space via wrapper functions in the standard C library.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: Printing to stdout

:::

```c
#include <stdio.h>

int main(){
    printf("Hello world!\n");
    return 0;
}
```

- Under the hood, this calls `write()` on file descriptor 1 (stdout) via glibc’s `printf`.
- On ARM64, glibc uses the `svc #0` instruction to trigger a syscall, passing registers set up for `SYS_write`.

---

## How Do We Interact With the Linux API?

---

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### C Overview

:::

- C/C++ is statically typed, compiled, widely used for systems programming.
- Many Linux-based malware or system tools are in C/C++.
- No garbage collector—developers manually manage memory (`malloc`, `free`).
- We’ll use it for low-level systems programming on ARM64 Linux.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Concepts You Need for This Class

:::

- Basic memory management in C (heap, stack).
- Pointer arithmetic.
- Basic C data types.
- Familiarity with cross-compiling for ARM64 (if developing on x86_64).

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Compiler Toolchain for This Class

:::

- **gcc / g++**: The GNU compiler collection.
- **clang / clang++**: LLVM-based toolchain.
- **zig cc**: Another compiler frontend that can cross-compile out of the box.
- We can use `make` or `CMake` to simplify building.
- Cross-compilation: e.g., `aarch64-linux-gnu-gcc` if you’re on x86_64 but need ARM64 binaries.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Highlight of Common Data Types on Linux

:::

- `size_t` (unsigned integer for sizes, often 64-bit on ARM64).
- `ssize_t` (signed integer for sizes).
- `off_t` (file offsets, can be 64-bit).
- `pid_t` (process ID).
- `uid_t`, `gid_t` (user/group IDs).
- `char*` vs. `wchar_t*` (UTF-8 vs. wide char, though wide char is less common on Linux).


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Development Tools

:::

- a text editor of choice.
- For debugging on Linux, `gdb` or `lldb`.
- For static analysis, `objdump -d`, `readelf -h` or `ghidra`.
- For reverse engineering: Ghidra, Radare2, or Binary Ninja.
- For live debugging an ARM64 binary on x86_64, QEMU-user or other virtualization can be used.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### C, glibc, and Runtimes

:::

- C is a language specification; glibc is an implementation of the standard C library.
- Implementation details can differ (glibc vs musl vs uclibc).
- glibc provides standard functions (e.g., `printf`, `malloc`).
- Linux also supports direct syscalls if needed, but typically you use the C library wrappers (unistd).

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Hello World! POSIX Style

:::

```c
#include <stdio.h>

int main(){
    printf("Hello world!\n");
    return 0;
}
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Hello World via Syscall

:::

```c
#include <unistd.h>
#include <sys/syscall.h>

int main(void) {
    const char msg[] = "Hello, World!\n";
    const size_t len = sizeof(msg) - 1;  // Exclude null terminator

    // AArch64 system call using inline assembly
    asm volatile (
        "mov x8, %[syscall_num]\n\t"     // SYS_write (64) in x8
        "mov x0, %[fd]\n\t"              // STDOUT_FILENO (1) in x0
        "mov x1, %[msg]\n\t"             // Message pointer in x1
        "mov x2, %[len]\n\t"             // Message length in x2
        "svc 0"                          // Invoke system call
        :
        : [syscall_num] "r" (SYS_write),
          [fd] "r" (STDOUT_FILENO),
          [msg] "r" (msg),
          [len] "r" (len)
        : "x0", "x1", "x2", "x8", "memory"
    );

    return 0;
}

```

- Bypasses `printf`, calls the system call wrapper `write()` in glibc.

---
<!-- slide template="[[Base Slide]]" -->

::: title

####  C vs C Runtime

:::

- The C runtime hides the syscall details.
- On Linux, `printf` calls `write()`, which sets up registers for `sys_write` and executes an `svc #0` (on ARM64).
- Using raw syscalls is handy for specialized tasks or for avoiding library overhead
- It is also nice if you  don't know the virtual address of a function 
	- ie shellcodes/payloads 


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Entry Point

:::

- Under the hood, `_start` is the true entry point for ELF binaries.
- `_start` is provided by the C runtime, which initializes things before calling `main()`.
- After `main()` returns, control eventually goes back to `_start`, which calls `exit()`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Code Sample 0:

:::

- We’ll compare a small ARM64 “hello world” compiled _with_ and _without_ the C runtime.
- Tools like `readelf -s` or Ghidra can show symbols in the ELF.
- You’ll see `_start` and possibly references to library init code.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Fundamental Concepts for This Class

:::

- C/C++ runtime on Linux (glibc or musl)
- Linux API (POSIX and syscalls)
- ELF binary format basics

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Ghidra

:::

- An interactive disassembler and decompiler from the NSA, open source and free.
- Great for reverse engineering ARM64 ELF binaries.
- Alternative is IDA Pro (paid).
- We’ll do quick demos in Ghidra to see how compiled code looks in assembly.

---

## Demo 0: Hello world on ARM64

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What Exactly Happened?

:::

- The compiler transforms `.c` source into `.o` object files.
- The linker then combines `.o` files plus libraries into an ELF binary.
- glibc or musl can be statically or dynamically linked.
- The kernel loads the ELF, sets up the stack, and runs `_start`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Converting Source to Binaries

:::

1. **Compilation**
    - Preprocessing (macros, includes).
    - AST generation and machine code emission.
    - Produces `.o` files.
2. **Linking**
    - Resolves symbols (function calls, global variables).
    - Stitches together `.o` files + libraries into a final ELF binary.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Linking: Static vs. Dynamic

:::

- **Static Linking**: Library code is included in the final binary.
    - Larger executables, but no external dependencies needed at runtime.
- **Dynamic Linking**: The linker sets up references to `.so` files loaded at runtime.
    - Smaller binaries, code sharing, but requires the matching shared library installed.

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Shared Object (so) Files

:::

- ELF file with library code (exported functions/symbols)
- Minimizes code duplication in memory.
- Tools like `ldd` show which `.so` dependencies your binary has.
- Common dependencies: `libc.so`, `libpthread.so`, `librt.so`, etc.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Interacting With Linux

:::

- **System calls**: The ultimate gateway to kernel land.
- glibc function calls: Documented, stable, typically prefer these.
- **Direct Syscalls**: Less common, used in specialized or minimal environments.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: Simple I/O

:::

```c
#include <stdio.h>

int main(){
    FILE *fp = fopen("test.txt", "w");
    if(fp){
        fprintf(fp, "Hello from ARM64 Linux!\n");
        fclose(fp);
    }
    return 0;
}
```

- Under the hood, calls `open()`, `write()`, and `close()`.
- `strace`: syscall trace
---

## Demo:

Compiling the Example & Linking Against glibc

