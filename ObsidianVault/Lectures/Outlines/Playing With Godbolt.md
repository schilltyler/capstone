
```c
#include <stddef.h>

void my_mcmcpy(void *dest, void *src, size_t n) 
{ 
// Typecast src and dest addresses to (char *) 
char *csrc = (char *)src; 
char *cdest = (char *)dest; 

// Copy contents of src[] to dest[] 
for (int i=0; i<n; i++) 
    cdest[i] = csrc[i]; 
} 
// zig cc -target aarch64-linux-gnu -fomit-frame-pointer -Os
```

```asm
my_mcmcpy(void*, void*, unsigned long): // @my_mcmcpy(void*, void*, unsigned long)
        cbz     x2, .LBB0_2
.LBB0_1:                                // =>This Inner Loop Header: Depth=1
        ldrb    w8, [x1], #1
        subs    x2, x2, #1
        strb    w8, [x0], #1
        b.ne    .LBB0_1
.LBB0_2:
        ret
```

https://godbolt.org/z/9ob8vTYzG
https://github.com/ARM-software/optimized-routines/blob/master/string/aarch64/memcpy.S
