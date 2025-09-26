---
slideNumber: true
theme: night
---

---
<!-- slide bg="[[Green Logo.svg]]" background-size="auto" -->


---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### CS-4973/6983
:::
::: left
![[Green Logo.svg | 400]] <!-- element style="object-fit: cover" -->

:::
::: right
<!-- element style="font-size: 32px;align:center" -->
Introduction to Malware, Threat Hunting & Offensive Capabilities Development
:::

---

<!-- slide template="[[Title Slide]]" -->
## Lecture 0x02: 
- Introduction to IOCs and basic  **Static Analysis** on Linux/aarch64
- typos, typos and more tpyos :)


---
<!-- slide template="[[Split Vertical]]" -->
::: title

#### CrackMe HW Is up!

:::

::: left

- You should get started on this ASAP.
- In this assignment, you’ll reverse engineer several  Linux (ARM64) binary puzzles.
- The goal is simple: Find the input that triggers the “Cracked!” message.
- You will get comfortable reading some basic assembly,  and performing common reverse engineering tasks.
	- eg find main, imports strings...etc
- You don’t need to run it—but you can, to verify your answer!

:::

::: right
![[0x04_slide_77_image.png]]

:::


---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Agenda for Today
:::

- Baby's first (compiled!) Malware (Linux/aarch64)
- This class will build on the Python malware from last lecture
- Keep the following question in the back of your mind: 
  - **What is needed to control a (Linux) computer?**
- Pre-Req: familiarity with ELF binaries, hex/base64 encoding, and C-strings

---

<!--slide template="[[Title Slide]]"-->
- Today we will talk about concepts that have yet to be rigorously defined 
- This lecture is all about building intuition for **triaging** malware


---

<!-- slide template="[[Title Slide]]" -->
## Reminder!
Different entities also use different jargon!

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Class Progression (Defenders)
:::

1. **Analyzing Malware**  
   - Pull out IOCs from the malware
   - *Example:* Find all C2 domains, interesting files, configurations, imports
2. **Write a Technical Report**  
   - Explain how it works and the significance of the IOCs
   - *Example:* What is the malware trying to accomplish?
3. **Create Threat Hunting Rules**  
   - Find other malware connected to the same threat group
   - *Example:* Identify all code related to APT “Chonky Bear” across a big binary repo

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Class Progression (Defenders)
:::

- Analysis of first-stage loaders
- More advanced static/dynamic analysis
- Refining detection and hunting rules

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Malware Analysis 101
:::

- **Static Analysis**: Analyzing the code without running/emulating it, to determine functionality and capabilities  
- **Dynamic Analysis**: Running/emulating all or parts of the code to see its *real* runtime behavior
- Today’s focus: **basic static analysis** (ELF on aarch64)

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### What Are We Actually Interested In?
:::

![[CommonMalwareQuestions]]


---
<!-- slide template="[[Split Vertical]]" -->
::: title
##### How Do We Begin to Answer These Questions?
:::

::: left
<!-- element style="font-size: 24px"-->
- **Tear the malware apart** and see what it does!
:::

::: right
![[0x03_slide_20_image.png|300]]
:::

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Purpose of Static Analysis
:::

::: left
- Identify potential IOCs (often easier)
- Determine functionality (can be hard)
- Keep eye doctors in business:
  - **Dark mode** recommended for long hours in Ghidra/radare2
:::

::: right
![[0x03_slide_21_image.png|600]]
:::

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Static Analysis
:::

- **Brief**: Analyze code without running it  
- **Longer**: Inspect ELF headers, sections, symbols, and embedded data. Extract IOCs if it’s malicious
- Let’s introduce ELF for Linux aarch64

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Executable File Formats
:::

- Bundles of machine code/data needed to run a program
- Usually requires an OS loader (on Linux: `ld-linux-*`) to load and link
- Code is organized according to an agreed-upon convention between the compiler/linker and the kernel loader (the ELF standard on Linux)

![[basic_elf]]

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Analysis Playbook
:::


![[sa_playbook.png]]


---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Finding the Real “Main” Function
:::

- Many compiled C programs have startup code for `__libc_start_main` or security checks
- “Main” might *not* be the first function to run
- Trickier malware can hide real entry logic in constructors or custom linker sections
- Tools like Ghidra let you follow from `_start` → see where main is eventually called

---

<!-- slide template="[[Title Slide]]" -->
## Demo: 
3 Ways to Find “Main” in an ELF (aarch64)!

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### General Tips
:::

- Look for references to suspicious strings
- Look for references to imported symbols (especially `socket()`, `execve()`, `system()`)
- For standard C++ code, “main” typically returns an `int`
- Practice on known compiled test binaries and see if you can locate main

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### When is This Methodology Difficult?
:::

- **Packed/Obfuscated** binaries: code is compressed or encrypted (e.g., UPX, custom packers)
- **Dynamic resolution** of imports: using `dlsym` or manual syscalls
- We’ll spend more time on these advanced topics later

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### UPX on Linux? Yes, It Happens
:::

::: left
<!-- element style="font-size: 24px"-->
- e.g., `upx -9 mybinary` can compress ELF files
- This is a common enough packer that many security tools flag “UPX-packed” as potentially malicious
:::

::: right
![[0x03_slide_52_image.png|300]]
:::



---

<!-- slide template="[[Title Slide]]" -->

#### Demo

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Discussion: Reading Documentation
:::

- How do we control different aspects of the Linux OS from userland?
- What exactly is the OS’s role?  
- How is Linux different from Windows or iOS, from a malware perspective?

---


