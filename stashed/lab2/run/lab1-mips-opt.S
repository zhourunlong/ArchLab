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

    # print n, m & preproc
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
    sll $20, $19, 1        # $20 = 4m
    lw  $17, 0($5)
    lw  $18, 0($6)
    la  $7, img

    # read img[0][*] & output img[0][*]
    move    $15, $0
$First_Row:
    jal _scanf_num
    nop
    sh  $4, 0($7)
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    addiu   $7, $7, 2
    addiu   $15, $15, 1
    bne     $18, $15, $First_Row
    nop
    li      $4, 10
    jal _printf_char
    nop

    # read img[1][*]
    move    $15, $0
$Second_Row:
    jal _scanf_num
    nop
    sh  $4, 0($7)
    addiu   $7, $7, 2
    addiu   $15, $15, 1
    bne     $18, $15, $Second_Row
    nop

    # read img[i][*] & output img'[i-1][*]

    #  $14 | $25 | $10
    # -----+-----+-----
    #  $13 | $24 | $9
    # -----+-----+-----
    #  $12 | $23 | $8

    li    $16, 2
$External:
    jal _scanf_num
    nop
    sh  $4, 0($7)
    subu $21, $7, $19  # $21 = $7 - 2m
    lhu $4, 0($21)
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    addiu   $7, $7, 2
    jal _scanf_num
    nop
    sh  $4, 0($7)
    
    subu $21, $7, $19   # $21 = $7 - 2m
    subu $22, $7, $20   # $22 = $7 - 4m
    move $8, $4
    lhu $9, 0($21)
    lhu $10, 0($22)
    lhu $23, -2($7)
    lhu $24, -2($21)
    lhu $25, -2($22)
    
    li    $15, 2

$Internal:
    move $14, $25
    move $25, $10
    move $13, $24
    move $24, $9
    move $12, $23
    move $23, $8

    addiu   $7, $7, 2
    jal _scanf_num
    nop
    sh  $4, 0($7)
    move $8, $4
    
    addiu $21, $21, 2   # $21 = $7 - 2m
    addiu $22, $22, 2   # $22 = $7 - 4m

    lhu $10, 0($22)
    lhu $9, 0($21)

    sll $4, $24, 1
    addu $4, $4, $13
    addu $4, $4, $23
    addu $4, $4, $9
    addu $4, $4, $25
    sll $4, $4, 1
    addu $4, $4, $14
    addu $4, $4, $10
    addu $4, $4, $12
    addu $4, $4, $8
    srl $4, $4, 4

    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    addiu   $15, $15, 1
    bne     $18, $15, $Internal
    nop

    move $4, $9
    jal _printf_num
    nop
    li  $4, 32
    jal _printf_char
    nop
    li      $4, 10
    jal _printf_char
    nop
    addiu   $16, $16, 1
    addiu   $7, $7, 2
    bne     $17, $16, $External
    nop
    
    # output img[n-1][*]
    move $15, $0
    addiu $7, $21, 2
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
    nop
    li      $4, 10
    jal _printf_char
    nop

    lw  $31, 28($sp)
    j   $31
    nop
    addiu   $sp, $sp, 32
    .end main

