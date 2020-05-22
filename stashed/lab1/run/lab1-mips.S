.data
  .globl n
  .align 2
n:
  .space 4
  .globl m
  .align 2
m:
  .space 4
  .globl img
  .align 2
img:
  .space 600000
  .globl kernel
  .align 2
kernel:
  .half 1
  .half 2
  .half 1
  .half 2
  .half 4
  .half 2
  .half 1
  .half 2
  .half 1

.text
_printf_num:    # print($4)
    li $v0, 1
    add $a0, $4, $zero
    syscall
    j   $31
    nop

_scanf_num: # $4 = read()
    li $v0, 5
    syscall
    add $4, $v0, $zero
    j   $31
    nop

_printf_char: # print($4)
    li $v0, 11
    add $a0, $4, $zero
    syscall
    j   $31
    nop

    .globl  main
    .ent    main
main:
    addiu   $sp, $sp, -32
    sw  $31, 28($sp)

    # read n
    la  $5, n
    jal _scanf_num
    nop
    sw  $4, 0($5)

    # read m
    la  $6, m
    jal _scanf_num
    nop
    sw  $4, 0($6)

    # Read img[n][m]
    la  $5, n
    lw  $4, 0($5)
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    la  $6, m
    lw  $4, 0($6)
    jal _printf_num
    nop
    li  $4, 10
    jal _printf_char
    nop
    lw $19, 0($6)
    sll $19, $19, 1        # $19 = 2m
    addiu $20, $19, 2      # $20 = 2m + 2
    li $22, 2
    subu $21, $19, $22     # $21 = 2m - 2
    lw  $17, 0($5)
    lw  $18, 0($6)
    la  $7, img
    move    $16, $0
$ReadImg_External:
    move    $15, $0
$ReadImg_Internal:
    jal _scanf_num
    nop
    sh  $4, 0($7)
    addiu   $7, $7, 2
    addiu   $15, $15, 1
    bne     $18, $15, $ReadImg_Internal
    nop
    addiu   $16, $16, 1
    bne     $17, $16, $ReadImg_External
    nop

    # proc & print img[i][j]
    la  $5, n
    la  $6, m
    li  $22, 1
    lw $17, 0($5)
    lw $18, 0($6)
    la  $7, img
    move    $15, $0
    
$First_Row:
    lhu $4, 0($7)
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    addiu   $7, $7, 2
    addiu   $15, $15, 1
    bne     $18, $15, $First_Row
    li      $4, 10
    jal _printf_char
    nop
    li    $16, 1

    subu  $17, $17, $22   # $17 = n - 1
    subu  $18, $18, $22   # $18 = m - 1
$PrintImg_External:
    lhu $4, 0($7)
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    addiu   $7, $7, 2
    li    $15, 1
$PrintImg_Internal:
    lhu $4, 0($7)
    sll $4, $4, 1
    lhu $22, 2($7)
    addu $4, $4, $22
    lhu $22, -2($7)
    addu $4, $4, $22
    addu $23, $7, $19
    lhu $22, 0($23)
    addu $4, $4, $22
    subu $23, $7, $19
    lhu $22, 0($23)
    addu $4, $4, $22
    sll $4, $4, 1
    subu $23, $7, $20
    lhu $22, 0($23)
    addu $4, $4, $22
    addu $23, $7, $20
    lhu $22, 0($23)
    addu $4, $4, $22
    subu $23, $7, $21
    lhu $22, 0($23)
    addu $4, $4, $22
    addu $23, $7, $21
    lhu $22, 0($23)
    addu $4, $4, $22
    srl $4, $4, 4
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    addiu   $7, $7, 2
    addiu   $15, $15, 1
    bne     $18, $15, $PrintImg_Internal
    nop
    lhu $4, 0($7)
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    li      $4, 10
    jal _printf_char
    nop
    addiu   $7, $7, 2
    addiu   $16, $16, 1
    bne     $17, $16, $PrintImg_External
    nop
    
    move $15, $0
    lw $18, 0($6)
$Last_Row:
    lhu $4, 0($7)
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    addiu   $7, $7, 2
    addiu   $15, $15, 1
    bne     $18, $15, $Last_Row
    li      $4, 10
    jal _printf_char
    nop

    lw  $31, 28($sp)
    j   $31
    addiu   $sp, $sp, 32
    .end main

