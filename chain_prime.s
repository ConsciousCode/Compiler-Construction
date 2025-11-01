.text
.globl chain_prime
chain_prime:
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_0:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_2
   j L_or_3
L_or_3:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_2
   j L_while_body_1
L_while_body_1:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_0
L_while_end_2:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_4
   j L_if_false_5
L_if_true_4:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_6
L_if_false_5:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_6:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_7:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_10
   j L_while_end_9
L_and_10:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_8
   j L_while_end_9
L_while_body_8:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_11:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_12
   j L_while_end_13
L_while_body_12:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_11
L_while_end_13:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_14:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_17
   j L_while_end_16
L_and_17:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_15
   j L_while_end_16
L_while_body_15:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_18:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_19
   j L_while_end_20
L_while_body_19:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_18
L_while_end_20:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_21
   j L_if_false_22
L_if_true_21:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_23
L_if_false_22:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_23:
   j L_while_cond_14
L_while_end_16:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_24
   j L_if_false_25
L_if_true_24:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_26
L_if_false_25:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_26:
   j L_while_cond_7
L_while_end_9:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_27:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_29
   j L_or_30
L_or_30:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_29
   j L_while_body_28
L_while_body_28:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_27
L_while_end_29:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_31
   j L_if_false_32
L_if_true_31:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_33
L_if_false_32:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_33:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_34:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_37
   j L_while_end_36
L_and_37:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_35
   j L_while_end_36
L_while_body_35:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_38:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_39
   j L_while_end_40
L_while_body_39:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_38
L_while_end_40:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_41:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_44
   j L_while_end_43
L_and_44:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_42
   j L_while_end_43
L_while_body_42:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_45:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_46
   j L_while_end_47
L_while_body_46:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_45
L_while_end_47:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_48
   j L_if_false_49
L_if_true_48:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_50
L_if_false_49:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_50:
   j L_while_cond_41
L_while_end_43:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_51
   j L_if_false_52
L_if_true_51:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_53
L_if_false_52:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_53:
   j L_while_cond_34
L_while_end_36:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_54:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_56
   j L_or_57
L_or_57:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_56
   j L_while_body_55
L_while_body_55:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_54
L_while_end_56:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_58
   j L_if_false_59
L_if_true_58:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_60
L_if_false_59:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_60:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_61:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_64
   j L_while_end_63
L_and_64:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_62
   j L_while_end_63
L_while_body_62:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_65:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_66
   j L_while_end_67
L_while_body_66:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_65
L_while_end_67:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_68:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_71
   j L_while_end_70
L_and_71:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_69
   j L_while_end_70
L_while_body_69:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_72:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_73
   j L_while_end_74
L_while_body_73:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_72
L_while_end_74:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_75
   j L_if_false_76
L_if_true_75:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_77
L_if_false_76:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_77:
   j L_while_cond_68
L_while_end_70:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_78
   j L_if_false_79
L_if_true_78:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_80
L_if_false_79:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_80:
   j L_while_cond_61
L_while_end_63:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_81:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_83
   j L_or_84
L_or_84:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_83
   j L_while_body_82
L_while_body_82:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_81
L_while_end_83:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_85
   j L_if_false_86
L_if_true_85:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_87
L_if_false_86:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_87:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_88:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_91
   j L_while_end_90
L_and_91:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_89
   j L_while_end_90
L_while_body_89:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_92:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_93
   j L_while_end_94
L_while_body_93:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_92
L_while_end_94:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_95:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_98
   j L_while_end_97
L_and_98:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_96
   j L_while_end_97
L_while_body_96:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_99:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_100
   j L_while_end_101
L_while_body_100:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_99
L_while_end_101:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_102
   j L_if_false_103
L_if_true_102:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_104
L_if_false_103:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_104:
   j L_while_cond_95
L_while_end_97:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_105
   j L_if_false_106
L_if_true_105:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_107
L_if_false_106:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_107:
   j L_while_cond_88
L_while_end_90:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_108:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_110
   j L_or_111
L_or_111:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_110
   j L_while_body_109
