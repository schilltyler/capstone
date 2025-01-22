
# Lab: Implementing C Functions, Extracting Assembly, and Creating Minimal Binaries for `aarch64-linux`

In this lab, you will implement several basic functions in C, analyze their assembly code, and create minimal binaries using `as` (assembler) and `ld` (linker) for the `aarch64-linux` platform. Additionally, you will use the `printf` function for formatted output and define a macro for system exit (`sys_exit`).

---

## Objectives
1. Implement several basic functions in C.
2. Extract the assembly code using [godbolt.org](https://godbolt.org/).
3. Assemble and link the assembly code into minimal binaries.
4. Explore the behavior of your functions on the `aarch64-linux` platform.

---

## Tools Needed
- [godbolt.org](https://godbolt.org/)
- A Linux system with:
  - `gcc` for ARM (or cross-compiler like `aarch64-linux-gnu-gcc`)
  - `as` (GNU assembler)
  - `ld` (GNU linker)

---

## Step 0:

Getting started: identifying Syscal Numbers.  Use the following program to find  the relevent syscall numbers. Note that  on the Pi`/usr/include/linux/unistd.h ` is where `<linux/unistd.h>` is implemented

```c
#include <stdio.h>
#include <linux/unistd.h> // Include syscall numbers
//#include <sys/syscall.h>  // Include syscall function declaration

int main() {
    printf("SYS_read: %d\n", __NR_read);
    return 0;
}
```
write a bash/python script that takes a syscall name such as read and prints out the syscall number

## Step 1: Function Implementation
### Functions to Implement
1. **`sum(a, b)`**: Computes the sum of two integers.
2. **`memcpy`**: Copies memory from one location to another.
3. **`strcpy`**: Copies a string from source to destination.
4. **`fibonacci(n)`**: Computes the nth Fibonacci number using recursion.
5. **`factorial(n)`**: Computes the factorial of a given number.

### Challenge Problem
Implement a function **`reverse_words(char *str)`** that reverses all the words in a given string **in place**.

---

## Step 2: Writing the Functions in C
Use the following skeleton code to define your functions:

```c
#include <stdio.h>

#define sys_exit(code) __asm__("mov x8, 93; mov x0, %0; svc 0;" :: "r"(code))

int sum(int a, int b) {
    return a + b;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int factorial(int n) {
    if (n == 0) return 1;
    return n * factorial(n - 1);
}

// Challenge Problem: Reverse words in a string
void reverse_words(char *str) {
    // Implement this as a challenge
}

int main() {
    printf("Sum: %d\n", sum(3, 5));
    return 0;
}
````

---

## Step 3: Extracting Assembly from godbolt.org

1. Navigate to [godbolt.org](https://godbolt.org/).
2. Paste the C code into the editor.
3. Select `aarch64` as the target architecture and enable the following compiler options:
    - `-O3` for optimized output.
    - `-S` to generate assembly code.
4. Copy the generated assembly code for each function.

---

## Step 4: Compiling Minimal Binaries

1. Save the extracted assembly code to a file, e.g., `sum.s`.
2. Use the GNU assembler (`as`) to assemble the code:
    
    ```bash
    as -o sum.o sum.s
    ```
    
3. Link the object file into a binary using the GNU linker (`ld`):
    
    ```bash
    ld -o sum sum.o -lc --dynamic-linker=/lib/ld-linux-aarch64.so.1
    ```
    
4. Run the binary on an `aarch64-linux` system.

---

## Step 5: Verifying the Output

- Test the binaries to ensure correctness.
- Use `strace/gdb` to analyze system calls if the program does not behave as expected:
    
    ```bash
    strace ./sum
    ```
    

---

## Additional Exercises

1. Modify `fibonacci(n)` to use iteration instead of recursion.
2. Optimize `memcpy` using `uint64_t` pointers for larger chunks.
3. Implement `reverse_words(char *str)` as described in the challenge problem.

---

## Submission

- hold onto your code  for next week :) x2
---

## Notes

- Make sure to include comments in your code and assembly for clarity.
- Be aware of alignment and endianness when working with memory in `aarch64`.
