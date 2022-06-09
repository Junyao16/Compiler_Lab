.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main

.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra

write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra

fact :
  addi $fp, $sp, 16
  lw $t0, -8($fp)
  li $t1, 1
  beq $t0, $t1, label0
  j label1
label0 :
  lw $t0, -8($fp)
  move $v0, $t0
  jr $ra
  j label2
label1 :
  lw $t0, -8($fp)
  addi $t1, $t0, -1
  addi $sp, $sp, -4
  sw $t1, -20($fp)
  addi $sp, $sp, -4
  lw $t0, -20($fp)
  sw $t0, 0($sp)
  addi $sp, $sp, -8
  sw $fp, 4($sp)
  sw $ra, 0($sp)
  jal fact
  move $t0, $v0
  addi $sp, $fp, -16
  lw $fp, 4($sp)
  lw $ra, 0($sp)
  addi $sp, $sp, 8
  addi $sp, $sp, 4
  addi $sp, $sp, -4
  sw $t0, -24($fp)
  lw $t0, -8($fp)
  lw $t1, -24($fp)
  mul $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -28($fp)
  lw $t0, -28($fp)
  move $v0, $t0
  jr $ra
label2 :
main :
  addi $fp, $sp, 4
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  addi $sp, $sp, -4
  sw $t0, -8($fp)
  lw $t0, -8($fp)
  move $t1, $t0
  addi $sp, $sp, -4
  sw $t1, -12($fp)
  lw $t0, -12($fp)
  li $t1, 1
  bgt $t0, $t1, label3
  j label4
label3 :
  addi $sp, $sp, -4
  lw $t0, -12($fp)
  sw $t0, 0($sp)
  addi $sp, $sp, -8
  sw $fp, 4($sp)
  sw $ra, 0($sp)
  jal fact
  move $t0, $v0
  addi $sp, $fp, -16
  lw $fp, 4($sp)
  lw $ra, 0($sp)
  addi $sp, $sp, 8
  addi $sp, $sp, 4
  addi $sp, $sp, -4
  sw $t0, -16($fp)
  lw $t0, -16($fp)
  move $t1, $t0
  addi $sp, $sp, -4
  sw $t1, -20($fp)
  j label5
label4 :
  li $t0, 1
  sw $t0, -20($fp)
label5 :
  lw $a0, -20($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $t0, 0
  move $v0, $t0
  jr $ra
