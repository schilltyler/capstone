

---
 <!-- slide template="[[Title Slide]]" -->
#### AArch64 Linux Process Creation & Dynamic Linking

#### From Kernel Mapping to the Dynamic Loader (ld‑so)


---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Agenda

:::

- Review: ELF structure & static loaders
- How the kernel creates a process on AArch64
    - Parsing the ELF header & program headers
    - Mapping segments into memory (PT_LOAD, alignment, protection)
- The INTERP segment and transferring control to ld‑so
- Dynamic linking: relocations and symbol resolution
- Code walkthrough: Building a basic loader
    - Functions: `map_segments()`, `handle_relocations()`, `handle_dynamic_section()`
    - Using `dlopen`/`dlsym` to patch R_AARCH64_JUMP_SLOT
- Live demo & debugging using `readelf` and `objdump`
- Q&A and further resources


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Linux Kernel Source

:::
- https://github.com/torvalds/linux/blob/09fbf3d502050282bf47ab3babe1d4ed54dd1fd8/fs/binfmt_elf.c#L825-L845
<pre>
<code>
/* Now we do a little grungy work by mmapping the ELF image into
	   the correct location in memory. */
	for(i = 0, elf_ppnt = elf_phdata;
	    i < elf_ex->e_phnum; i++, elf_ppnt++) {
		int elf_prot, elf_flags;
		unsigned long k, vaddr;
		unsigned long total_size = 0;
		unsigned long alignment;

		if (elf_ppnt->p_type != PT_LOAD)
			continue;
....
</code>
</pre>

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Overview: Linux Process Creation

:::

- **Step 1:** The kernel opens the ELF binary, reads the ELF header
- **Step 2:** It inspects the **program header table** to find segments
- **Step 3:** The kernel allocates memory (via the MMU) for all **PT_LOAD** segments
- **Step 4:** For dynamically linked executables, the **INTERP** segment tells the kernel which dynamic linker to load
- **Step 5:** After mapping segments and setting up the initial process state (stack, registers), the kernel transfers control:
    - **Static binaries:** Directly to the entry point
    - **Dynamic binaries:** To the dynamic loader (ld‑so), which finishes relocations and then calls the program’s `_start`
- **Key Point:** Although our user‑space loader example is a simplified version, it mimics the kernel’s basic responsibilities.

---

<!-- slide template="[[Base Slide]]" -->
::: title

#### How the Kernel Loads an ELF Binary

:::

- **ELF Header:**
    - Validates the file format
    - Provides the entry point and the offset to the program headers
- **Program Headers (PT_LOAD, PT_DYNAMIC, PT_INTERP, etc.):**
    - **PT_LOAD:** Describes which file bytes to map into memory
    - **PT_DYNAMIC:** Contains data for the dynamic linker
    - **PT_INTERP:** Specifies the path to the dynamic linker (e.g., `/lib/ld-linux-aarch64.so.1`)
- **Kernel Tasks:**
    - Compute memory size from the lowest and highest virtual addresses
    - Map file segments at proper addresses with correct protection (R, W, X)
    - Set up the process’s virtual memory space and stack

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Memory Mapping of Segments
:::

- **For each PT_LOAD segment:**
    1. Compute the region’s virtual address (p_vaddr) and size (p_memsz)
    2. Align addresses to page boundaries (usually 4 KB)
    3. Map file bytes (p_filesz) into memory and zero‑fill the extra bytes (if p_memsz > p_filesz) 

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: Mapping a PT_LOAD Segment
:::
Consider a PT_LOAD entry:

```
Type    Offset              VirtAddr         FileSiz   MemSiz    Flags
LOAD    0x0000000000000000  0x0000000000400000 0x2ac     0x2ac    R E
```

- **Mapping Steps:**
    - Load file bytes from offset 0x0 → 0x2ac
    - Place them at virtual address 0x400000
    - If needed, pad the segment up to 0x2ac bytes with zeros
- **Alignment:**
    - The kernel rounds down the starting address and up the ending address according to page size

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### The INTERP Segment and Dynamic Linking

:::

- **INTERP Segment Example:**
    
    ```
    [Requesting program interpreter: /lib/ld-linux-aarch64.so.1]
    ```
    
- **Purpose:**
    - For dynamically linked executables (type `DYN` or PIE), the kernel uses this field to know which dynamic linker to load.
- **After mapping segments:**
    - The kernel transfers control to the dynamic linker’s entry point
    - The dynamic linker then:
        - Loads additional shared libraries (DT_NEEDED)
        - Resolves symbols and applies relocations
        - Calls the executable’s `_start`

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Transition of Control

:::

- **Static Executables:**
    - The kernel jumps directly to the entry point from the ELF header.
- **Dynamic Executables:**
    - The kernel sets up the initial state and jumps to ld‑so.
    - ld‑so processes the dynamic section (DT_* tags), applies relocations, then calls the program’s startup routine.
    - Even though the kernel does the initial heavy lifting (mapping, permission setup), the final setup (especially relocations) is handled by the dynamic linker.




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Case Study: hello_printf ELF

::: Let’s examine the hello_printf binary.

- **Relocation Sections (from `readelf -r hello_printf`):**
    
    ```
    Relocation section '.rela.dyn' at offset 0x4a0 contains 8 entries:
      Offset          Info           Type           Sym. Value    Sym. Name + Addend
    000000010d88  000000000403 R_AARCH64_RELATIV                    7d0
    000000010d90  000000000403 R_AARCH64_RELATIV                    780
    000000010ff0  000000000403 R_AARCH64_RELATIV                    800
    000000011008  000000000403 R_AARCH64_RELATIV                    11008
    000000010fd8  000500000401 R_AARCH64_GLOB_DA 0000000000000000 _ITM_deregisterTM[...] + 0
    000000010fe0  000600000401 R_AARCH64_GLOB_DA 0000000000000000 __cxa_finalize@GLIBC_2.17 + 0
    000000010fe8  000700000401 R_AARCH64_GLOB_DA 0000000000000000 __gmon_start__ + 0
    000000010ff8  000a00000401 R_AARCH64_GLOB_DA 0000000000000000 _ITM_registerTMCl[...] + 0
    
    Relocation section '.rela.plt' at offset 0x560 contains 6 entries:
      Offset          Info           Type           Sym. Value    Sym. Name + Addend
    000000010fa0  000300000402 R_AARCH64_JUMP_SL 0000000000000000 exit@GLIBC_2.17 + 0
    000000010fa8  000400000402 R_AARCH64_JUMP_SL 0000000000000000 __libc_start_main@GLIBC_2.34 + 0
    000000010fb0  000600000402 R_AARCH64_JUMP_SL 0000000000000000 __cxa_finalize@GLIBC_2.17 + 0
    000000010fb8  000700000402 R_AARCH64_JUMP_SL 0000000000000000 __gmon_start__ + 0
    000000010fc0  000800000402 R_AARCH64_JUMP_SL 0000000000000000 abort@GLIBC_2.17 + 0
    000000010fc8  000900000402 R_AARCH64_JUMP_SL 0000000000000000 puts@GLIBC_2.17 + 0
    ```
    
- **Key Relocation Types:**
    - **R_AARCH64_RELATIV:** Adjusts pointers relative to the load address
    - **R_AARCH64_GLOB_DA:** Used for non‐PLT global data symbols
    - **R_AARCH64_JUMP_SL:** Used in the PLT for function calls (patched later by the dynamic linker)

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dissecting hello_printf: `readelf -l`

::: Example output:

```
Elf file type is DYN (Position-Independent Executable file)
Entry point 0x6c0
...
[Requesting program interpreter: /lib/ld-linux-aarch64.so.1]
...
```

- **Observation:**
    - The executable is a PIE (ET_DYN) and requires ld‑so for relocation resolution.
- **Role of ld‑so:**
    - Upon being called, it reads the dynamic section, applies relocations (including those in `.rela.plt`), and finally calls the actual program entry point.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Kernel vs. Dynamic Loader Responsibilities

:::

- **Kernel’s Role:**
    - Open the binary and read the ELF header
    - Map all PT_LOAD segments into memory (with proper permissions)
    - Set up the process’s virtual memory (stack, registers, etc.)
    - Identify the interpreter (if any) from the PT_INTERP segment
- **Dynamic Loader (ld‑so)’s Role:**
    - Parse the dynamic section (DT_* entries)
    - Load dependent shared libraries
    - Apply relocations:
        - For **R_AARCH64_RELATIV:** Add the base address
        - For **R_AARCH64_JUMP_SL:** Resolve via `dlsym` and patch the PLT/GOT
    - Finally, call the program’s startup routine

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Building a Basic User‑Space Loader

:::

- **Objective:**
    - Emulate a “baby” loader that loads a statically linked ELF first and then extend the ideas for dynamic executables.
- **We break functionality into three main functions:**
    1. **map_segments()** – Allocate memory and copy in each PT_LOAD segment.
    2. **handle_relocations()** – Walk through relocation sections (e.g., `.rela.dyn`, `.rela.plt`) and adjust addresses.
    3. **handle_dynamic_section()** – Process DT_* tags and, if needed, use `dlopen`/`dlsym` to patch function pointers for R_AARCH64_JUMP_SL.
		1. for now we assume that all the required libraries are loaded in memory (libc)
- **After these steps:**
    - For a dynamic binary, the loader should jump to the entry point of ld‑so (or call its equivalent function).

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Function 1: map_segments()

::: 

We did this last time :)

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Function 2: handle_relocations()

