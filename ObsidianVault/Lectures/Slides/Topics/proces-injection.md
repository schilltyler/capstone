

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### `ptrace`: Process Tracing and Manipulation

:::

- The `ptrace()` system call: A powerful tool for interacting with processes.
- it is what powers tools like `gdb`, `strace` and allows us to "break" the rules of normal process isolation
---
<!-- slide template="[[Base Slide]]" -->
::: title

#### `man ptrace`

:::

- **`ptrace()`** is a Linux system call that allows one process (the **tracer**) to observe and control the execution of another process (the **tracee**).
- It provides deep introspection and manipulation capabilities into another process's state.
- **Primary Uses:**
    - **Debugging:** Setting breakpoints, inspecting memory/registers (e.g., `gdb`, `lldb`).
    - **System Call Tracing:** Monitoring interactions with the kernel (e.g., `strace`).
    - **Code Coverage Analysis:** Tracking executed code paths.
    - **Reverse Engineering / Tampering:** Analyzing or modifying program behavior.
    - **Sandboxing:** Intercepting and potentially modifying system calls (though `seccomp` is often preferred now).
    

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Using `ptrace` 

:::

- **Tracer/Tracee:** One process attaches to and controls another.
- **Attaching:**
- **Waiting:** The tracer uses `waitpid(tracee_pid, &status, ...)` to wait for the tracee to stop (e.g., due to a signal, syscall). The `status` variable indicates _why_ it stopped.
- **Detaching:** `ptrace(PTRACE_DETACH, pid, ...)`: Tracer releases control, allowing the tracee to continue normally.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Using `ptrace`

:::

- **Tracer/Tracee:** One process attaches to and controls another.
- **Attaching:**
    - `ptrace(PTRACE_ATTACH, pid, ...)`: Tracer initiates control over an _existing_ process `pid`. Requires appropriate permissions.
    - `ptrace(PTRACE_TRACEME, 0, ...)`: Tracee requests its parent become its tracer (used when launching a new process specifically for debugging).
- **Waiting:** The tracer uses `waitpid(tracee_pid, &status, ...)` to wait for the tracee to stop (e.g., due to a signal, syscall). The `status` variable indicates _why_ it stopped.
    - _Example Code:_ `main()` uses `PTRACE_ATTACH` and `waitpid()`. The `print_status()` helper function decodes the `status`.
- **Detaching:** `ptrace(PTRACE_DETACH, pid, ...)`: Tracer releases control, allowing the tracee to continue normally.
    - _Example Code:_ `inject_so()` finishes with `PTRACE_DETACH`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Debugging with `ptrace`

:::

- **Read/Write Registers:**
- `ptrace(PTRACE_GETREGS/SETREGS, ...)` (older API).
- `ptrace(PTRACE_GETREGSET/SETREGSET, ...)` (newer, uses `iovec`, handles different register sets like FP/SIMD).
- _Example Code:_ `ptrace_getregs()` and `ptrace_setregs()` wrap `GETREGSET/SETREGSET` for AArch64 general-purpose registers (`NT_PRSTATUS`). `print_regs()` displays them.
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Debugging with `ptrace`

:::
    - **Read/Write Memory:**
	- `ptrace(PTRACE_PEEKTEXT/POKETEXT, pid, addr, data)`: Read/write a `long` word at `addr`. "TEXT" implies code, but often works for data too.
	- `ptrace(PTRACE_PEEKDATA/POKEDATA, ...)`: Similar.
	- _Example Code:_ `remote_call()` uses `PEEKTEXT` to save/restore instructions and `POKETEXT` to insert a breakpoint. `inject_so()` uses `POKETEXT` to write the SO path into the target's memory.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Debugging with `ptrace`

:::

- **Single Stepping:** `ptrace(PTRACE_SINGLESTEP, pid, ...)`: Execute one instruction, then stop.
- **Breakpoints:** Often implemented via `POKETEXT` to write a breakpoint instruction (e.g., `int3` on x86, `brk #0` on ARM64) into code, then `PTRACE_CONT`. Tracer waits for `SIGTRAP`.
- **Continuing Execution:** `ptrace(PTRACE_CONT, pid, NULL, signal)`: Resume execution, optionally delivering a signal.

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Our use: Remote Function Call

