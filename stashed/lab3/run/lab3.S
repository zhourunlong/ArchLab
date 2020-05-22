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
  .space 800000

.text

    .globl  main
    .ent    main
main:
    addiu   $sp, $sp, -32
    sw  $31, 28($sp)

    # TODO: There are two samples for outputing. Remove them. 
    # Remove

    # read n
    la  $5, n
    
    li $v0, 5
    syscall
    add $4, $v0, $zero
    sw  $4, 0($5)

    # read m
    la  $6, m
    
    li $v0, 5
    syscall
    add $4, $v0, $zero
    sw  $4, 0($6)

    # print n
    la  $6, n
    lw  $4, 0($6)
    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

    # ImgOutput
    # print m
    la  $5, m
    lw  $4, 0($5)
    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

    # print \n
    li $v0, 11
    li $a0, 10
    syscall

    la  $t8, n
    la  $t9, m
    lw  $s0, 0($t8) #n
    lw  $s1, 0($t9) #m

    la $a3, img
    addiu $a3, $a3, 2
    move $a1, $a3
#read and output the first row
    li $s3, 0
$forf:
    li $v0, 5
    syscall
    move $a0, $v0
    sb $a0, 0($a3)
    addiu $a3, $a3, 1
    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall
    addiu $s3, $s3, 1
    bne $s3, $s1, $forf

    li $v0, 11
    li $a0, 10
    syscall
$df:
    addiu $a3, $a3, 1
    andi $s7, $a3, 3
    addi $s7, $s7, -2
    bne $s7, $zero, $df

#read the second row
    move $a2 $a3
    li $s3, 0
$fors:
    li $v0, 5
    syscall
    sb $v0, 0($a3)
    addiu $a3, $a3, 1
    addiu $s3, $s3, 1
    bne $s3, $s1, $fors

$df1:
    addiu $a3, $a3, 1
    andi $s7, $a3, 3
    addi $s7, $s7, -2
    bne $s7, $zero, $df1
#t0 t1 t2
#t3 t4 t5
#t6 t7 v0(t8)

    li $s2, 2
$fori:
    #compute ending address for a2
    add $t9, $a2, $s1
    #the first column
    #read and save 
    lbu $t0, 0($a1)
    addiu $a1, $a1, 1
    lbu $t3, 0($a2)
    addiu $a2, $a2, 1
    li $v0, 5
    syscall 
    sb $v0, 0($a3)
    move $t6, $v0
    addiu $a3, $a3, 1

    move $a0, $t3
    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

    #read the second column
    lbu $t1, 0($a1)
    addiu $a1, $a1, 1
    lbu $t4, 0($a2)
    addiu $a2, $a2, 1
    li $v0, 5
    syscall 
    sb $v0, 0($a3)
    move $t7, $v0
    addiu $a3, $a3, 1
    #prepare
    addiu $s7, $t9, -1
    andi $s6, $s7, 3
    subu $t8, $s7, $s6

    #load t
    addu $t7, $t1, $t7
    sll $t4, $t4,1
    addu $t7, $t7, $t4
    sll $t7, $t7, 16

    addu $s7, $t0, $t6
    sll $t3, $t3, 1
    addu $s7, $s7, $t3

    addu $t7, $t7, $s7
