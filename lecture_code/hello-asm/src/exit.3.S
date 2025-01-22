// Define sys_exit to be 93
.equ sys_exit, 93


// Macro to perform the exit syscall
// note that code is an arg
.macro exit code
    mov x8, sys_exit     // Load syscall number (94 for exit)
    mov x0, \code        // Load the exit code into x0
    svc #0               // Make the syscall
.endm

.section .text
.global _start


_start:
    exit 0               // Call the macro with exit code 0

