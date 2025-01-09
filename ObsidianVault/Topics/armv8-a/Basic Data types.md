
# Basic Data Types in ARMv8-A

Welcome back to our exploration of the ARMv8-A (AArch64) architecture! In this quick guide, we’ll cover the fundamental data types you’ll encounter and how they’re typically named in the ARM world. Understanding these sizes is crucial when using load/store instructions, managing memory, or working with registers in assembly.

---
bit 
0/1

nibble (4 bits)
## 1. Byte (8 bits)

- **Size**: 8 bits  
- **Common Use**: Characters, small integers, or raw data.  
- **Relevant Instructions**: `LDRB` (load byte), `STRB` (store byte).

A byte is your smallest addressable unit of memory. In AArch64, a “byte” is 8 bits. 

---

## 2. Halfword (16 bits)

- **Size**: 16 bits (2 bytes)  
- **Common Use**: Short integers (e.g., `uint16_t`, `int16_t` in C).  
- **Relevant Instructions**: `LDRH` (load halfword), `STRH` (store halfword).

The term “halfword” is derived from the fact that it’s half the size of a 32-bit word in many architectures. 

---

## 3. Word (32 bits)

- **Size**: 32 bits (4 bytes)  
- **Common Use**: Typical integer type (e.g., `uint32_t`, `int32_t`).  
- **Relevant Instructions**: `LDR`/`STR` (when you’re dealing with 32-bit registers, you might see `W0–W30`).

Historically, a “word” can vary across architectures, but in AArch64 documentation, **word** often refers to 32 bits.

---

## 4. Doubleword (64 bits)

- **Size**: 64 bits (8 bytes)  
- **Common Use**: Long integers (`uint64_t`, `int64_t`), double-precision floats.  
- **Relevant Instructions**: `LDR`/`STR` to/from 64-bit registers (`X0–X30`).

With AArch64, your general-purpose registers (`X0–X30`) are 64 bits wide, which makes the 64-bit “doubleword” a natural size for many operations.

---

## 5. Quadword (128 bits)

- **Size**: 128 bits (16 bytes)  
- **Common Use**: NEON/SIMD operations, certain cryptographic instructions.  
- **Relevant Instructions**: Often referenced in vector instructions (`V0–V31`).

A “quadword” is 128 bits, used in SIMD (Single Instruction, Multiple Data) contexts or specialized extensions like NEON for parallel operations.

---

## Visual Overview

Below is a Mermaid diagram that gives you a quick hierarchical look at these data types, from smallest to largest:

```mermaid
flowchart LR
    A[Byte <br> 8 bits] --> B[Halfword <br> 16 bits]
    B --> C[Word <br> 32 bits]
    C --> D[Doubleword <br> 64 bits]
    D --> E[Quadword <br> 128 bits]
````

---

## Practical Pointers

- In assembly, **knowing the correct data size** is critical for choosing the right load/store instruction (e.g., `ldrb` vs. `ldrh` vs. `ldr`).
- AArch64 register names **wX** vs. **xX** can help you remember if you’re dealing with 32-bit (word) or 64-bit (doubleword) portions of a register.
- When working with SIMD or cryptographic operations, **V** registers often hold data of 128-bit (quadword) size.

Understanding these fundamental data types will help you avoid pitfalls when writing assembly or optimizing code on ARMv8-A. Keep these sizes in mind whenever you’re moving data around in memory or registers.
