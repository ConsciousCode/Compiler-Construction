#vars: input=s1, n=s2, output=s3, rem=s5, quot=s4, steps=s6, 
.text
.globl collatz
collatz:
   addi sp, sp, -16
   # s6 <- input (steps)
   ld s6, 40(a0)
   # s4 <- input (quot)
   ld s4, 24(a0)
   # s5 <- input (rem)
   ld s5, 32(a0)
   # s3 <- input (output)
   ld s3, 16(a0)
   # s2 <- input (n)
   ld s2, 8(a0)
   # s1 <- input (input)
   ld s1, 0(a0)
   
   # n := input
   mv s2, s1
   # steps := 0
   li s6, 0
# if [n > 1]
L_cond_2:
   mv t2, s2
   li t3, 1
   blt t3, t2, L_true_3
   j L_false_12
# then
L_true_3:
   # rem := n
   mv s5, s2
   # quot := 0
   li s4, 0
# if [rem > 1]
L_cond_5:
   mv t2, s5
   li t3, 1
   blt t3, t2, L_true_6
   j L_false_8
# then
L_true_6:
   # rem := (rem - 2)
   mv t0, s5
   sd t0, 0(sp)
   li t1, 2
   ld t0, 0(sp)
   sub s5, t0, t1
   # quot := (quot + 1)
   mv t0, s4
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   add s4, t0, t1
   j L_cond_5
# else
# if [rem = 0]
L_false_8:
   mv t2, s5
   li t3, 0
   beq t2, t3, L_true_9
   j L_false_10
# then
L_true_9:
   # n := quot
   mv s2, s4
L_block_11:
   # steps := (steps + 1)
   mv t0, s6
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   add s6, t0, t1
   j L_cond_2
# else
L_false_10:
   # n := ((3 * n) + 1)
   li t0, 3
   sd t0, 0(sp)
   mv t1, s2
   ld t0, 0(sp)
   mul t0, t0, t1
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   add s2, t0, t1
   j L_block_11
# else
L_false_12:
   # output := steps
   mv s3, s6
   j L_return
   
L_return:
   addi sp, sp, 16
   # output (steps) <- s6
   sd s6, 40(a0)
   # output (quot) <- s4
   sd s4, 24(a0)
   # output (rem) <- s5
   sd s5, 32(a0)
   # output (output) <- s3
   sd s3, 16(a0)
   # output (n) <- s2
   sd s2, 8(a0)
   # output (input) <- s1
   sd s1, 0(a0)
   ret
