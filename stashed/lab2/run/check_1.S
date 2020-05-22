    .file    1 "check_1.S"
# This program outputs a fixed value (at line 11) to the screen with a '\n'.
# You can get it right without handling RAW hazard and delay slot.
    .text
    .align    2
    .globl    main
    .ent    main
main:
    .frame  $fp,40,$31        # vars= 8, regs= 2/0, args= 16, gp= 8
	addiu	$sp,$sp,-40

    addiu   $4,$zero,0x7        # value to be printed
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

    addiu   $sp,$sp,40          # restoring frame (unnecessary)
    addiu   $v0,$zero,0xa       # syscall with EXIT commands
    nop
    nop                         # insert bubbles to wait for addiu completion
    syscall
    nop
    .end    main