:::

- Adjust pointers in the loaded image based on relocation entries.
-  Do:
    - Locate the relocation sections (e.g. `.rela.dyn` and `.rela.plt`) via the dynamic section.
    - For each relocation of type **R_AARCH64_RELATIV**:
        - Compute: `*(target) = base_address + addend`
    - For **R_AARCH64_GLOB_DATA** and **R_AARCH64_JUMP_SLOT**:
        - These typically require symbol lookup (which we can delegate to the dynamic loader).



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamic Segment 

:::
-  Parse the program header and find the rva of the  dynamic segment
- from the dynamic segment, find the rva of the symbol table,  and the symbol string table and the relocation table
- the symbol table is a table of symbols
- the symbol string table 
- handle each relocation type as needed 


---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Dyn: Pass 1
:::

<small>
- make a first pass over the dynamic segment

- **DT_RELA:** Represents relocation entries with explicit addends. The code retrieves the address of the relocation table (`rela`) by adjusting the pointer in the dynamic entry (`dyn->d_un.d_ptr`) with the base address of the loaded ELF image (`elf_img->mem`) and offset by the minimum virtual address (`elf_img->min_vaddr`).
    
- **DT_RELASZ:** Specifies the total size in bytes of the associated relocation entries (DT_RELA). The code stores this size in `rela_size`.
    
