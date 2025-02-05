




---
<!-- slide template="[[Title Slide]]" -->
# `man elf`

![[Pasted image 20250205120818.png]]

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Agenda: ELF

:::
- Reading the docs  
- Parsing
- Basic loading 
	- Goal by end of class is to build a loader that can handle statically linked elves 

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Structure of an ELF: Header

:::
- Recap: 
	- header: where is my stuff
	- Segments: the stuff in memory
	- Sections: mostly used for linking and not suuuuper relevant to loading
- Elf header
- Collection of file offsets 
- indicates the type of elf, where the  loader can find information needed to run the elf



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Terminology

:::
- `elf file` the file as it exists on disk
- `elf buffer/elf_file_bytes` the file as it exists on disk read into memory. 
- `elf image` the region of memory that houses the memory mapped segments of an elf

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Sample Program: Static 

:::

```c
//cat src/static_write.c 
#include "syscall_wrapper.h"
#include <unistd.h>

/*
 *
// compile
aarch64-linux-gnu-gcc src/static_write.c -static -nostdlib  -Iinc/ -o
bin/static_write
// run
qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/static_write
 * */

int _start() {
  const char msg[] = "Hello world!\n";
  sys_write(0, msg, sizeof(msg));
  sys_exit(0);
  return 0;
}

```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Sample Program: Dynamic

:::
```c
#include <stdio.h>

extern void _start(void); // Declare _start as an external symbol

void prepend() {
  printf("woah I ran first. wonder what this means :)\n");
  _start();
}
void unused_func() {
  printf("Woah how did I get here?\n");
  return;
}

int main() {
  printf("Hello world!\n");
  return 0;
}

```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `elf.h` 

:::
- Skim through elf.h
- make sure to read `man elf`


---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  ELF Header 

::: 
::: left 
- Used in part for verifying the file format type
	- I.e., does the arch match, what kind of elf do we have, how is it linked 
- Used to find the entrypoint 
- Used to find the `Program Header` (if it exists--which usually it does)
::: 
::: right

![[elf_header.png]]
:::


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### ELF Header 

:::
![[elf_header.excalidraw]]




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example program

:::
Where is the entry point?
```
readelf -h bin/ex_elf_entry 
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              DYN (Position-Independent Executable file)
  Machine:                           AArch64
  Version:                           0x1
  Entry point address:               0x680
  Start of program headers:          64 (bytes into file)
  Start of section headers:          7184 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         9
  Size of section headers:           64 (bytes)
  Number of section headers:         28
  Section header string table index: 27


```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Continued

:::

```
$ objdump -D bin/ex_elf_entry 
...
0000000000000680 <_start>:
 680:   d503201f        nop
 684:   d280001d        mov     x29, #0x0                       // #0
 688:   d280001e        mov     x30, #0x0                       // #0
 68c:   aa0003e5        mov     x5, x0
 690:   f94003e1        ldr     x1, [sp]
 694:   910023e2        add     x2, sp, #0x8
 698:   910003e6        mov     x6, sp
 69c:   90000080        adrp    x0, 10000 <__FRAME_END__+0xf694>
 6a0:   f947f800        ldr     x0, [x0, #4080]
 6a4:   d2800003        mov     x3, #0x0                        // #0
 6a8:   d2800004        mov     x4, #0x0                        // #0
 6ac:   97ffffd1        bl      5f0 <__libc_start_main@plt>
 6b0:   97ffffdc        bl      620 <abort@plt>


```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Sanity check

:::
- Build this program and  use `readelf` to determine the entrypoint
- write a small c program called `patch_entry` that takes the binary name, the new entry point file offset, and creates a new binary called `<binary>.patched` that changes the entry point 
	- Optionally, call the main function again after
- use `objdump -D` to find an offset to the new start. try it out with both
	- `prepend` and `unused_func`
- For parsing:
	- ` unsigned long new_entry = strtoul(argv[2], NULL, 16);`
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Patching the entrypoint

