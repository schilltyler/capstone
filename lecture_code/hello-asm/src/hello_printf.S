.include "common.S" // Include the macros file

.section .rodata 
message:
    .asciz "Hello from AArch64 assembly!" // Null-terminated string
fmt:
  .asciz "%s\n"


.section .text
.global _start


_start:
    ldr x0, =fmt 
    ldr x1, =message
    bl printf
    exit 0 
