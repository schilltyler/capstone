// Macros 
.equ STDOUT_FD, 1
.equ SYS_WRITE, 64
.equ  SYS_EXIT, 93

.macro write_stdout message,length
  mov x0, STDOUT_FD
  ldr x1, =\message 
  mov x2, \length 
  mov x8, SYS_WRITE
  svc 0
.endm

.macro exit_with code 
  mov x0, \code 
  mov x8, SYS_EXIT
  svc 0
.endm 

.section .rodata 
hello:
  .asciz "hello world!\n"

.section .text 
.global _start
_start:
  write_stdout hello, 14
  exit_with 0
