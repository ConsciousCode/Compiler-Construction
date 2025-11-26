#vars: output=s1, w6=s10, t=s2, w8=s12, w0=s3, w1=s4, w2=s6, w7=s11, w10=s5, w3=s7, w4=s8, w5=s9, w9=s13, 
.text
.globl comparatornetwork_bestknown_for_11
comparatornetwork_bestknown_for_11:
   addi sp, sp, -80
   # s13 <- input (w9)
   ld s13, 96(a0)
   # s9 <- input (w5)
   ld s9, 64(a0)
   # s8 <- input (w4)
   ld s8, 56(a0)
   # s7 <- input (w3)
   ld s7, 48(a0)
   # s5 <- input (w10)
   ld s5, 32(a0)
   # s11 <- input (w7)
   ld s11, 80(a0)
   # s6 <- input (w2)
   ld s6, 40(a0)
   # s4 <- input (w1)
   ld s4, 24(a0)
   # s3 <- input (w0)
   ld s3, 16(a0)
   # s12 <- input (w8)
   ld s12, 88(a0)
   # s2 <- input (t)
   ld s2, 8(a0)
   # s10 <- input (w6)
   ld s10, 72(a0)
   # s1 <- input (output)
   ld s1, 0(a0)
   
# if [w0 < w1]
L_cond_0:
   mv t2, s3
   mv t3, s4
   blt t2, t3, L_true_1
   j L_false_2
# then
   # Skip
L_true_1:
# if [w2 < w3]
L_cond_5:
   mv t2, s6
   mv t3, s7
   blt t2, t3, L_true_6
   j L_false_7
# then
   # Skip
L_true_6:
# if [w4 < w5]
L_cond_10:
   mv t2, s8
   mv t3, s9
   blt t2, t3, L_true_11
   j L_false_12
# then
   # Skip
L_true_11:
# if [w6 < w7]
L_cond_15:
   mv t2, s10
   mv t3, s11
   blt t2, t3, L_true_16
   j L_false_17
# then
   # Skip
L_true_16:
# if [w8 < w9]
L_cond_20:
   mv t2, s12
   mv t3, s13
   blt t2, t3, L_true_21
   j L_false_22
# then
   # Skip
L_true_21:
# if [w1 < w3]
L_cond_25:
   mv t2, s4
   mv t3, s7
   blt t2, t3, L_true_26
   j L_false_27
# then
   # Skip
L_true_26:
# if [w5 < w7]
L_cond_30:
   mv t2, s9
   mv t3, s11
   blt t2, t3, L_true_31
   j L_false_32
# then
   # Skip
L_true_31:
# if [w0 < w2]
L_cond_35:
   mv t2, s3
   mv t3, s6
   blt t2, t3, L_true_36
   j L_false_37
# then
   # Skip
L_true_36:
# if [w4 < w6]
L_cond_40:
   mv t2, s8
   mv t3, s10
   blt t2, t3, L_true_41
   j L_false_42
# then
   # Skip
L_true_41:
# if [w8 < w10]
L_cond_45:
   mv t2, s12
   mv t3, s5
   blt t2, t3, L_true_46
   j L_false_47
# then
   # Skip
L_true_46:
# if [w1 < w2]
L_cond_50:
   mv t2, s4
   mv t3, s6
   blt t2, t3, L_true_51
   j L_false_52
# then
   # Skip
L_true_51:
# if [w5 < w6]
L_cond_55:
   mv t2, s9
   mv t3, s10
   blt t2, t3, L_true_56
   j L_false_57
# then
   # Skip
L_true_56:
# if [w9 < w10]
L_cond_60:
   mv t2, s13
   mv t3, s5
   blt t2, t3, L_true_61
   j L_false_62
# then
   # Skip
L_true_61:
# if [w1 < w5]
L_cond_65:
   mv t2, s4
   mv t3, s9
   blt t2, t3, L_true_66
   j L_false_67
