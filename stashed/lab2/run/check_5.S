    .file    1 "check_3.S"
# This program acts as simple_add with procedure calling.
# You can get it right without handling RAW hazard and delay slot.
    .text
    .align  2
    .globl  _scanf_num
    .ent    _scanf_num
_scanf_num:
    .frame  $fp,24,$31        # vars= 8, regs= 1/0, args= 0, gp= 8
    addiu   $sp,$sp,-24
    nop
    nop
    sw      $fp,20($sp)
    move    $fp,$sp
    li      $v0, 5
    nop
    nop
    syscall
    nop
    nop
    add     $3, $v0, $zero
    nop
    nop
    sw      $3,8($fp)
    nop
    nop
    lw      $2,8($fp)
    move    $sp,$fp
    nop
    nop
    lw      $fp,20($sp)
    addiu   $sp,$sp,24
    j       $31
    nop
    .end    _scanf_num

    .align  2
    .globl  main
    .ent    main
main:
    .frame  $fp,40,$31          # vars= 8, regs= 2/0, args= 16, gp= 8
    addiu   $sp,$sp,-40

    li $10, 100
    li $11, 200
    beq $10, $11, $Go
    li $13, 400
$Go:
    li $12, 300

    addiu   $sp,$sp,40          # restoring frame (unnecessary)
    addiu   $v0,$zero,0xa       # syscall with EXIT commands
    nop
    nop                         # insert bubbles to wait for addiu completion
    syscall
    nop
    .end    main
