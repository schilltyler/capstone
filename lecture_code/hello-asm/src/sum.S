/* 
long sum_to_n(long n)
 {
   long total = 0;
   for (long i = 1; i <= n; i++)
       total += i;
   return total;
 }
*/
sum_to_n:
    stp x29, x30, [sp, #-16]!
    mov  x29, sp

    mov x1, #1        // i = 1
    mov x2, #0        // total = 0

loop:
    cmp x1, x0        //  compare i to n
    b.gt end          // if i > n => go end
    add x2, x2, x1    // total += i
    add x1, x1, #1    // i++
    b loop

end:
    mov x0, x2
    mov sp, x29
    ldp x29, x30, [sp], #16
    ret
