.text
.globl loop
loop:
L_while_cond_0:
   ld t0, 0(a0)
   li t1, 0
   blt t1, t0, L_while_body_1
   j L_while_end_2
L_while_body_1:
   ld t0, 8(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 8(a0)
   ld t0, 0(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 0(a0)
   j L_while_cond_0
L_while_end_2:
   ret
