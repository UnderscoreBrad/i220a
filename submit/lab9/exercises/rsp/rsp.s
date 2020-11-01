	.file	"rsp.c"
	.text
	.type	get_rsp, @function
get_rsp:
.LFB3:
	.cfi_startproc
#APP
# 8 "rsp.c" 1
	leaq (%rsp), %rax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE3:
	.size	get_rsp, .-get_rsp
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"&stkVar = %p; rsp = %p\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	call	get_rsp
	leaq	12(%rsp), %rsi
	movq	%rax, %rdx
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
