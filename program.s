.text
.globl program
program:
   ld t0, 8(a0)
   sd t0, 16(a0)
   li t0, 1
   sd t0, 24(a0)
L_while_cond_0:
   ld t0, 16(a0)
   li t1, 1
   blt t1, t0, L_while_body_1
   j L_while_end_2
L_while_body_1:
   ld t0, 24(a0)
   ld t1, 16(a0)
   mul t0, t0, t1
   sd t0, 24(a0)
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_while_cond_0
L_while_end_2:
   li t0, 0
   sd t0, 16(a0)
   ld t0, 24(a0)
   sd t0, 0(a0)
   ret
