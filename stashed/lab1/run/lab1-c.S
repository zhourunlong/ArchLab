	.file	1 "lab1-c.c"
	.data
	.globl	n
	.align	2
n:
	.space	4
	.globl	m
	.align	2
m:
	.space	4
	.globl	img
	.align	2
img:
	.space	524288
	.text
	.align	2
	.globl	_printf_num
	.set	nomips16
	.set	nomicromips
	.ent	_printf_num
_printf_num:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$4,8($fp)
	lw	$3,8($fp)
#APP
 # 6 "mystdio.h" 1
	li $v0, 1 
	add $a0, $3, $zero 
	syscall 
	
 # 0 "" 2
#NO_APP
	.set	noreorder
	nop
	.set	reorder
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	j	$31
	.end	_printf_num
	.align	2
	.globl	_scanf_num
	.set	nomips16
	.set	nomicromips
	.ent	_scanf_num
_scanf_num:
	.frame	$fp,24,$31		# vars= 8, regs= 1/0, args= 0, gp= 8
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-24
	sw	$fp,20($sp)
	move	$fp,$sp
#APP
 # 17 "mystdio.h" 1
	li $v0, 5 
	syscall 
	add $3, $v0, $zero 
	
 # 0 "" 2
#NO_APP
	sw	$3,8($fp)
	lw	$2,8($fp)
	move	$sp,$fp
	lw	$fp,20($sp)
	addiu	$sp,$sp,24
	j	$31
	.end	_scanf_num
	.align	2
	.globl	_printf_char
	.set	nomips16
	.set	nomicromips
	.ent	_printf_char
_printf_char:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	move	$2,$4
	sb	$2,8($fp)
	lbu	$3,8($fp)
#APP
 # 28 "mystdio.h" 1
	li $v0, 11 
	add $a0, $3, $zero 
	syscall 
	
 # 0 "" 2
#NO_APP
	.set	noreorder
	nop
	.set	reorder
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	j	$31
	.end	_printf_char
	.align	2
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
main:
	.frame	$fp,48,$31		# vars= 16, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$fp,40($sp)
	move	$fp,$sp
	sw	$0,24($fp)
	sw	$0,28($fp)
	jal	_scanf_num
	move	$3,$2
	la	$2,n
	sw	$3,0($2)
	jal	_scanf_num
	move	$3,$2
	la	$2,m
	sw	$3,0($2)
	sw	$0,24($fp)
	b	$L6
$L9:
	sw	$0,28($fp)
	b	$L7
$L8:
	jal	_scanf_num
	andi	$3,$2,0xffff
	la	$4,img
	lw	$2,24($fp)
	sll	$5,$2,9
	lw	$2,28($fp)
	addu	$2,$5,$2
	sll	$2,$2,1
	addu	$2,$4,$2
	sh	$3,0($2)
	lw	$2,28($fp)
	addiu	$2,$2,1
	sw	$2,28($fp)
$L7:
	lw	$3,28($fp)
	la	$2,m
	lw	$2,0($2)
	sltu	$2,$3,$2
	bne	$2,$0,$L8
	lw	$2,24($fp)
	addiu	$2,$2,1
	sw	$2,24($fp)
$L6:
	lw	$3,24($fp)
	la	$2,n
	lw	$2,0($2)
	sltu	$2,$3,$2
	bne	$2,$0,$L9
	la	$2,n
	lw	$2,0($2)
	move	$4,$2
	jal	_printf_num
	li	$4,32			# 0x20
	jal	_printf_char
	la	$2,m
	lw	$2,0($2)
	move	$4,$2
	jal	_printf_num
	li	$4,10			# 0xa
	jal	_printf_char
	sh	$0,32($fp)
	sw	$0,28($fp)
	b	$L10
$L11:
	la	$3,img
	lw	$2,28($fp)
	sll	$2,$2,1
	addu	$2,$3,$2
	lhu	$2,0($2)
	move	$4,$2
	jal	_printf_num
	li	$4,32			# 0x20
	jal	_printf_char
	lw	$2,28($fp)
	addiu	$2,$2,1
	sw	$2,28($fp)
$L10:
	lw	$3,28($fp)
	la	$2,m
	lw	$2,0($2)
	sltu	$2,$3,$2
	bne	$2,$0,$L11
	li	$4,10			# 0xa
	jal	_printf_char
	li	$2,1			# 0x1
	sw	$2,24($fp)
	b	$L12
$L16:
	la	$3,img
	lw	$2,24($fp)
	sll	$2,$2,10
	addu	$2,$3,$2
	lhu	$2,0($2)
	move	$4,$2
	jal	_printf_num
	li	$4,32			# 0x20
	jal	_printf_char
	li	$2,1			# 0x1
	sw	$2,28($fp)
	b	$L13
