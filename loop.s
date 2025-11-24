#vars: count=s1, i=s2, k=s3, output=s4, range=s5, result=s6, 
.text
.globl program
program:
   addi sp, sp, -8
   # s6 <- input (result)
   ld s6, 40(a0)
   # s5 <- input (range)
   ld s5, 32(a0)
   # s4 <- input (output)
   ld s4, 24(a0)
   # s3 <- input (k)
   ld s3, 16(a0)
   # s2 <- input (i)
   ld s2, 8(a0)
   # s1 <- input (count)
   ld s1, 0(a0)
   
   # result := 0
   li s6, 0
# if [range >= 2]
L_cond_1:
   mv t2, s5
   li t3, 2
   bge t2, t3, L_true_2
   j L_false_18
# then
L_true_2:
   # count := 0
   li s1, 0
   # i := 2
   li s2, 2
# if [(i * i) <= range]
L_cond_4:
   mv t0, s2
   sd t0, 0(sp)
   mv t1, s2
   ld t0, 0(sp)
   mul t2, t0, t1
   mv t3, s5
   bge t3, t2, L_true_5
   j L_false_14
# then
L_true_5:
   # k := range
   mv s3, s5
# if [count > 0]
L_cond_6:
   mv t2, s1
   li t3, 0
   blt t3, t2, L_true_7
   j L_false_8
# then
   # Skip
L_true_7:
L_block_13:
   # i := (i + 1)
   mv t0, s2
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   add s2, t0, t1
   j L_cond_4
# else
# if [k >= 1]
L_false_8:
   mv t2, s3
   li t3, 1
   bge t2, t3, L_true_9
   j L_block_13
# then
# if [(k * i) = range]
L_true_9:
   mv t0, s3
   sd t0, 0(sp)
   mv t1, s2
   ld t0, 0(sp)
   mul t2, t0, t1
   mv t3, s5
   beq t2, t3, L_true_10
   j L_false_11
# then
L_true_10:
   # count := (count + 1)
   mv t0, s1
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   add s1, t0, t1
L_block_12:
   # k := (k - 1)
   mv t0, s3
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   sub s3, t0, t1
   j L_false_8
# else
   # Skip
L_false_11:
   j L_block_12
# else
# if [count > 0]
L_false_14:
   mv t2, s1
   li t3, 0
   blt t3, t2, L_true_15
   j L_false_16
# then
   # Skip
L_true_15:
L_block_17:
   # range := (range - 1)
   mv t0, s5
   sd t0, 0(sp)
   li t1, 1
   ld t0, 0(sp)
   sub s5, t0, t1
   j L_cond_1
# else
L_false_16:
   # result := (1 + result)
   li t0, 1
   sd t0, 0(sp)
   mv t1, s6
   ld t0, 0(sp)
   add s6, t0, t1
   j L_block_17
# else
L_false_18:
   # output := result
   mv s4, s6
   j L_return
   
L_return:
   addi sp, sp, 8
   # output (result) <- s6
   sd s6, 40(a0)
   # output (range) <- s5
   sd s5, 32(a0)
   # output (output) <- s4
   sd s4, 24(a0)
   # output (k) <- s3
   sd s3, 16(a0)
   # output (i) <- s2
   sd s2, 8(a0)
   # output (count) <- s1
   sd s1, 0(a0)
   ret