# then
   # Skip
L_true_66:
# if [w6 < w10]
L_cond_70:
   mv t2, s10
   mv t3, s5
   blt t2, t3, L_true_71
   j L_false_72
# then
   # Skip
L_true_71:
# if [w5 < w9]
L_cond_75:
   mv t2, s9
   mv t3, s13
   blt t2, t3, L_true_76
   j L_false_77
# then
   # Skip
L_true_76:
# if [w2 < w6]
L_cond_80:
   mv t2, s6
   mv t3, s10
   blt t2, t3, L_true_81
   j L_false_82
# then
   # Skip
L_true_81:
# if [w1 < w5]
L_cond_85:
   mv t2, s4
   mv t3, s9
   blt t2, t3, L_true_86
   j L_false_87
# then
   # Skip
L_true_86:
# if [w6 < w10]
L_cond_90:
   mv t2, s10
   mv t3, s5
   blt t2, t3, L_true_91
   j L_false_92
# then
   # Skip
L_true_91:
# if [w0 < w4]
L_cond_95:
   mv t2, s3
   mv t3, s8
   blt t2, t3, L_true_96
   j L_false_97
# then
   # Skip
L_true_96:
# if [w3 < w7]
L_cond_100:
   mv t2, s7
   mv t3, s11
   blt t2, t3, L_true_101
   j L_false_102
# then
   # Skip
L_true_101:
# if [w4 < w8]
L_cond_105:
   mv t2, s8
   mv t3, s12
   blt t2, t3, L_true_106
   j L_false_107
# then
   # Skip
L_true_106:
# if [w0 < w4]
L_cond_110:
   mv t2, s3
   mv t3, s8
   blt t2, t3, L_true_111
   j L_false_112
# then
   # Skip
L_true_111:
# if [w1 < w4]
L_cond_115:
   mv t2, s4
   mv t3, s8
   blt t2, t3, L_true_116
   j L_false_117
# then
   # Skip
L_true_116:
# if [w7 < w10]
L_cond_120:
   mv t2, s11
   mv t3, s5
   blt t2, t3, L_true_121
   j L_false_122
# then
   # Skip
L_true_121:
# if [w3 < w8]
L_cond_125:
   mv t2, s7
   mv t3, s12
   blt t2, t3, L_true_126
   j L_false_127
# then
   # Skip
L_true_126:
# if [w2 < w3]
L_cond_130:
   mv t2, s6
   mv t3, s7
   blt t2, t3, L_true_131
   j L_false_132
# then
   # Skip
L_true_131:
# if [w8 < w9]
L_cond_135:
   mv t2, s12
   mv t3, s13
   blt t2, t3, L_true_136
   j L_false_137
# then
   # Skip
L_true_136:
# if [w2 < w4]
L_cond_140:
   mv t2, s6
   mv t3, s8
   blt t2, t3, L_true_141
   j L_false_142
# then
   # Skip
L_true_141:
# if [w7 < w9]
L_cond_145:
   mv t2, s11
   mv t3, s13
   blt t2, t3, L_true_146
   j L_false_147
# then
   # Skip
L_true_146:
# if [w3 < w5]
L_cond_150:
   mv t2, s7
   mv t3, s9
   blt t2, t3, L_true_151
   j L_false_152
# then
   # Skip
L_true_151:
# if [w6 < w8]
L_cond_155:
   mv t2, s10
   mv t3, s12
   blt t2, t3, L_true_156
   j L_false_157
# then
   # Skip
L_true_156:
# if [w3 < w4]
L_cond_160:
   mv t2, s7
   mv t3, s8
   blt t2, t3, L_true_161
   j L_false_162
# then
   # Skip
L_true_161:
# if [w5 < w6]
L_cond_165:
   mv t2, s9
   mv t3, s10
   blt t2, t3, L_true_166
   j L_false_167
# then
   # Skip
