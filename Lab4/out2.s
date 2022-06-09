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

main :
  addi $fp, $sp, 4
  li $t0, 0
  addi $sp, $sp, -4
  sw $t0, -8($fp)
  addi $sp, $sp, -20
  li $t0, 4
  li $t1, 0
  mul $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -32($fp)
  addi $sp, $sp, -4
  la $t0, -36($fp)
  lw $t1, -32($fp)
  add $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -40($fp)
  li $t0, 7
  lw $t1, -40($fp)
  sw $t0, 0($t1)
  sw $t1, -40($fp)
  li $t0, 4
  li $t1, 1
  mul $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -44($fp)
  la $t0, -36($fp)
  lw $t1, -44($fp)
  add $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -48($fp)
  li $t0, 8
  lw $t1, -48($fp)
  sw $t0, 0($t1)
  sw $t1, -48($fp)
  li $t0, 4
  li $t1, 2
  mul $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -52($fp)
  la $t0, -36($fp)
  lw $t1, -52($fp)
  add $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -56($fp)
  li $t0, 10
  lw $t1, -56($fp)
  sw $t0, 0($t1)
  sw $t1, -56($fp)
  li $t0, 0
  addi $t1, $t0, -1
  addi $sp, $sp, -4
  sw $t1, -60($fp)
  li $t0, 4
  li $t1, 3
  mul $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -64($fp)
  la $t0, -36($fp)
  lw $t1, -64($fp)
  add $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -68($fp)
  lw $t0, -60($fp)
  lw $t1, -68($fp)
  sw $t0, 0($t1)
  sw $t1, -68($fp)
  li $t0, 4
  li $t1, 4
  mul $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -72($fp)
  la $t0, -36($fp)
  lw $t1, -72($fp)
  add $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -76($fp)
  li $t0, 2
  lw $t1, -76($fp)
  sw $t0, 0($t1)
  sw $t1, -76($fp)
label0 :
  lw $t0, -8($fp)
  li $t1, 5
  blt $t0, $t1, label1
  j label2
label1 :
  li $t0, 4
  lw $t1, -8($fp)
  mul $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -80($fp)
  la $t0, -36($fp)
  lw $t1, -80($fp)
  add $t2, $t0, $t1
  addi $sp, $sp, -4
  sw $t2, -84($fp)
  lw $t0, -84($fp)
  lw $t1, 0($t0)
  addi $sp, $sp, -4
  sw $t1, -88($fp)
  lw $t0, -88($fp)
  move $t1, $t0
  addi $sp, $sp, -4
  sw $t1, -92($fp)
  lw $a0, -92($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t0, -8($fp)
  addi $t1, $t0, 1
  addi $sp, $sp, -4
  sw $t1, -96($fp)
  lw $t0, -96($fp)
  move $t1, $t0
  sw $t1, -8($fp)
  j label0
label2 :
  li $t0, 0
  move $v0, $t0
  jr $ra
