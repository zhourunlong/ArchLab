	.file	1 "simple_add.c"
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
	nop
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
	nop
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
	nop
	.end	_printf_char
	.align	2
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
main:
	.frame	$fp,40,$31		# vars= 8, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	reorder
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	jal	_scanf_num
	nop
	sw	$2,24($fp)
	jal _scanf_num
	nop
	sw	$2,28($fp)
	lw	$3,24($fp)
	lw	$2,28($fp)
	addu	$2,$3,$2
	move	$4,$2
	jal	_printf_num
	nop
	li	$4,10			# 0xa
	jal	_printf_char
	nop
	move	$2,$0
	move	$sp,$fp
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addiu	$sp,$sp,40
  li $v0, 0xa
  syscall
	j	$31
	nop
	.end	main
