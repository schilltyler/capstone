
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### (Some) Linux/Unix Design Principles

:::

- Open source
- Portability
- Modularity (kernel modules, subsystems)
- Performance
- Security (mandatory or discretionary)
- Stability (POSIX compliance, stable user APIs)
- UNIX --> "Small" well defined APIs 

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Extensibility

:::

- Most Linux functionality lives in kernel subsystems (VFS, networking stack, device drivers...etc).
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
- The Hardware Abstraction Layer (HAL)  is typically integrated directly in architecture subdirectories (arch/arm64, arch/x86, etc.) within the kernel.

---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Reliability

:::

::: left 
- User applications cannot typically crash the kernel due to robust memory isolation.
- Kernel module signing and Secure Boot exist to mitigate malicious or buggy LKMs.
- However, a kernel module can still crash the system if it’s badly coded—so caution is necessary.

:::
::: right
![[memory-protections.png]]
:::


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
- The user-space ABI (Application Binary Interface) is quite stable
	- But differences in LIBC versions, file system layouts ..etc can make creating a "build once  anywhere" binaries for distros challenging 
- The kernel internal APIs can evolve more frequently, but glibc and userland see fewer breaking changes compared to kernel modules.
- ...this is of course only in theory.
	- syscall changes, libc versions,  distro fragmentation ...etc

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Security

:::
- Separation of privilege 
	- Separation of kernel and user mode.
- User/Group/Other permissions, SELinux or AppArmor for MAC.
- Each process has its own private virtual address space
	- I.e.e the view of the system from a process should be private
