.section .rodata 
hello:
  .asciz "hello world!\n"

.section .text 
.global _start
_start:
  // 1 is stdout 
  mov x0, #1
  ldr x1, =hello 
  mov x2, #14
  mov x8, #64
  svc 0
  mov x0, 0
  mov x8, #93
  svc 0


  
