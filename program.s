.text
.globl program
program:
   li t0, 0
   sd t0, 0(a0)
   li t0, 1
   sd t0, 8(a0)
   li t0, 0
   sd t0, 40(a0)
L_while_cond_0:
   ld t0, 16(a0)
   ld t1, 40(a0)
   beq t0, t1, L_while_end_2
   j L_while_body_1
L_while_body_1:
   ld t0, 0(a0)
   ld t1, 8(a0)
   add t0, t0, t1
   sd t0, 32(a0)
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 32(a0)
   sd t0, 8(a0)
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
   li t0, 0
   sd t0, 40(a0)
   j L_while_cond_0
L_while_end_2:
   ld t0, 0(a0)
   sd t0, 24(a0)
   ret