- **DT_JMPREL:** Represents relocation entries for procedure linkage table (PLT) entries. The code retrieves the address of the jump relocation table (`jmprel`) similar to DT_RELA.
    
- **DT_PLTRELSZ:** Specifies the total size in bytes of the PLT relocation entries (DT_JMPREL). The code stores this size in `jmprel_size`.
    
- **DT_SYMTAB:** Represents the address of the dynamic symbol table. The code retrieves the address of the symbol table (`dynsym`) similar to DT_RELA and DT_JMPREL.
    
- **DT_STRTAB:** Represents the address of the dynamic string table, used to resolve symbol names. The code retrieves the address of the string table (`dynstr`) similar to DT_RELA, DT_JMPREL, and DT_SYMTAB.
    
- **DT_NULL:** Marks the end of the dynamic section. The loop continues until a DT_NULL entry is encountered.
</small>

---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Pass 1 Starter

:::

<pre>
<code>
  for (Elf64_Dyn *dyn = dynamic; dyn->d_tag != DT_NULL; dyn++) {
    switch (dyn->d_tag) {
    case DT_RELA:
      break;
    case DT_RELASZ:
      break;
    case DT_JMPREL:
      break;
    case DT_PLTRELSZ:
      break;
    case DT_SYMTAB:
      break;
    case DT_STRTAB:
      break;
    default:
      break;
    }
  }


</pre>
</code>

---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Walking Dynamic Segment: Pass 2
:::

- At this point we  have
	- symbol able information
	- relocation information



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Pseudeo Code 

:::

<pre>
<code>
void handle_relocations(void *base_addr, Elf64_Dyn *dynamic) {
    // Assume you have parsed the dynamic section and located the relocation table
    Elf64_Rela *rela = /* pointer to relocation table */;
    size_t rela_count = /* number of entries */;
    
    for (size_t i = 0; i < rela_count; i++) {
        Elf64_Rela *r = &rela[i];
        DO_RELOCATION(r);
    }
}
</pre>
</code>



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What relocation do we need to handle?

:::


<pre>
<code>
readelf -r bin/hello       