L_true_166:
# if [w7 < w8]
L_cond_170:
   mv t2, s11
   mv t3, s12
   blt t2, t3, L_true_171
   j L_false_172
# then
   # Skip
L_true_171:
L_block_175:
   # output := ((((((((((w0 + w1) + w2) + w3) + w4) + w5) + w6) + w7) + w8) + w9) + w10)
   mv t0, s3
   sd t0, 0(sp)
   mv t1, s4
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s6
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s7
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s8
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s9
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s10
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s11
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s12
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s13
   ld t0, 0(sp)
   add t0, t0, t1
   sd t0, 0(sp)
   mv t1, s5
   ld t0, 0(sp)
   add s1, t0, t1
   j L_return
# else
L_false_172:
   # t := w7
   mv s2, s11
   # w7 := w8
   mv s11, s12
   # w8 := t
   mv s12, s2
   j L_block_175
# else
L_false_167:
   # t := w5
   mv s2, s9
   # w5 := w6
   mv s9, s10
   # w6 := t
   mv s10, s2
   j L_cond_170
# else
L_false_162:
   # t := w3
   mv s2, s7
   # w3 := w4
   mv s7, s8
   # w4 := t
   mv s8, s2
   j L_cond_165
# else
L_false_157:
   # t := w6
   mv s2, s10
   # w6 := w8
   mv s10, s12
   # w8 := t
   mv s12, s2
   j L_cond_160
# else
L_false_152:
   # t := w3
   mv s2, s7
   # w3 := w5
   mv s7, s9
   # w5 := t
   mv s9, s2
   j L_cond_155
# else
L_false_147:
   # t := w7
   mv s2, s11
   # w7 := w9
   mv s11, s13
   # w9 := t
   mv s13, s2
   j L_cond_150
# else
L_false_142:
   # t := w2
   mv s2, s6
   # w2 := w4
   mv s6, s8
   # w4 := t
   mv s8, s2
   j L_cond_145
# else
L_false_137:
   # t := w8
   mv s2, s12
   # w8 := w9
   mv s12, s13
   # w9 := t
   mv s13, s2
   j L_cond_140
# else
L_false_132:
   # t := w2
   mv s2, s6
   # w2 := w3
   mv s6, s7
   # w3 := t
   mv s7, s2
   j L_cond_135
# else
L_false_127:
   # t := w3
   mv s2, s7
   # w3 := w8
   mv s7, s12
   # w8 := t
   mv s12, s2
   j L_cond_130
# else
L_false_122:
   # t := w7
   mv s2, s11
   # w7 := w10
   mv s11, s5
   # w10 := t
   mv s5, s2
   j L_cond_125
# else
L_false_117:
   # t := w1
   mv s2, s4
   # w1 := w4
   mv s4, s8
   # w4 := t
   mv s8, s2
   j L_cond_120
# else
L_false_112:
   # t := w0
   mv s2, s3
   # w0 := w4
   mv s3, s8
   # w4 := t
   mv s8, s2
   j L_cond_115
# else
L_false_107:
   # t := w4
   mv s2, s8
   # w4 := w8
   mv s8, s12
   # w8 := t
   mv s12, s2
   j L_cond_110
# else
L_false_102:
   # t := w3
   mv s2, s7
   # w3 := w7
   mv s7, s11
   # w7 := t
   mv s11, s2
   j L_cond_105
# else
L_false_97:
   # t := w0
   mv s2, s3
   # w0 := w4
   mv s3, s8
   # w4 := t
   mv s8, s2
   j L_cond_100
# else
L_false_92:
   # t := w6
   mv s2, s10
   # w6 := w10
   mv s10, s5
   # w10 := t
   mv s5, s2
   j L_cond_95
# else
L_false_87:
   # t := w1
   mv s2, s4
   # w1 := w5
   mv s4, s9
   # w5 := t
   mv s9, s2
   j L_cond_90
