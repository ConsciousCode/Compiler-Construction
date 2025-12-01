#vars: 
.text
.globl skip
skip:
   addi sp, sp, -0
   
   # Skip
   # Skip
   j L_return
   
L_return:
   addi sp, sp, 0
   ret
