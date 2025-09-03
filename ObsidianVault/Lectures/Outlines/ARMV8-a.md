


- Computer architecture 101
- What is an operating system 
	- Recommended reading: 3 easy pieces 
	- "OS as a fancy resource manager"
	- "OS as an API"
-  The linux operating system 
	- Basic Features
		- Virtual Memory
		- Virtual CPU/Virtual Threads
		- Logical interface for interacting with hardware
		- File systems as trees 
		- "the shell"
		- "everything is a file"
			- profcs
			- file descriptors 
				- Honestly i like windows language more: handle (handle to kernel object/resource)
			- But you an access most things via the file system
- User land vs Kernel
	- privilege levels
	- How are privilege levels implemented? 
		- interrupts/traps/exceptions..etc
- Crossing Boundaries via "syscall"
- 
- CPU as an abstract register machine 
- Programming the CPU 
- CISC/RISC architectures 
- Where Do we see RISC
	- Smaller devices
- ARM disambiguation 
	- the company 
	- the Family of Architectures and Micro Architectures 
- ARMv8
	- A (application)
	- Other families
- Abstract Register Machine 
- ArmV8-A Assembly
	- aarch64--> A64/A32
	- extensions?
- Focusing on ArmV8-a specifically looking at 
	- Arm Cortex-A76 CPU
	- 




# using  binutils 

````bash
objdump -T  myelf


````