# else
L_false_82:
   # t := w2
   mv s2, s6
   # w2 := w6
   mv s6, s10
   # w6 := t
   mv s10, s2
   j L_cond_85
# else
L_false_77:
   # t := w5
   mv s2, s9
   # w5 := w9
   mv s9, s13
   # w9 := t
   mv s13, s2
   j L_cond_80
# else
L_false_72:
   # t := w6
   mv s2, s10
   # w6 := w10
   mv s10, s5
   # w10 := t
   mv s5, s2
   j L_cond_75
# else
L_false_67:
   # t := w1
   mv s2, s4
   # w1 := w5
   mv s4, s9
   # w5 := t
   mv s9, s2
   j L_cond_70
# else
L_false_62:
   # t := w9
   mv s2, s13
   # w9 := w10
   mv s13, s5
   # w10 := t
   mv s5, s2
   j L_cond_65
# else
L_false_57:
   # t := w5
   mv s2, s9
   # w5 := w6
   mv s9, s10
   # w6 := t
   mv s10, s2
   j L_cond_60
# else
L_false_52:
   # t := w1
   mv s2, s4
   # w1 := w2
   mv s4, s6
   # w2 := t
   mv s6, s2
   j L_cond_55
# else
L_false_47:
   # t := w8
   mv s2, s12
   # w8 := w10
   mv s12, s5
   # w10 := t
   mv s5, s2
   j L_cond_50
# else
L_false_42:
   # t := w4
   mv s2, s8
   # w4 := w6
   mv s8, s10
   # w6 := t
   mv s10, s2
   j L_cond_45
# else
L_false_37:
   # t := w0
   mv s2, s3
   # w0 := w2
   mv s3, s6
   # w2 := t
   mv s6, s2
   j L_cond_40
# else
L_false_32:
   # t := w5
   mv s2, s9
   # w5 := w7
   mv s9, s11
   # w7 := t
   mv s11, s2
   j L_cond_35
# else
L_false_27:
   # t := w1
   mv s2, s4
   # w1 := w3
   mv s4, s7
   # w3 := t
   mv s7, s2
   j L_cond_30
# else
L_false_22:
   # t := w8
   mv s2, s12
   # w8 := w9
   mv s12, s13
   # w9 := t
   mv s13, s2
   j L_cond_25
# else
L_false_17:
   # t := w6
   mv s2, s10
   # w6 := w7
   mv s10, s11
   # w7 := t
   mv s11, s2
   j L_cond_20
# else
L_false_12:
   # t := w4
   mv s2, s8
   # w4 := w5
   mv s8, s9
   # w5 := t
   mv s9, s2
   j L_cond_15
# else
L_false_7:
   # t := w2
   mv s2, s6
   # w2 := w3
   mv s6, s7
   # w3 := t
   mv s7, s2
   j L_cond_10
# else
L_false_2:
   # t := w0
   mv s2, s3
   # w0 := w1
   mv s3, s4
   # w1 := t
   mv s4, s2
   j L_cond_5
   
L_return:
   addi sp, sp, 80
   # output (w9) <- s13
   sd s13, 96(a0)
   # output (w5) <- s9
   sd s9, 64(a0)
   # output (w4) <- s8
   sd s8, 56(a0)
   # output (w3) <- s7
   sd s7, 48(a0)
   # output (w10) <- s5
   sd s5, 32(a0)
   # output (w7) <- s11
   sd s11, 80(a0)
   # output (w2) <- s6
   sd s6, 40(a0)
   # output (w1) <- s4
   sd s4, 24(a0)
   # output (w0) <- s3
   sd s3, 16(a0)
   # output (w8) <- s12
   sd s12, 88(a0)
   # output (t) <- s2
   sd s2, 8(a0)
   # output (w6) <- s10
   sd s10, 72(a0)
   # output (output) <- s1
   sd s1, 0(a0)
   ret
