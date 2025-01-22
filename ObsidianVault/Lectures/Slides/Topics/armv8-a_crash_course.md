






---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What is ARM?

:::
- stands for **Advanced RISC Machine(s)**.
- RISC stands for  reduced instruction set computer
- ARM is a company, an ambiguous term for Assembly Languages and architectures 

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### ARM Holdings

:::
  - A British semiconductor and software company.
  - Specializes in designing RISC  architectures.
  - Does not manufacture its own chips.
  - Licenses its designs to other companies (e.g., Qualcomm, Apple, Samsung).
  - ARM chips power billions of devices, from smartphones to IoT devices to ssd controllers to Enterprise servers.
  - Majority owned by Soft Bank (questionable call by the UK)




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### RISC vs CISC

:::

| Feature              | RISC                         | CISC                        |
| -------------------- | ---------------------------- | --------------------------- |
| **Instruction Set**  | Simple, small                | Complex, large              |
| **Execution**        | Single cycle per instruction | Multi-cycle per instruction |
| **Power Efficiency** | High                         | Lower                       |
| **Performance**      | Optimized for pipelining     | Slower due to complexity    |
| **Program Size**     | Larger                       | Smaller                     |
| **Examples**         | ARM, RISC-V                  | x86, Intel 8086             |



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### ArmV8 profiles

:::

1. **A (Application)**:
   - Supports rich operating systems.
   - Focused on performance and complex applications.
   - Examples: smartphones, tablets, servers.
2. **R (Real-Time)**:
   - Predictable and deterministic performance.
   - Common in automotive and industrial systems.
3. **M (Microcontroller)**:
   - Low power, low cost.
   - Designed for IoT and embedded systems.
   - probably in your SSD controller




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Microarchitectures

:::

  - Specific implementations of the ARM architecture.
  - Examples: Cortex-A series, Cortex-R series, Cortex-M series.
- Each microarchitecture optimizes for specific use cases:
  - Performance, power efficiency, or a balance of both.



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### ARMv8-a 

:::
-  This class focuses on ARMv8-a 
- RISC arch   built by ARM holdings 
-  `aarch64` -> A64: 64 bit execution state 
- `aarch32` -> A32: 32 bit execution state 

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### A64  (AArch64 mode) Assembly Crash Course

:::

- `aarch64`: 64 bit execution state  for ARMV8-a
- `A64` Assembly language/instruction set for `aarch64`
-  32-bit assembly  instructions
- Uses 31  64 bit general-purpose registers: `x0`–`x30`, plus `sp` (stack pointer) and `pc` (program counter).
	- `w0-w30` are the lower 32 bits $b_0,...,b_{31}$
- Uses 31  128 bit floating point registers `q0-q31`
	- `b,h,w,d,q`:  -> 
	- `byte`, `hald-word`, `word`, `double-word`,`quad-word`
- Some registers have special usage/convention:
- The goal here is to learn **just enough** to be useful
- technically supports little/big endian but I have only ever seen little endian
	
 




---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Special General Purpose registers (A64) 

:::
- Here "special" means there exists a convention
- `x0` often used for function return values form a subroutine.
- `x0`–`x7` typically used for arguments in many Linux ABIs.
- `x8` holds syscall number   
-  `x29`: (sometimes aliased as `fp`) Frame Pointer (optional)
-  `x30`: Link Register often holds return address on subroutine calls




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Comparison to x86_64

:::

<small>

| **Aarch64 Register** | **Purpose/Usage**                                         | **x86_64 Equivalent**                  | **Explanation**                                                                          |
| -------------------- | --------------------------------------------------------- | -------------------------------------- | ---------------------------------------------------------------------------------------- |
| `x0`                 | Return values from a subroutine                           | `rax`                                  | Both are used to store return values for functions.                                      |
| `x0`–`x7`            | Arguments in many Linux ABIs                              | `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9` | x86_64 uses a similar approach with a set of registers for function arguments.           |
| `x8`                 | Syscall number                                            | `rax`                                  | On x86_64, the syscall number is passed through `rax` before invoking syscall.           |
| `x29` (aliased `fp`) | Frame pointer (optional; helps manage stack frames)       | `rbp`                                  | Both are used as a frame pointer to access local variables and manage the stack.         |
| `x30`                | Link register (holds return address for subroutine calls) | `ret` stack mechanism                  | In x86_64, the return address is stored on the stack instead of in a dedicated register. |

</small>

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### A64 vs x86_64 notes 

:::
- Aarch64 cannot directly operate on data unless it is stored in a register. 
- Fixed-length 32-bit instructions simplify decoding and pipelining.
- Implements a **load/store architecture**, meaning it cannot perform operations directly on memory. 
- All data must first be loaded into registers for processing and then stored back into memory. 
	- For example, in `memcpy`, data is copied in chunks by loading blocks into registers and then storing them to the destination address.



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Other important registers

:::
- `xzr`: zero register. ignores writes. 
- `sp`: stack pointer
- `pc`: instruction pointer/program counter. Can't directly read/modify  like $x_i$
- `lr`: alias for `x30`. 
	- There is no `call` instruction.
	- There is no `ret` instruction 
- various system registers ( exception levels, mmu ...etc)
- this is more trivia than anything but "`x31`" is  either zero register or stack pointer



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Refresher: Common Registers (AArch64 )

:::

- **x0–x7**: 
	- used to pass function arguments, return values. <!-- element class="fragment" data-fragment-index="1" -->
- **x8**: 
	- indirect syscall param or special usage. <!-- element class="fragment" data-fragment-index="2" -->