Relocation section '.rela.dyn' at offset 0x480 contains 8 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000010d90  000000000403 R_AARCH64_RELATIV                    750
000000010d98  000000000403 R_AARCH64_RELATIV                    700
000000010ff0  000000000403 R_AARCH64_RELATIV                    754
000000011008  000000000403 R_AARCH64_RELATIV                    11008
000000010fd8  000400000401 R_AARCH64_GLOB_DA 0000000000000000 _ITM_deregisterTM[...] + 0
000000010fe0  000500000401 R_AARCH64_GLOB_DA 0000000000000000 __cxa_finalize@GLIBC_2.17 + 0
000000010fe8  000600000401 R_AARCH64_GLOB_DA 0000000000000000 __gmon_start__ + 0
000000010ff8  000900000401 R_AARCH64_GLOB_DA 0000000000000000 _ITM_registerTMCl[...] + 0

Relocation section '.rela.plt' at offset 0x540 contains 5 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000010fa8  000300000402 R_AARCH64_JUMP_SL 0000000000000000 __libc_start_main@GLIBC_2.34 + 0
000000010fb0  000500000402 R_AARCH64_JUMP_SL 0000000000000000 __cxa_finalize@GLIBC_2.17 + 0
000000010fb8  000600000402 R_AARCH64_JUMP_SL 0000000000000000 __gmon_start__ + 0
000000010fc0  000700000402 R_AARCH64_JUMP_SL 0000000000000000 abort@GLIBC_2.17 + 0
000000010fc8  000800000402 R_AARCH64_JUMP_SL 0000000000000000 puts@GLIBC_2.17 + 0
</pre>
</code>


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What relocation do we need to handle?

:::
-  R_AARCH64_RELATIVE
-  R_AARCH64_GLOB_DATA
-  R_AARCH64_JUMP_SLOT
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### How do we handle each relocation type?

:::
- https://github.com/openbsd/src/blob/2347e6edcd5e06bc97a8ea95077c54fc76d2e634/libexec/ld.so/aarch64/archdep.h#L41
- search for constants in source code/macros names 
for example

<code>
<pre>
static inline void
RELOC_DYN(Elf_RelA *r, const Elf_Sym *s, Elf_Addr *p, unsigned long v)
{
	if (ELF_R_TYPE(r->r_info) == R_AARCH64_RELATIVE) {
		*p = v + r->r_addend;
	} else if (ELF_R_TYPE(r->r_info) == R_AARCH64_GLOB_DAT) {
		*p = v + s->st_value + r->r_addend;
	} else if (ELF_R_TYPE(r->r_info) == R_AARCH64_ABS64) {
		*p = v + s->st_value + r->r_addend;
	} else {
		_csu_abort();
	}
}

</code>
</pre>

---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Handling Dynamic Section: PLT

:::

-  Process the DT_* tags for dynamic linking.
- Do: 
    - Find entries like **DT_PLTGOT**, **DT_JMPREL**, **DT_PLTRELSZ** (for the PLT relocations).
    - Use `dlopen`/`dlsym` to resolve external symbols for **R_AARCH64_JUMP_SLOT**.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Title

:::
-  Patching  va for required symbols


```c
Elf64_Sym sym = dynsym[sym_index];
const char *sym_name = dynstr + sym.st_name;
debug_printf("RELA[%zu]: R_AARCH64_JUMP_SLOT for symbol '%s'\n", i,
			 sym_name);
void *sym_addr = dlsym(RTLD_DEFAULT, sym_name);


```

_Note:_ In a complete loader, you’d parse the dynamic table to find each needed DT_* tag.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Bringing It All Together

:::

**Main Loader Pseudo‑code:**

```c
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf-binary>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // 1. Map the segments of the ELF into memory.
    void *base_addr = map_segments(argv[1]);
    
    // 2. Parse the dynamic section (if any) and handle relocations.
    // (For simplicity, assume dynamic section pointer is found in the mapped image)
   // Elf64_Dyn *dynamic = /* locate dynamic section in base_addr */;
    handle_dynamic_section(base_addr, dynamic);
    
    // 3. Transfer control.
    // For statically linked ELF: jump to entry point from ELF header.
    // For dynamic ELF: normally the kernel would call ld‑so's entry point.
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)base_addr;
    void (*entry_point)(void) = (void (*)(void))((char *)base_addr + ehdr->e_entry);
    entry_point();
    
    return 0;
}
```

