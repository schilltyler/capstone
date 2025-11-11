.global _start
.text

_start:
    // x0 = argc
    ldr x0, [sp]

    // x1 = argv
    add x1, sp, #8

    // x2 = envp = argv + (argc + 1)
    ldr x3, [sp]
    add x3, x3, #1
    lsl x3, x3, #3
    add x2, x1, x3

    bl main

    mov x8, #93 // SYS_exit
    svc #0