L_while_body_109:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_108
L_while_end_110:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_112
   j L_if_false_113
L_if_true_112:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_114
L_if_false_113:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_114:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_115:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_118
   j L_while_end_117
L_and_118:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_116
   j L_while_end_117
L_while_body_116:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_119:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_120
   j L_while_end_121
L_while_body_120:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_119
L_while_end_121:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_122:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_125
   j L_while_end_124
L_and_125:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_123
   j L_while_end_124
L_while_body_123:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_126:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_127
   j L_while_end_128
L_while_body_127:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_126
L_while_end_128:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_129
   j L_if_false_130
L_if_true_129:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_131
L_if_false_130:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_131:
   j L_while_cond_122
L_while_end_124:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_132
   j L_if_false_133
L_if_true_132:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_134
L_if_false_133:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_134:
   j L_while_cond_115
L_while_end_117:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_135:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_137
   j L_or_138
L_or_138:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_137
   j L_while_body_136
L_while_body_136:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_135
L_while_end_137:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_139
   j L_if_false_140
L_if_true_139:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_141
L_if_false_140:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_141:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_142:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_145
   j L_while_end_144
L_and_145:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_143
   j L_while_end_144
L_while_body_143:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_146:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_147
   j L_while_end_148
L_while_body_147:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_146
L_while_end_148:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_149:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_152
   j L_while_end_151
L_and_152:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_150
   j L_while_end_151
L_while_body_150:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_153:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_154
   j L_while_end_155
L_while_body_154:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_153
L_while_end_155:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_156
   j L_if_false_157
L_if_true_156:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_158
L_if_false_157:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_158:
   j L_while_cond_149
L_while_end_151:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_159
   j L_if_false_160
L_if_true_159:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_161
L_if_false_160:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_161:
   j L_while_cond_142
L_while_end_144:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_162:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_164
   j L_or_165
L_or_165:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_164
   j L_while_body_163
L_while_body_163:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_162
L_while_end_164:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_166
   j L_if_false_167
L_if_true_166:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_168
L_if_false_167:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_168:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_169:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_172
   j L_while_end_171
L_and_172:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_170
   j L_while_end_171
L_while_body_170:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_173:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_174
   j L_while_end_175
L_while_body_174:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_173
L_while_end_175:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_176:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_179
   j L_while_end_178
L_and_179:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_177
   j L_while_end_178
L_while_body_177:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_180:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_181
   j L_while_end_182
L_while_body_181:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_180
L_while_end_182:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_183
   j L_if_false_184
L_if_true_183:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_185
L_if_false_184:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_185:
   j L_while_cond_176
L_while_end_178:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_186
   j L_if_false_187
L_if_true_186:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_188
L_if_false_187:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_188:
   j L_while_cond_169
L_while_end_171:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_189:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_191
   j L_or_192
L_or_192:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_191
   j L_while_body_190
L_while_body_190:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_189
L_while_end_191:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_193
   j L_if_false_194
L_if_true_193:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_195
L_if_false_194:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_195:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_196:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_199
   j L_while_end_198
L_and_199:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_197
   j L_while_end_198
L_while_body_197:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_200:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_201
   j L_while_end_202
L_while_body_201:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_200
L_while_end_202:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_203:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_206
   j L_while_end_205
L_and_206:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_204
   j L_while_end_205
L_while_body_204:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_207:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_208
   j L_while_end_209
L_while_body_208:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_207
L_while_end_209:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_210
   j L_if_false_211
L_if_true_210:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_212
L_if_false_211:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_212:
   j L_while_cond_203
L_while_end_205:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_213
   j L_if_false_214
L_if_true_213:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_215
L_if_false_214:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_215:
   j L_while_cond_196
L_while_end_198:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_216:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_218
   j L_or_219
L_or_219:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_218
   j L_while_body_217
L_while_body_217:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_216
L_while_end_218:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_220
   j L_if_false_221
L_if_true_220:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_222
L_if_false_221:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_222:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_223:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_226
   j L_while_end_225
L_and_226:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_224
   j L_while_end_225