:::
```
//$ aarch64-linux-gnu-objdump -D bin/ex_elf_entry | grep unused
// Update the entry point
ehdr.e_entry = new_entry;

 * $ qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/patch_entry bin/ex_elf_entry 
 * $ qemu-aarch64 -L /usr/aarch64-linux-gnu/ bin/ex_elf_entry.patched $
 * $ chmod +x bin/ex_elf_entry.patched
 * $ qemu-aarch64 -L /usr/aarch64-linux-gnu/
 * bin/ex_elf_entry.patched
```
- Question: What do we think will happen with each one?



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Program Header

:::
- `man elf`

```
 Program header (Phdr)
       - An executable or shared object file's program header table is an array of structures,
       - each describing a segment or other information the system  needs  to
       prepare  the  program for execution.  
       - An object file segment contains one or more sections.  
       - Program headers are meaningful only for executable and shared
       object files.  A file specifies its own program header size with the ELF header's e_phentsize and e_phnum members.  The ELF program header is described by
           typedef struct {
               uint32_t   p_type;
               uint32_t   p_flags;
               Elf64_Off  p_offset;
               Elf64_Addr p_vaddr;
               Elf64_Addr p_paddr;
               uint64_t   p_filesz;
               uint64_t   p_memsz;
               uint64_t   p_align;
           } Elf64_Phdr;

```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### ELF Header $\rightarrow$ Program Header  

:::
- Base address: (\*ELF Header)
- `e_phoff` "program header offset"
- \*Program Header = Base address + elfhdr->e_phoff
- `elfhdr->e_phentsize`: byte size of program header entries
- `elfhdr->e_phnum` : number of program header entries



---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Program Header Types

:::

- **PT_NULL**: Unused entry; values undefined.
- **PT_LOAD**: Loadable segment; file bytes mapped to memory.
- **PT_DYNAMIC**: Contains dynamic linking information.
- **PT_INTERP**: Specifies interpreter pathname (for executables).
- **PT_NOTE**: Holds auxiliary note information.
- **PT_SHLIB**: Reserved; semantics unspecified.
- **PT_PHDR**: Specifies program header table location & size.
- **PT_LOPROC - PT_HIPROC**: Reserved for processor-specific use.
- **PT_GNU_STACK**: GNU extension to control stack state.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Relevant Types For Static

:::
- PT_LOAD: information about a segment 
- provides information about  segment start  and end 
	- both Physical and Virtual
	- This means we have the size of the segment
- Here: physical address is a file offset 
- Provides information about memory protections 
- Question: is virtual size = physical size? 

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `readelf -l` 

:::
- statically linked binary

```
readelf -l bin/static_write 

Elf file type is EXEC (Executable file)
Entry point 0x4001b4
There are 3 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x00000000000002ac 0x00000000000002ac  R E    0x10000
  NOTE           0x00000000000000e8 0x00000000004000e8 0x00000000004000e8
                 0x0000000000000024 0x0000000000000024  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10

 Section to Segment mapping:
  Segment Sections...
   00     .note.gnu.build-id .text .rodata .eh_frame 
   01     .note.gnu.build-id 
   02     
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What if we change it? 

:::

```c
#include "syscall_wrapper.h"
#include <unistd.h>

const char msg[] = "Hello world!\n";
int _start() {

  sys_write(0, msg, sizeof(msg));
  sys_exit(0);
  return 0;
}

```
- How many segments do we expect of type PT_LOAD?
- What permissions do we expect?



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `readelf -l: ro`


:::
- Wait what? why is there only 1 segment?

```
readelf -l  bin/static_write_ro                                                                  

Elf file type is EXEC (Executable file)
Entry point 0x4001b4
There are 3 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x0000000000000294 0x0000000000000294  R E    0x10000
  NOTE           0x00000000000000e8 0x00000000004000e8 0x00000000004000e8
                 0x0000000000000024 0x0000000000000024  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10

 Section to Segment mapping:
  Segment Sections...
   00     .note.gnu.build-id .text .rodata .eh_frame 
   01     .note.gnu.build-id 
   02     
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Sections? Segments?

:::
- oh --- It is making a separate section but it can store it in the same segment

