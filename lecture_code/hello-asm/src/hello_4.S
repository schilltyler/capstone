.equ  SYS_EXIT, 93
.section .rodata
fmt: .asciz "Hello %s!\n"
fmt_alt: .asciz "fmt addr: 0x%p\n"
val: .asciz "world"

.section .text 
.global _start 

_start:
  ldr x0, =fmt
  ldr x1, =val 
  bl printf 
  ldr x0, =fmt_alt
  ldr x1, =fmt 
  bl printf 
  mov x8, SYS_EXIT
  svc 0 


