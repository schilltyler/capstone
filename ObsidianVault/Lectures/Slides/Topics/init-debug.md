
---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### CS-4973
:::
::: left
![[Green Logo.svg | 400]] <!-- element style="object-fit: cover" -->
:::
::: right
<!-- element style="font-size: 32px; align:center" -->
Introduction to Malware, Threat Hunting & Offensive Capabilities Development (Linux Edition)
:::


---
<!-- slide template="[[Title Slide]]" -->
## Dynamic Analysis (Linux Focus)
---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Lecture 0x06: Dynamic Analysis on Linux
:::

::: left
<!-- element style="font-size: 24px"-->
- Dynamic Analysis: Actually **running** or **emulating** the suspicious code in a contained environment.  
- Typically we watch the malware’s every move: file I/O, process creation, registry-like equivalents (in Linux, think of system configs), network connections, etc.
- i.e., running with introspection

:::

::: right

![[wait-for-c2.png|300]]

:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Dynamic Analysis
:::

- Examples in Linux:
  - Attaching `gdb` to a running process to step through code.
  - Using snapshots or containers/VMs to observe changes in the file system or the OS state.
  - Running `Wireshark` to see network activity in real-time.
  - Using `strace` to monitor system calls and signals.
  - Using `ltrace` to monitor library calls
  - bpbtrace, frida and other tools that can be used to hook functions
  - Using `inetsim` to simulate internet services so your malware doesn’t go on a real-world rampage.

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Recall: What Are We Usually Interested In?
:::

- Malware usually has a goal—like exfiltrating data, performing cryptomining, or bricking your copmuter
- We need to figure out **how** it accomplishes that goal to stop it or protect against it.

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Questions We Usually Want Answered
:::

![[CommonMalwareQuestions]]



---
<!-- slide template="[[Base Slide]]" -->

::: title

####  What took look for 

:::
1. What files does it modify or create?  
2. What network connections does it establish?  
3. Does it persist across reboots (systemd services, cron, etc.)?  
4. Does it check if it’s in a VM/container?  
5. Where does it hide its malicious code (memory, hidden files, etc.)?


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Debuggers (Linux Style)
:::

<!-- element style="font-size: 24px"-->
- **GDB (GNU Debugger)**: The classic userland debugger for Linux. Attach to processes, set breakpoints, step, watch memory, etc.
- **KGDB**: For kernel debugging. (You probably don’t want to go down that rabbit hole unless the malware is messing with the kernel)
- **Hypervisor Debuggers**: For more hardcore analysis if you suspect it’s messing with system-level instructions or using advanced VM detection.  

Most **Linux** malware is userland, so GDB is typically enough to start with.



---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Getting stated

:::
- lets revisit sample2 from last week
- from our static analysis, we know it seems to be some sort of shell 
- lets use `strace` to see what syscalls it is using
- after lets use `ltrace`
---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### GDB 101
:::

::: left
- We’ll focus on GDB for this class.  
- Yes, it’s old. 
- It’s definitely not as flashy as some modern GUIs, 
- you can run it basically everywhere 
- IDA pro is too expensive
- its extensible and we can make it more user friendly  

:::

::: right
![[gdb-old.png]]
:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Sample2
:::

- lets revisit sample2 from last week
-  We suspect it’s malicious.  
- Let’s attach GDB and see how to poke around.

---
<!-- slide template="[[Base Slide]]" -->
::: title
####
:::

- Similar to Windows debuggers, GDB will show you loads of info: registers, breakpoints, memory regions, etc.
- If your eyes glaze over, don’t worry. Happens to the best of us.

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Debug loop
:::

::: left
- set breakpoints on common libc functions
- watch memory that might be written to of interest
- perform some static analysis to figure out where to being debuggin
:::

::: right

![[gru-debug.png]]

:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### GDB Basics
:::

<!-- element style="font-size: 24px"-->
- **Breakpoints**:  
  - `break main` sets a breakpoint at `main`.  
  - `break *0x400d40` sets a breakpoint at a specific address.  
- **Run**:  
  - `run` starts or restarts the program from the beginning.  
- **Step** / **Next**:  
  - `step` goes one instruction, stepping into function calls.  
  - `next` runs the next line (stepping over function calls).  
- **Continue**:  
  - `continue` runs until the next breakpoint.

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Viewing Memory Maps in GDB
:::

