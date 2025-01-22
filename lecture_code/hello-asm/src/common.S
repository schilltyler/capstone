// common.S
// Define syscall numbers
.equ sys_exit, 93 // Syscall number for exit

// Macro to perform the exit syscall
.macro exit code
    mov x8, sys_exit     // Load syscall number
    mov x0, \code        // Load the exit code into x0
    svc #0               // Make the syscall
.endm