L_while_body_224:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_227:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_228
   j L_while_end_229
L_while_body_228:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_227
L_while_end_229:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_230:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_233
   j L_while_end_232
L_and_233:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_231
   j L_while_end_232
L_while_body_231:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_234:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_235
   j L_while_end_236
L_while_body_235:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_234
L_while_end_236:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_237
   j L_if_false_238
L_if_true_237:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_239
L_if_false_238:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_239:
   j L_while_cond_230
L_while_end_232:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_240
   j L_if_false_241
L_if_true_240:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_242
L_if_false_241:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_242:
   j L_while_cond_223
L_while_end_225:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ld t0, 40(a0)
   sd t0, 16(a0)
   ld t0, 16(a0)
   sd t0, 32(a0)
L_while_cond_243:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_while_end_245
   j L_or_246
L_or_246:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_while_end_245
   j L_while_body_244
L_while_body_244:
   ld t0, 32(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_243
L_while_end_245:
   ld t0, 32(a0)
   li t1, 1
   beq t0, t1, L_if_true_247
   j L_if_false_248
L_if_true_247:
   ld t0, 16(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 16(a0)
   j L_if_end_249
L_if_false_248:
   ld t0, 16(a0)
   li t1, 1
   sub t0, t0, t1
   sd t0, 16(a0)
L_if_end_249:
   ld t0, 16(a0)
   sd t0, 8(a0)
   li t0, 0
   sd t0, 64(a0)
L_while_cond_250:
   ld t0, 8(a0)
   li t1, 2
   bge t0, t1, L_and_253
   j L_while_end_252
L_and_253:
   ld t0, 64(a0)
   li t1, 0
   beq t0, t1, L_while_body_251
   j L_while_end_252
L_while_body_251:
   li t0, 1
   sd t0, 48(a0)
L_while_cond_254:
   ld t0, 48(a0)
   ld t1, 48(a0)
   mul t0, t0, t1
   ld t1, 8(a0)
   bge t1, t0, L_while_body_255
   j L_while_end_256
L_while_body_255:
   ld t0, 48(a0)
   li t1, 1
   add t0, t0, t1
   sd t0, 48(a0)
   j L_while_cond_254
L_while_end_256:
   li t0, 3
   sd t0, 24(a0)
   li t0, 0
   sd t0, 56(a0)
L_while_cond_257:
   ld t0, 24(a0)
   ld t1, 48(a0)
   bge t1, t0, L_and_260
   j L_while_end_259
L_and_260:
   ld t0, 56(a0)
   li t1, 0
   beq t0, t1, L_while_body_258
   j L_while_end_259
L_while_body_258:
   ld t0, 8(a0)
   sd t0, 32(a0)
L_while_cond_261:
   ld t0, 32(a0)
   li t1, 0
   blt t1, t0, L_while_body_262
   j L_while_end_263
L_while_body_262:
   ld t0, 32(a0)
   ld t1, 24(a0)
   sub t0, t0, t1
   sd t0, 32(a0)
   j L_while_cond_261
L_while_end_263:
   ld t0, 32(a0)
   li t1, 0
   beq t0, t1, L_if_true_264
   j L_if_false_265
L_if_true_264:
   li t0, 1
   sd t0, 56(a0)
   j L_if_end_266
L_if_false_265:
   ld t0, 24(a0)
   li t1, 2
   add t0, t0, t1
   sd t0, 24(a0)
L_if_end_266:
   j L_while_cond_257
L_while_end_259:
   ld t0, 24(a0)
   ld t1, 48(a0)
   blt t1, t0, L_if_true_267
   j L_if_false_268
L_if_true_267:
   li t0, 1
   sd t0, 64(a0)
   j L_if_end_269
L_if_false_268:
   ld t0, 8(a0)
   li t1, 2
   sub t0, t0, t1
   sd t0, 8(a0)
L_if_end_269:
   j L_while_cond_250
L_while_end_252:
   ld t0, 8(a0)
   sd t0, 0(a0)
   ld t0, 0(a0)
   sd t0, 40(a0)
   ret
