	.text
	.globl	_mysort
	.def	_mysort;	.scl	2;	.type	32;	.endef
_mysort:
LFB14:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$36, %esp
	.cfi_offset 3, -12
	movl	%esp, %eax
	movl	%eax, %ebx
	movl	$0, -20(%ebp)
	movl	8(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, -24(%ebp)
	sall	$2, %eax
	leal	3(%eax), %edx
	movl	$16, %eax
	subl	$1, %eax
	addl	%edx, %eax
	movl	$16, %ecx
	movl	$0, %edx
	divl	%ecx
	imull	$16, %eax, %eax
	call	___chkstk_ms
	subl	%eax, %esp
	movl	%esp, %eax
	addl	$3, %eax
	shrl	$2, %eax
	sall	$2, %eax
	movl	%eax, -28(%ebp)
	movl	$0, -12(%ebp)
	jmp	L2
L3:
	movl	-12(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %ecx
	movl	-28(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	%ecx, (%eax,%edx,4)
	addl	$1, -12(%ebp)
L2:
	movl	-12(%ebp), %eax
	cmpl	8(%ebp), %eax
	jl	L3
	movl	$0, -12(%ebp)
	jmp	L4
L8:
	movl	$0, -16(%ebp)
	jmp	L5
L7:
	movl	-28(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %edx
	movl	-16(%ebp), %eax
	leal	1(%eax), %ecx
	movl	-28(%ebp), %eax
	movl	(%eax,%ecx,4), %eax
	cmpl	%eax, %edx
	jle	L6
	movl	-28(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	(%eax,%edx,4), %eax
	movl	%eax, -32(%ebp)
	movl	-16(%ebp), %eax
	leal	1(%eax), %edx
	movl	-28(%ebp), %eax
	movl	(%eax,%edx,4), %ecx
	movl	-28(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	%ecx, (%eax,%edx,4)
	movl	-16(%ebp), %eax
	leal	1(%eax), %ecx
	movl	-28(%ebp), %eax
	movl	-32(%ebp), %edx
	movl	%edx, (%eax,%ecx,4)
L6:
	addl	$1, -16(%ebp)
L5:
	movl	8(%ebp), %eax
	subl	-12(%ebp), %eax
	subl	$1, %eax
	cmpl	-16(%ebp), %eax
	jg	L7
	addl	$1, -12(%ebp)
L4:
	movl	-12(%ebp), %eax
	cmpl	8(%ebp), %eax
	jl	L8
	movl	$0, -12(%ebp)
	jmp	L9
L11:
	movl	-28(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	(%eax,%edx,4), %edx
	movl	-12(%ebp), %eax
	leal	0(,%eax,4), %ecx
	movl	12(%ebp), %eax
	addl	%ecx, %eax
	movl	(%eax), %eax
	cmpl	%eax, %edx
	je	L10
	addl	$1, -20(%ebp)
L10:
	addl	$1, -12(%ebp)
L9:
	movl	-12(%ebp), %eax
	cmpl	8(%ebp), %eax
	jl	L11
	movl	-20(%ebp), %eax
	movl	%ebx, %esp
	movl	-4(%ebp), %ebx
	leave
	.cfi_restore 5
	.cfi_restore 3
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
