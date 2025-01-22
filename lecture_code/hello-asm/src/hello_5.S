
.equ  SYS_EXIT, 93
.section .rodata
fmt: .asciz "Result %llu!\n"

.section .text
// https://godbolt.org/z/Gxjor9Kqj
// Subroutine: factorial
// Sets up the recursion and calls the helper.
// Arguments:
// - x0: Input number
// Returns:
// - x0: Factorial of the input number
factorial:
    // Check if the input number is 0 or 1
    cbz x0, base_case
    mov x8, x0 
    mov x0, #1 
    b recursive_case 
recursive_case:
  subs x9, x8, #1 
  mul x0, x8, x0 
  mov x8, x9 
  b.ne recursive_case
  ret
base_case:
  mov x0, 1 
  ret 

// Entry point
.global _start
_start:
    // Initialize the number to calculate factorial
    mov x0, 5              // Example: Calculate factorial of 5

    // Call factorial_helper
    bl factorial  // x0 will contain the result
    mov x1, x0 
    ldr x0, =fmt
    bl printf 

    // Exit program
    mov x8, 93             // Syscall number for exit
    mov x0, 0              // Exit code
    svc 0
