#vars: n=s1, output=s2, 
.text
.globl loop
loop:
   addi sp, sp, -8
   # s2 <- input (output)
   ld s2, 8(a0)
   # s1 <- input (n)
   ld s1, 0(a0)
   
# if [n > 0]
L_cond_0:
   mv t2, s1
   li t3, 0
   blt t3, t2, L_true_1
   j L_return
# then
L_true_1:
   # output := (output + 1)
   mv t0, s2
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   add s2, t0, t1
   # n := (n - 1)
   mv t0, s1
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   sub s1, t0, t1
   j L_cond_0
   
L_return:
   addi sp, sp, 8
   # output (output) <- s2
   sd s2, 8(a0)
   # output (n) <- s1
   sd s1, 0(a0)
   ret