```
readelf -S bin/static_write_ro  
There are 9 section headers, starting at offset 0x5b8:

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .note.gnu.bu[...] NOTE             00000000004000e8  000000e8
       0000000000000024  0000000000000000   A       0     0     4
  [ 2] .text             PROGBITS         000000000040010c  0000010c
       00000000000000d8  0000000000000000  AX       0     0     4
  [ 3] .rodata           PROGBITS         00000000004001e8  000001e8
       000000000000000e  0000000000000000   A       0     0     8
  [ 4] .eh_frame         PROGBITS         00000000004001f8  000001f8
       000000000000009c  0000000000000000   A       0     0     8
  [ 5] .comment          PROGBITS         0000000000000000  00000294
       000000000000002b  0000000000000001  MS       0     0     1
  [ 6] .symtab           SYMTAB           0000000000000000  000002c0
       0000000000000228  0000000000000018           7    14     8
  [ 7] .strtab           STRTAB           0000000000000000  000004e8
       000000000000007c  0000000000000000           0     0     1
  [ 8] .shstrtab         STRTAB           0000000000000000  00000564
       000000000000004f  0000000000000000           0     0     1
...
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example 2: `rw` 

:::

- What about this?

```
#include "syscall_wrapper.h"
#include <unistd.h>


 char msg[] = "Hello world!\n";
int _start() {

  sys_write(0, msg, sizeof(msg));
  sys_exit(0);
  return 0;
}

```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `readelf -l`: `rw`

:::

- because the data is not marked as read only, we need a separate segment to store it 
- we don't want the segment containing our executable code to be writable

```c
 readelf -l  bin/static_write_rw 

Elf file type is EXEC (Executable file)
Entry point 0x400224
There are 5 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x0000000000000304 0x0000000000000304  R E    0x10000
  LOAD           0x0000000000000fd8 0x0000000000410fd8 0x0000000000410fd8
                 0x0000000000000028 0x0000000000000028  RW     0x10000
  NOTE           0x0000000000000158 0x0000000000400158 0x0000000000400158
                 0x0000000000000024 0x0000000000000024  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x0000000000000fd8 0x0000000000410fd8 0x0000000000410fd8
                 0x0000000000000028 0x0000000000000028  R      0x1

 Section to Segment mapping:
  Segment Sections...
   00     .note.gnu.build-id .text .rodata .eh_frame 
   01     .got .got.plt 
   02     .note.gnu.build-id 
   03     
   04     .got .got.plt 
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Exercise 

:::
- Consider the following:

```
readelf -l bin/static_write 

Elf file type is EXEC (Executable file)
Entry point 0x4001b4
There are 3 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x00000000000002ac 0x00000000000002ac  R E    0x10000
  NOTE           0x00000000000000e8 0x00000000004000e8 0x00000000004000e8
                 0x0000000000000024 0x0000000000000024  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
```

how could we modify the second segment of type NOTE to provide us with `RWX` scratch space?


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Solution

:::

```c
typedef struct {
	   uint32_t   p_type;
	   uint32_t   p_flags;
	   Elf64_Off  p_offset;
	   Elf64_Addr p_vaddr;
	   Elf64_Addr p_paddr;
	   uint64_t   p_filesz;
	   uint64_t   p_memsz;
	   uint64_t   p_align;
   } Elf64_Phdr;
phdr->p_type = PT_LOAD;
phdr->p_memsz = 0x1000;
phdr->p_flags= <read, write, executable> // how??


```



---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  `man elf`

::: 
::: left 

```bahs 
man elf
....
p_flags
 This member holds a bit mask of flags relevant to the segment:
	  PF_X   An executable segment.
	  PF_W   A writable segment.
	  PF_R   A readable segment.
```

:::

::: right
![[Pasted image 20250205125925.png]]
:::

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Solution

:::

```c
typedef struct {
	   uint32_t   p_type;
	   uint32_t   p_flags;
	   Elf64_Off  p_offset;
	   Elf64_Addr p_vaddr;
	   Elf64_Addr p_paddr;
	   uint64_t   p_filesz;
	   uint64_t   p_memsz;
	   uint64_t   p_align;
   } Elf64_Phdr;
phdr->p_type = PT_LOAD;
phdr->p_memsz = 0x1000; // make it as big as you want i guess
// but becareful if other segments come after it!
phdr->p_flags= PF_X || PF_R || PF_W; // man elf!


```
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Ok what about a "normal" binary?

