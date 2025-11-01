.text
.globl add
add:
   li t0, 1
   sd t0, 8(a0)
   li t0, 2
   sd t0, 16(a0)
   ld t0, 8(a0)
   ld t1, 16(a0)
   add t0, t0, t1
   sd t0, 24(a0)
   ld t0, 24(a0)
   sd t0, 0(a0)
   ret