- **x9–x15**: temporary registers.
- **x16–x17**: intra-procedure call scratch regs.
- **x19–x28**: callee-saved registers.
- **x29**: 
	- optional frame pointer (often used with fp alias) <!-- element class="fragment" data-fragment-index="3" -->
- **x30**: 
	- link register (return address). <!-- element class="fragment" data-fragment-index="4" -->
- **sp**: stack pointer.
- **pc**: program counter (auto-updated cant be directly used).

---
<!-- slide template="[[Split Vertical]]" -->

::: title

####  Operand Sizes/ Types

:::

::: left

- Byte: 8 bits
- Halfword: 16 bits
- Word: 32 bits
- Doubleword: 64 bits
- Quadword: 128 bits
- ARMv8-a is (almost always)  **little-endian**  \*.
- In assembly, you’ll see mnemonics for loading/storing different widths (e.g. `ldrb`, `ldrh`, `ldr`, `ldur`). :::

::: right
![[0x05_slide_32_image.png|300]] :::


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Sanity Check

:::
What does this print? #exercise  
```c
#include <stdio.h>

int main() {
  int x = 0xdeadbeef;
  unsigned char *y = (unsigned char *)&x;
  for (int i = 0; i < 4; i++) {
    unsigned char c = y[i] & 0xff;
    printf("%x ", c);
  }
  printf("\n");
}

```
- ef be ad de <!-- element class="fragment" data-fragment-index="1" -->

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Basic Arithmetic (AArch64)

:::

- **ADD** / **SUB**: Integer addition/subtraction
    - Example: `ADD x0, x1, x2` (x0 ← x1 + x2)
    - Immediate form: `ADD w3, w4, #10`
- **ADDS** / **SUBS**: Same as above, but **sets condition flags** (N, Z, C, V)
    - Useful for branching on results
- **MUL**: Multiply the lower 64 bits
    - `MUL x0, x1, x2`
- **SMULH** / **UMULH**: Signed / unsigned high 64-bit multiply
    - If the product exceeds 64 bits, high part stored in the destination

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Load/Store Operations

:::

- **LDR** / **STR**: Primary load/store instructions
    - `LDR x0, [x1]` → x0 ← _(uint64_t_)x1
    - `STR x2, [x3, #16]` → _(uint64_t_)(x3+16) ← x2
- **LDRB** / **STRB**: For 8-bit (byte) load/store
    - Similarly `LDRH`, `LDRW` for halfword/word
- **LDP** / **STP**: Load/Store **pairs** of registers
    - Often used to save/restore register pairs in function prolog/epilog
- Offsets can be 
	- immediate (`[x1, #4]`)
	- post-/pre-indexed (`[x1], #4`, `[x1, #4]!`)
- READ: https://developer.arm.com/documentation/102374/0102/Loads-and-stores---addressing



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Immediate  Offset

:::

- The address is computed by adding a constant offset directly to the base register.
- the base register is not modified.

Syntax:
```
ldr w0, [x1, #4]  // Load from address (x1 + 4)
str w0, [x1, #-8] // Store to address (x1 - 8)
```
- address = `x1 + 4 (or x1 - 8)`.
- Perform the memory access.
- Base register x1 remains unchanged.



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Post-indexed Offset 

:::
- The address is computed using the base register.
- After the access, the offset is added to the base register.

Syntax:
```
ldr w0, [x1], #4   // Load from x1, then x1 += 4
str w0, [x1], #-8  // Store to x1, then x1 -= 8
```
- Use the address in the base register for memory access.
- Update the base register `(x1 += 4 or x1 -= 8)`.



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Pre-Index Offset

:::
- The offset is added to the base register before the memory access.
- The base register is updated with the new address.

Syntax:
```
ldr w0, [x1, #4]!  // x1 += 4, then load from x1
str w0, [x1, #-8]! // x1 -= 8, then store to x1
```
-  Update the base register (x1 += 4 or x1 -= 8).    
- Use the updated value of the base register for memory access.



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Conditional Flags

:::
-   Condition Codes: Special flags set by the processor to indicate the result of an operation.
-  Enables conditional execution of instructions based on previous operations.
- N (Negative): Set if the result of an operation is negative.
- Z (Zero): Set if the result is zero.
- C (Carry): Set if an operation results in a carry out or borrow.
- V (Overflow): Set if an operation causes a signed overflow.



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Insturctions-> Conditional Flags

:::
- Comparison Instructions:
	- CMP: Compares two values by subtracting one from the other; updates N, Z, C, V flags.
	- CMN: Compares two values by adding them; updates N, Z, C, V flags.
- Logical Instructions:
	- TST: Performs an AND operation; updates N and Z flags.
	- TEQ: Performs an XOR operation; updates N and Z flags.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Basic Control FLow

:::

- **B** : Unconditional branch
- **BL** : Branch and link (subroutine call)
    - Automatically saves the return address in **x30** (Link Register)
- **RET**: Return from subroutine
	- Not a real instruction :)
	- `blr x30`
    - Jumps to the address in x30
- **CBZ** / **CBNZ**: Compare register to zero and branch
    - `CBZ x0, label` → if x0 == 0, branch
- Condition-based branches (after `ADDS`, `SUBS`):
    - **B.EQ**, **B.NE**, **B.GT**, **B.LT**, etc.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### More Resources

:::
- https://developer.arm.com/documentation/102374/0102
- https://web.archive.org/web/20240829145252/https://modexp.wordpress.com/2018/10/30/arm64-assembly/
- https://mariokartwii.com/armv8/
