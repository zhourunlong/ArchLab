    .file    1 "check_2.S"
# This program outputs a fixed value and do conduct more test cases with extra instruction types. You can add your own cases.
# You can get it right without handling RAW hazard and delay slot.
# However, PASS ALL does not mean that your functionality is all done. You need careful validation.
    .text
    .align  2
    .globl  main
    .ent    main
main:
    .frame  $fp,40,$31          # vars= 8, regs= 2/0, args= 16, gp= 8
    addiu   $sp,$sp,-40

    addu    $4,$zero,0x9        # value to be printed
    addiu   $v0,$zero,0x1
    nop
    nop
    syscall                     # syscall with PRINT_INT commands
    nop

    addiu   $4,$zero,10         # 0xa=='\n'
    addiu   $v0,$zero,0xb
    nop
    nop
    syscall                     # syscall with PRINT_CHAR commands
    nop

    # here are 2 test cases for extra instruction types:
    lui     $s1,0xdead          # test LUI, ORI
    nop
    nop
    ori     $s1,0xbeef
    sll     $s0,$zero,0         # test SLL
    nop
    nop
    sw      $s1,0($sp)          # test LW
    nop
    nop
    lw      $s2,0($sp)          # test SW
    nop
    nop
    beq     $s1,$s2,L_PASS1     # test BEQ
    nop
    ori     $s0,$s0,0x1
L_PASS1:
    ori     $s3,$zero,0xdead
    nop
    nop
    lhu     $s4,2($sp)          # test LHU
    nop
    nop
    beq     $s3,$s4,L_PASS2
    nop
    ori     $s0,$s0,0x1
    nop
    nop
L_PASS2:


    # Finish all tests. Output the final result.
    # 0 means PASS. 1 means FAIL.
    addu    $4,$zero,$s0        # value to be printed
    addiu   $v0,$zero,0x1
    nop
    nop
    syscall                     # syscall with PRINT_INT commands
    nop

    addiu   $sp,$sp,40          # restoring frame (unnecessary)
    addiu   $v0,$zero,0xa       # syscall with EXIT commands
    nop
    nop                         # insert bubbles to wait for addiu completion
    syscall
    nop
    .end    main
