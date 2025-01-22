// Macros 
.equ STDOUT_FD, 1
.equ SYS_WRITE, 64
.equ  SYS_EXIT, 93


.section .rodata 
hello:
  .asciz "hello world!\n"

.section .text 
.global _start
_start:
  // 1 is stdout 
  mov x0, STDOUT_FD
  ldr x1, =hello 
  mov x2, #14
  mov x8, SYS_WRITE
  svc 0
  mov x0, 0
  mov x8, SYS_EXIT
  svc 0