:::
-  Here by "normal" we mean dynamically linked 
- there is nothing wrong with using statically linked binaries but usually we will want to do  at least *some* dynamic linking 


```c
#include <stdio.h>
extern void _start(void); // Declare _start as an external symbol

void unused_func() {
  printf("Woah how did I get here?\n");
  return;
}

int main() {
  printf("Hello world!\n");
  return 0;
}

void prepend() {
  printf("woah I ran first. wonder what this means :)\n");
  _start();
}

```

---
<!-- slide template="[[Base Slide]]" -->

::: title

####  `readelf -l` 

:::
- `man readelf`

```

Elf file type is DYN (Position-Independent Executable file)
Entry point 0x680
There are 9 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                 0x00000000000001f8 0x00000000000001f8  R      0x8
  INTERP         0x0000000000000238 0x0000000000000238 0x0000000000000238
                 0x000000000000001b 0x000000000000001b  R      0x1
      [Requesting program interpreter: /lib/ld-linux-aarch64.so.1]
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000970 0x0000000000000970  R E    0x10000
  LOAD           0x0000000000000d90 0x0000000000010d90 0x0000000000010d90
                 0x0000000000000280 0x0000000000000288  RW     0x10000
  DYNAMIC        0x0000000000000da0 0x0000000000010da0 0x0000000000010da0
                 0x00000000000001f0 0x00000000000001f0  RW     0x8
  NOTE           0x0000000000000254 0x0000000000000254 0x0000000000000254
                 0x0000000000000044 0x0000000000000044  R      0x4
  GNU_EH_FRAME   0x0000000000000844 0x0000000000000844 0x0000000000000844
                 0x000000000000004c 0x000000000000004c  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x0000000000000d90 0x0000000000010d90 0x0000000000010d90
                 0x0000000000000270 0x0000000000000270  R      0x1

 Section to Segment mapping:
  Segment Sections...
   00     
   01     .interp 
   02     .interp .note.gnu.build-id .note.ABI-tag .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt .init .plt .text .fini .rodata .eh_frame_hdr .eh_frame 
   03     .init_array .fini_array .dynamic .got .data .bss 
   04     .dynamic 
   05     .note.gnu.build-id .note.ABI-tag 
   06     .eh_frame_hdr 
   07     
   08     .init_array .fini_array .dynamic .got 

```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### wtf is all of this

:::

![[Pasted image 20250205122214.png]]



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamic linking 

:::
- Its pretty complicated and has a lot of moving parts. 
- For that reason, we are going to stick with statically linked binaries and work towards loading a statically linked binary first. 



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Baby's First loader 

:::
-  Rather than trying to implement a fully fledged s loader, we are going to start small 
-  binary is an elf  of type `EXEC`
- binary is statically linked 
- All we need to do is memory map the sections, and then start running at the entry point 
- For now, we will use a  c program with full access to `libc`
- (no dlopen or dlsym though :)



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Steps 

:::
- Open the elf file 
- parse the elf header: take note of the entry point, program header size, offset ..etc
	- optionally validate that this is the file is the correct format 
- Allocate a block of memory that is large enough to fit the memory mapped elf
- map the sections



---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  Memory Mapping Segments

::: 
::: left 
- Walk the program header and take note of all the entries of type `PT_LOAD`
- Keep track of the position in the block of memory used for mapping the elf 
- Copy `physical_size` number of bytes starting at the proper offset to the segment 
- pad with zeros if the virtual size is > physical size 
::: 
::: right

![[mapping_segments.excalidraw.svg]]
:::



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example

:::
```
Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x00000000000002ac 0x00000000000002ac  R E    0x10000
```

- If the file is loaded into a buffer at virtual address $X$  and the newly allocated buffer for the memory mapped elf is at $Y$ 
- we copy $X ,...,X +  0x2ac-1 $ bytes to $Y$ 
- note that the rest of  that segment is padded with zeros. 
- namely, $Y+ 0x2ac,..., $Y + 0x400000 - 1