$loop:
    #loop x 4
    #load t
    lw $t0, 0($a1)
    lw $t1, 0($a2)
    addiu $a1 $a1 4
    addiu $a2 $a2 4

    li $v0, 5
    syscall
    move $t2, $v0

    li $v0, 5
    syscall
    sll $s3, $v0, 8
    or $s4, $t2, $s3
    
    li $v0, 5
    syscall
    sll $s3, $v0, 16
    or $s5, $s4, $s3

    li $v0, 5
    syscall
    sll $s3, $v0, 24
    or $t2, $s5, $s3   

    sw $t2, 0($a3)
    addiu $a3 $a3 4

    #t3
    move $t3, $t7
    #t5
    andi $s6, $t0, 255
    andi $s7, $t0, 65280
    sll $s5, $s7, 8
    or $s4, $s5, $s6


    andi $s6, $t1, 255
    andi $s7, $t1, 65280
    sll $t5, $s7, 8
    or $t6, $t5, $s6


    andi $t5, $t2, 255
    andi $s7, $t2, 65280
    sll $t7, $s7, 8
    or $s6, $t7, $t5

    sll $s5 $t6 1
    srl $t4, $t0, 16
    addu $s7 $s4 $s6
    srl $t6, $t1, 16
    addu $t5 $s5 $s7

    #t7
    srl $s3, $t2, 16 

    andi $s6, $t4, 255
    andi $s7, $t4, 65280
    sll $s5, $s7, 8
    or $t4, $s5, $s6

    andi $s6, $t6, 255
    andi $s7, $t6, 65280
    andi $s4, $s3, 255
    sll $t7, $s7, 8
    andi $s7, $s3, 65280
    or $t6, $t7, $s6
    sll $t7, $s7, 8
    or $s6, $t7, $s4
    sll $s5 $t6 1
    addu $s7 $t4 $s6
    srl $s4,$t3,16
    addu $t7 $s5 $s7


    #t4
    sll $s5,$t5,16
    or $t4,$s4,$s5
    #t6
    srl $s4,$t5,16
    sll $s5,$t7,16
    or $t6,$s4,$s5

    #t3 + t4 * 2 + t5
    addu $a0, $t5, $t7
    addu $s4, $t3, $t5
    sll $s5, $t4, 1
    sll $v0, $t6, 1
    addu $s6, $s4, $s5
    addu $s7, $a0, $v0

    #t5 + t6 * 2 + t7

    andi $t4, $s6, 65535
    srl $a0, $t4, 4

    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

    srl $a0, $s6, 20

    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

    andi $t4, $s7, 65535
    srl $a0, $t4, 4

    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

    srl $a0, $s7, 20

    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall


    bne $a2 $t8 $loop

    #finishup work
    #load t
    srl $t0 $t0 16
    srl $t1 $t1 16
    srl $t2 $t2 16

    srl $t7, $t2, 8 
    andi $t6, $t2, 255

    srl $t4, $t1, 8 
    andi $t3, $t1, 255

    srl $t1, $t0, 8 
    andi $t0, $t0, 255

$forfin:
    # load t2 
    lbu $t2, 0($a1)
    addiu $a1, $a1, 1
    # load t5
    lbu $t5, 0($a2)
    addiu $a2, $a2, 1

    # input v0 & save
    li $v0, 5
    syscall
    sb $v0, 0($a3)
    addiu $a3, $a3, 1
    # compute answer store in $a0

    # 1x in s5
    add $s3 $t0 $t2
    add $s4 $t6 $v0
    add $s5 $s3 $s4

    #2x in s6
    add $s3 $t3 $t5
    add $s4 $t1 $t7
    add $s6 $s3 $s4

    #tgother
    sll $s3 $t4 1
    add $s4 $s3 $s6
    sll $s3 $s4 1
    add $a0 $s3 $s5

    # pass on 
    move $t0, $t1
    move $t1, $t2
    move $t3, $t4
    move $t4, $t5
    move $t6, $t7
    move $t7, $v0
    
    # output 
    srl $a0, $a0, 4
    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

    bne $a2, $t9, $forfin

    move $a0, $t4
    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall

$df23:
    addiu $a3, $a3, 1
    andi $s7, $a3, 3
    addi $s7, $s7, -2
    bne $s7, $zero, $df23

$df22:
    addiu $a2, $a2, 1
    andi $s7, $a2, 3
    addi $s7, $s7, -2
    bne $s7, $zero, $df22

$df21:
    addiu $a1, $a1, 1
    andi $s7, $a1, 3
    addi $s7, $s7, -2
    bne $s7, $zero, $df21

    li $v0, 11
    li $a0, 10
    syscall
    addiu $s2, $s2, 1
    bne $s2, $s0, $fori

#output the last row
    li $s3, 0
$forl:
    lbu $a0, 0($a2)
    addiu $a2, $a2, 1
    li $v0, 1
    syscall
    li $v0, 11
    li $a0, 32
    syscall
    addiu $s3, $s3, 1
    bne $s3, $s1, $forl


    li $v0, 11
    li $a0, 10
    syscall

    lw  $31, 28($sp)
    j   $31
    addiu   $sp, $sp, 32
    .end main