::: left
<!-- element style="font-size: 20px"-->
- In Linux, each process has a `/proc/<pid>/maps`. GDB can show this via:
  - `info proc mappings` (if supported)  
  - or just open another terminal and `cat /proc/<pid>/maps`.  
- You can also do `info files` in GDB to see what sections are mapped if symbols are loaded.

:::

::: right
```
(gdb) info files
Symbols from "/home/user/c5e4d7c93f3f03bc0e0c135c4d9da183d4a120cfa3bdf42b249af31fe7f08fdb".
Native process:
	Using the running image of child process 7372.
	While running this, GDB does not access memory from...
Local exec file:
	`/home/user/c5e4d7c93f3f03bc0e0c135c4d9da183d4a120cfa3bdf42b249af31fe7f08fdb', file type elf64-littleaarch64.
	Entry point: 0xaaaaaaaa0e00
	0x0000aaaaaaaa0238 - 0x0000aaaaaaaa0253 is .interp
	0x0000aaaaaaaa0254 - 0x0000aaaaaaaa0278 is .note.gnu.build-id
	0x0000aaaaaaaa0278 - 0x0000aaaaaaaa0298 is .note.ABI-tag
	0x0000aaaaaaaa0298 - 0x0000aaaaaaaa02b4 is .gnu.hash
	0x0000aaaaaaaa02b8 - 0x0000aaaaaaaa0630 is .dynsym
	0x0000aaaaaaaa0630 - 0x0000aaaaaaaa078e is .dynstr
	0x0000aaaaaaaa078e - 0x0000aaaaaaaa07d8 is .gnu.version
	0x0000aaaaaaaa07d8 - 0x0000aaaaaaaa0808 is .gnu.version_r
	0x0000aaaaaaaa0808 - 0x0000aaaaaaaa08c8 is .rela.dyn
	0x0000aaaaaaaa08c8 - 0x0000aaaaaaaa0bc8 is .rela.plt
	0x0000aaaaaaaa0bc8 - 0x0000aaaaaaaa0be0 is .init
	0x0000aaaaaaaa0be0 - 0x0000aaaaaaaa0e00 is .plt
	0x0000aaaaaaaa0e00 - 0x0000aaaaaaaa160c is .text
	0x0000aaaaaaaa160c - 0x0000aaaaaaaa1620 is .fini
	0x0000aaaaaaaa1620 - 0x0000aaaaaaaa1705 is .rodata