---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  Title

::: 
::: left 

| Field    | Value                  |
|----------|------------------------|
| Type     | LOAD                   |
| Offset   | 0x0000000000000000      |
| VirtAddr | 0x0000000000400000      |
| PhysAddr | 0x0000000000400000      |
| FileSiz  | 0x00000000000002ac      |
| MemSiz   | 0x00000000000002ac      |
| Flags    | R E                     |
| Align    | 0x10000                 |


::: 
::: right

![[pt_load_align.excalidraw.svg|500px]]
:::




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What about Multiple PT_LOAD?

:::

```
 readelf -l bin/static_write_rw 

Elf file type is EXEC (Executable file)
Entry point 0x400224
There are 5 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x0000000000000304 0x0000000000000304  R E    0x10000
  LOAD           0x0000000000000fd8 0x0000000000410fd8 0x0000000000410fd8
                 0x0000000000000028 0x0000000000000028  RW     0x10000
  NOTE           0x0000000000000158 0x0000000000400158 0x0000000000400158
                 0x0000000000000024 0x0000000000000024  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x0000000000000fd8 0x0000000000410fd8 0x0000000000410fd8
                 0x0000000000000028 0x0000000000000028  R      0x1

```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Relevant PT_LOAD

:::
| Type | Offset              | VirtAddr            | PhysAddr            | FileSiz            | MemSiz             | Flags | Align  |
|------|---------------------|---------------------|---------------------|---------------------|---------------------|-------|--------|
| LOAD | 0x00  | 0x400000  | 0x400000  | 0x304  | 0x304  | R E   | 0x10000 |
| LOAD | 0xfd8  | 0x410fd8  | 0x410fd8  | 0x28  | 0x28  | RW    | 0x10000 |



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Calculating Virtual Ismage Size 

:::

1. **Find the Lowest Virtual Address**:
    
    - Look at all **PT_LOAD** segments and find the **smallest** `p_vaddr` (Virtual Address).
2. **Find the Highest Virtual Address + Size**:
    - For each **PT_LOAD** segment, compute: Segment End=pvaddr+pmemsz Segment End = p_vaddr + p_memszSegment End=pv​addr+pm​emsz
    - Take the **largest** of these segment ends.
3. **Compute Total Virtual Memory Required**:
    
    - **Total Memory = (Highest Segment End - Lowest Virtual Address)**
    - This gives the total amount of memory that must be mapped in the virtual address space.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Step 1

:::

- First Segment (R E) starts at  `0x00400000`
- Second Segment (RW) starts at `0x00410fd8`
- Lowest Virtual Address = `0x00400000`


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Step 2

:::
- First Segment End: `0x00400000 + 0x00000304 = 0x00400304`
- Second Segment End: `0x00410fd8 + 0x00000028 = 0x00411000`
- Since alignment is `0x10000`, the second segment may actually be mapped at `0x00410000` (page-aligned), so the end extends to `0x00411000`

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Step 3

:::
- **Total Virtual Memory Required**: 
- 0x00411000 − 0x00400000   = 0x00011000  
-  Hence we need `0x00011000` 
- **Decimal Equivalent:** `69632` bytes (`68 KB`)

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Expected mapping

:::
- Assume we are mapped at address=0x400000 size=0x11000
-  segment 0: vaddr=0x400000 offset=0x0 filesz=0x304 memsz=0x304 flags=5
- segment 1: vaddr=0x410fd8 offset=0xfd8 filesz=0x28 memsz=0x28 flags=6



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Practical Tips: Page Alignment 

:::
```
/* Page size is typically 4KB (0x1000 bytes) on most systems */

#define PAGE_SIZE 0x1000
#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(addr) ((addr) & PAGE_MASK)
#define PAGE_ALIGN_UP(addr) (((addr) + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_OFFSET(addr) ((addr) & (PAGE_SIZE - 1))

```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Practical Tips Allocating Memory

:::
- `man mmap`
- map anonymous 
- Ideally map as `RW` then fix up permissions later
- To start, it is ok to map as `RWX`
- when setting segment permissions, round the page down. 