:::

- `ptrace` allows forcing the tracee to execute a function in its _own_ address space.
- **The `remote_call` Technique (from example code for AArch64):**
    1. **Attach & Stop:** Target process must be stopped via `ptrace`.
    2. **Save State:** Get current registers (`ptrace_getregs` stores in `original`).
    3. **Prepare Return/Breakpoint:**
    4. **Setup Call:** Modify registers (`regs` struct):
    5. **Execute:** Continue the tracee (`PTRACE_CONT`).
    6. **Wait for Trap:** `waitpid()` until the tracee hits the breakpoint (`SIGTRAP`).
    7. **Get Result:** Read the return value (conventionally in `x0`, `regs.regs[0]`).
    8. **Restore State:** Restore the original instruction word (`POKETEXT`), then restore all original registers (`setregs` using `original`).

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Our use: Remote Function Call

:::

- **Prepare Return/Breakpoint:**
	- Choose a "return address" (e.g., the tracee's current Link Register `LR`, stored in `original.regs[30]`).
	- Read the instruction word at that address (`PTRACE_PEEKTEXT`).
	- Overwrite that instruction with a breakpoint (`brk #0` for ARM64, using `PTRACE_POKETEXT`). _Crucial: The target memory page must be writable! If not (e.g., code section), this fails. A trampoline in writable memory might be needed._

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Our use: Remote Function Call

:::

- `ptrace` allows forcing the tracee to execute a function in its _own_ address space.
- **The `remote_call` Technique (from example code for AArch64):**
   1. **Setup Call:** Modify registers (`regs` struct):
	- Set `PC` to the address of the function to call (`func`).
	- Place arguments in registers `x0-x5` (`regs.regs[0]` to `regs.regs[5]`).
	- Set `LR` (`regs.regs[30]`) to the address where the breakpoint was just placed.
	- Apply these registers to the tracee (`ptrace_setregs`).

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Injecting a Shared Object (.so)

:::

- **Goal:** Force a running process to load (`dlopen()`) and execute code from a shared library (`.so`) that it wasn't initially linked with.
- **Why?**
    - **Instrumentation:** Add dynamic logging, performance monitoring.
    - **Hooking/Patching:** Intercept or modify existing function behavior.
    - **Debugging:** Inject specialized debugging helpers.
    - **Malware:** Load malicious code (requires privileges).
- **Core Idea:** Use `ptrace` and the `remote_call` technique to make the target process execute `dlopen()`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Shared Object Injection: Steps

:::

1. **Attach:** Use `PTRACE_ATTACH` to gain control of the target process `pid`.
2. **Find Remote Function Addresses:** Determine the addresses of needed functions (like `dlopen`, `mmap`) within the _target process's_ virtual address space. This is needed due to ASLR (Address Space Layout Randomization).
3. **Allocate Memory in Target:** Use `remote_call` to execute `mmap()` _in the target process_. This allocates a memory buffer within the target's address space.
4. **Write SO Path:** Use `PTRACE_POKETEXT` repeatedly to write the full path string of the shared object file (e.g., `/path/to/myinject.so`) into the remotely allocated buffer.
5. **Call `dlopen` in Target:** Use `remote_call` again to execute `dlopen(remote_buffer_address, flags)` _in the target process_. Common flags include `RTLD_NOW | RTLD_GLOBAL`.
6. **Check Result:** Examine the return value from the remote `dlopen` call (a non-NULL handle indicates success). Handle cleanup

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Shared Object Injection

:::

- **Finding Remote Functions (`dlopen`, `mmap`):**
    - Find libc base address in _local_ (injector) process (`get_local_libc_base` uses `dladdr`).
    - Find libc base address in _remote_ (target) process (`get_remote_libc_base` parses `/proc/pid/maps`).
    - Get function address _locally_ using `dlsym(RTLD_DEFAULT, "function_name")`.
    - Calculate _remote_ function address using the offset: `remote_addr = remote_libc + (local_addr - local_libc)`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Shared Object Injection: 

:::

- **Allocating Remote Memory:**
    - The example uses `remote_call` to invoke the calculated `remote_mmap` address in the target. Arguments specify size (4096), protection (`PROT_READ | PROT_WRITE`), flags (`MAP_PRIVATE | MAP_ANONYMOUS`), etc.
    - `remote_buf = remote_call(target_pid, remote_mmap, ...)`
    - we find the address via /proc/pid/maps, and use the offset
    -

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Shared Object 

:::
- **Writing Path:**
    - A loop uses `PTRACE_POKETEXT` to copy the `so_path` string, `sizeof(long)` bytes at a time, into the `remote_buf`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Shared Object Injection: 

:::

- **Executing Remote `dlopen`:**
    - Uses `remote_call` with the calculated `remote_dlopen` address, passing `remote_buf` (containing the path) as the first argument and `dlopen_flags` as the second.
    - `handle = remote_call(target_pid, remote_dlopen, remote_buf, dlopen_flags, ...)`

---

<!-- slide template="[[Base Slide]]" -->
::: title

#### Example Code Overview: `inject.c`

:::

- The provided C code is a concrete example of `.so` injection using `ptrace`.
- **Target Architecture:** Specifically written for **AArch64 (ARM64)**.
    - Uses `struct user_pt_regs` matching ARM64 registers (`x0`-`x30`, `sp`, `pc`, `pstate`).
    - Uses the ARM64 software breakpoint instruction (`brk #0`, value `0xd4200000`).
    - Relies on AArch64 calling convention (args `x0-x7`, return `x0`, link register `x30`).
- **Key Functions:**
    - `remote_call()`: Implements the function call primitive.
    - `inject_so()`: Main logic for finding functions, allocating memory, writing path, and calling `dlopen`.
    - `get_remote_libc_base()`, `get_local_libc_base()`: Handle ASLR by finding libc mappings.
    - `ptrace_getregs()`, `ptrace_setregs()`: Wrappers for `PTRACE_GETREGSET/SETREGSET`.
- **Usage:** `./injector /full/path/to/your/lib.so <target_pid>`


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### DEMO

:::


![[sandy_ptrace.png]]


---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  ptrace ch0nky

::: 
::: left 
- `zygote` spawns `ch0nky_proxy` (fork)
- `ch0nky_proxy` spawns `ch0nky_app` (fork)
- `ch0nky_proxy` can `ptrace` `ch0nky_app`!
::: 
::: right
![[olive_oil_ptrace.png]]

:::
---

<!-- slide template="[[Base Slide]]" -->
::: title

#### Security Considerations & Limitations

:::

- **Privileges Required:** Attaching to an arbitrary process (`PTRACE_ATTACH`) typically requires:
    - Root privileges (`sudo`).
    - The `CAP_SYS_PTRACE` capability.
    - A permissive `ptrace_scope` setting (`/proc/sys/kernel/yama/ptrace_scope`). A value of 0 is most permissive; higher values restrict tracing (e.g., only child processes).
- **Architecture Specificity:** Low-level details like register names, calling conventions, and breakpoint instructions are architecture-dependent (e.g., x86-64 differs significantly from AArch64). The _concepts_ are similar, but code is not portable without changes.
- **Anti-Debugging:** Programs can detect tracing attempts (e.g., timing checks, checking `ptrace_scope`, trying `PTRACE_TRACEME`).

---

<!-- slide template="[[Base Slide]]" -->
::: title

#### Conclusion

:::

- `ptrace` provides fundamental capabilities for process observation and control in Linux.
- It underpins essential tools like debuggers (`gdb`) and tracers (`strace`).
- Advanced techniques, demonstrated by the example code, allow remote function execution and dynamic library injection.
- These techniques require careful management of process state (registers, memory), understanding of low-level architecture details (calling conventions, instruction encoding), and handling ASLR.
- `ptrace` is powerful but intrusive, necessitating strict permission models (`ptrace_scope`, capabilities) for system security.



![[ptrace_hannibal.png]]
![[pat_mmap.png]]



![[ptrace_squidward.png]]


![[bmo.png]]