```
:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Memory Protections
:::

::: left
<!-- element style="font-size: 18px"-->
-  **Read**, **Write**, **Execute** (R, W, X).  
- Linux uses `mprotect` calls to change protections on memory pages.  
- If you suspect the malware is injecting code, watch for pages marked with RWX or newly changed to RX.

:::

::: right

:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Watching Memory with GDB
:::

::: left
- Use **watchpoints** to break when a variable or memory address changes.  
  - `watch *0xdeadbeef`  (Hex addresses, not recommended for your breakfast)  
  - GDB stops execution the moment that address is modified.  

:::

::: right
:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Changing Function Arguments
:::
::: left
- arguments are passed in `x0-x7`
- In GDB, you can do `set $x0 = 0` to force the first argument to be zero.  
- This is super handy if malware tries to do something silly like sleep forever.
:::
::: right
![[gdb-clock.png|250]]
:::
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Strace
:::

<!-- element style="font-size: 24px"-->
- **Strace** : it logs **system calls** made by a process.  
- E.g. `strace ./EvilPenguin` might reveal calls to `open`, `connect`, `execve`, etc.  
-  `strace -f` follows child processes.



---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### striped binaries 
:::

::: left
- `starti`
- `info files`
- `b *<addr_start>`
- `b __libc_start_main`
- `b __libc_start_main@plt`
- ghidra --> (make sur eto relocate)
:::
::: right
![[gdb-meme.png|350]]
:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Network Analysis with Wireshark
:::

- If the malware tries to phone home you can catch it in the act using network analysis tools 
- for this class, we will use `tshark` to capture and `wireshark` to analyze the results
- Filter for suspicious traffic, watch for weird domains or IP addresses.  
- Great for discovering if it’s exfiltrating data or retrieving additional payloads.
- what happens if it uses TLS?

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Using Inetsim
:::

- **Inetsim** simulates common services (HTTP, FTP, DNS, etc.).  
- Let your malware think it’s talking to the real world, but in reality it’s talking to a sneaky decoy.  
- Perfect for analyzing how it communicates, what data it sends, or if it tries to download more goodies (malicious ones, obviously).

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Dynamic Analysis Workflow
:::

1. **Snapshot** or clone your VM/container to revert later (safety net!).  
2. **Initial Recon**: Use `strings`, `file`, `ldd`, or `objdump -x` on the binary.  
3. **Set Up Tools**: GDB, strace, Wireshark, inetsim.  
4. **Run the Malware**: Possibly attach GDB or run under `strace`.  
5. **Observe**: Network connections, file writes, processes spawned.  
6. **Document**:  **this is also advice to myself** 
7. **Revert**: Return your environment to a safe snapshot.

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Putting it All together
:::

- DEMO
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Sandboxes & Public Repos
:::

<!-- element style="font-size: 24px"-->
- **Remnux**: A popular Linux distro pre-loaded with analysis tools.  
- **VirusTotal**: You can upload suspicious files to see if AV engines detect them, but be careful—uploading them might tip off threat actors that you’re onto them.  
- **Hybrid-Analysis**, **Malshare**, etc.: More community-based sources of samples and analyses.

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### DIY Sandbox on Linux
:::

<!-- element style="font-size: 24px"-->
- Why do it yourself?
  - You want privacy—uploading to VirusTotal might expose your intel to the public (or the criminals).  
  - You can tailor the environment to mimic your real server or dev environment.  
  - You can trick the malware better (use real environment variables, same kernel version, etc.).

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### DIY Sandbox Steps
:::
 DEMO
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Warning: Operational Security
:::

- If you connect your VM to the internet, the malware might roam free.  
- But what could go wrong?... 
- Always isolate your environment or use inetsim to keep your analysis contained.

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Why Dynamic Analysis Might Fail
:::

- Malware authors get sneaky: they detect VMs, check if GDB is attached, or see if the environment “feels real.”  
- They can wait or “sleep” for days, expecting you’ll quit your analysis.  
- They might deploy decoys that do nothing malicious while the real payload is hidden elsewhere.

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Making Reverse Engineering Harder
:::

- Attackers can:
  - **Obfuscate** strings with encryption.  
- pack binaries
- anti-sandbox checks
  - Use unusual syscalls/direct syscalls.  
  - “Sleep” for ages.  
  - waist your time reversing something. triage vs research 
---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Triage Environment
:::

::: left
- If you’re hammered by wave after wave of random Linux malware:
  - You just want the **IOCs** fast (file hashes, IP addresses, malicious domains).  
  - You might not have time to dissect it thoroughly.
  - This can turn into a never ending game of whack a mole 
:::

::: right
![[kafka.png|300]]
:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Example: Generic Malspam
:::

- Spammers sometimes target Linux web servers or shared hosting   
- They’re often recycled, just changing some domain or minor code.  
- Full in-depth reversing might not be worth it if you just need an IP block list or a quick filter rule.

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Research/Investigative Environment
:::

- You dig deeper if:
  - It’s super advanced or uses 0-days.  
  - It’s specifically targeting your environment or high-value servers.  
  - Your boss demands a thorough breakdown.  
  - you can't help yourself :)

:::


---
<!-- slide template="[[Title Slide]]" -->
## Efficiency is Key
---

<!-- slide template="[[Base Slide]]" -->
::: title
#### The Reverse Engineer Always Wins*
:::

- *Given enough time, coffee, and frustration .*  
- Malware tries to slow you down, but you can automate tasks, script GDB commands, etc.  
- The real question is **trade-offs**: time vs. depth.  

---
<!-- slide template="[[Title Slide]]" -->
## Dealing with Anti-Analysis Tricks
---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Discussion
:::

- How can malicious code detect it’s in a container or VM?  
  - Checking `dmidecode`, or `/proc/cpuinfo` for “Virtual,” or certain QEMU device strings.  
  - Checking for special MAC addresses associated with VMs.  
- How can you fool them?  
  - Changing VM settings, randomizing hardware info, *cackling maniacally, etc.*

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Wen Does the Malware win 
:::

- it only needs to be alive for $x<y$ and you spend $y$ on it 
- it is cryptographically tailored and you don't have the key
- you dismiss something interesting as something pedestrian 
- you give up
	- ie you run out of time, money or energy