$L15:
	lw	$2,24($fp)
	addiu	$4,$2,-1
	lw	$2,28($fp)
	addiu	$2,$2,-1
	la	$3,img
	sll	$4,$4,9
	addu	$2,$4,$2
	sll	$2,$2,1
	addu	$2,$3,$2
	lhu	$2,0($2)
	move	$5,$2
	lw	$2,24($fp)
	addiu	$4,$2,-1
	lw	$2,28($fp)
	addiu	$2,$2,1
	la	$3,img
	sll	$4,$4,9
	addu	$2,$4,$2
	sll	$2,$2,1
	addu	$2,$3,$2
	lhu	$2,0($2)
	addu	$2,$5,$2
	lw	$3,24($fp)
	addiu	$5,$3,1
	lw	$3,28($fp)
	addiu	$3,$3,-1
	la	$4,img
	sll	$5,$5,9
	addu	$3,$5,$3
	sll	$3,$3,1
	addu	$3,$4,$3
	lhu	$3,0($3)
	addu	$2,$2,$3
	lw	$3,24($fp)
	addiu	$5,$3,1
	lw	$3,28($fp)
	addiu	$3,$3,1
	la	$4,img
	sll	$5,$5,9
	addu	$3,$5,$3
	sll	$3,$3,1
	addu	$3,$4,$3
	lhu	$3,0($3)
	addu	$3,$2,$3
	lw	$2,24($fp)
	addiu	$2,$2,-1
	la	$4,img
	sll	$5,$2,9
	lw	$2,28($fp)
	addu	$2,$5,$2
	sll	$2,$2,1
	addu	$2,$4,$2
	lhu	$2,0($2)
	move	$6,$2
	lw	$2,28($fp)
	addiu	$2,$2,-1
	la	$4,img
	lw	$5,24($fp)
	sll	$5,$5,9
	addu	$2,$5,$2
	sll	$2,$2,1
	addu	$2,$4,$2
	lhu	$2,0($2)
	addu	$2,$6,$2
	lw	$4,28($fp)
	addiu	$4,$4,1
	la	$5,img
	lw	$6,24($fp)
	sll	$6,$6,9
	addu	$4,$6,$4
	sll	$4,$4,1
	addu	$4,$5,$4
	lhu	$4,0($4)
	addu	$2,$2,$4
	lw	$4,24($fp)
	addiu	$4,$4,1
	la	$5,img
	sll	$6,$4,9
	lw	$4,28($fp)
	addu	$4,$6,$4
	sll	$4,$4,1
	addu	$4,$5,$4
	lhu	$4,0($4)
	addu	$2,$2,$4
	sll	$2,$2,1
	addu	$3,$3,$2
	la	$4,img
	lw	$2,24($fp)
	sll	$5,$2,9
	lw	$2,28($fp)
	addu	$2,$5,$2
	sll	$2,$2,1
	addu	$2,$4,$2
	lhu	$2,0($2)
	sll	$2,$2,2
	addu	$2,$3,$2
	bgez	$2,$L14
	addiu	$2,$2,15
$L14:
	sra	$2,$2,4
	sh	$2,32($fp)
	lhu	$2,32($fp)
	move	$4,$2
	jal	_printf_num
	li	$4,32			# 0x20
	jal	_printf_char
	lw	$2,28($fp)
	addiu	$2,$2,1
	sw	$2,28($fp)
$L13:
	la	$2,m
	lw	$2,0($2)
	addiu	$3,$2,-1
	lw	$2,28($fp)
	sltu	$2,$2,$3
	bne	$2,$0,$L15
	la	$2,m
	lw	$2,0($2)
	addiu	$2,$2,-1
	la	$3,img
	lw	$4,24($fp)
	sll	$4,$4,9
	addu	$2,$4,$2
	sll	$2,$2,1
	addu	$2,$3,$2
	lhu	$2,0($2)
	move	$4,$2
	jal	_printf_num
	li	$4,32			# 0x20
	jal	_printf_char
	li	$4,10			# 0xa
	jal	_printf_char
	lw	$2,24($fp)
	addiu	$2,$2,1
	sw	$2,24($fp)
$L12:
	la	$2,n
	lw	$2,0($2)
	addiu	$3,$2,-1
	lw	$2,24($fp)
	sltu	$2,$2,$3
	bne	$2,$0,$L16
	sw	$0,28($fp)
	b	$L17
$L18:
	la	$2,n
	lw	$2,0($2)
	addiu	$2,$2,-1
	la	$3,img
	sll	$4,$2,9
	lw	$2,28($fp)
	addu	$2,$4,$2
	sll	$2,$2,1
	addu	$2,$3,$2
	lhu	$2,0($2)
	move	$4,$2
	jal	_printf_num
	li	$4,32			# 0x20
	jal	_printf_char
	lw	$2,28($fp)
	addiu	$2,$2,1
	sw	$2,28($fp)
$L17:
	lw	$3,28($fp)
	la	$2,m
	lw	$2,0($2)
	sltu	$2,$3,$2
	bne	$2,$0,$L18
	li	$4,10			# 0xa
	jal	_printf_char
	move	$2,$0
	move	$sp,$fp
	lw	$31,44($sp)
	lw	$fp,40($sp)
	addiu	$sp,$sp,48
	j	$31
	.end	main
