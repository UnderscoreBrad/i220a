	.file	"rip.c"
	.text
	.type	get_rip, @function
get_rip:
.LFB3:
	.cfi_startproc
#APP
# 8 "rip.c" 1
	leaq (%rip), %rax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE3:
	.size	get_rip, .-get_rip
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"&get_rip() = %p; rip = %p\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	get_rip
	movq	%rax, %rdx
	leaq	get_rip(%rip), %rsi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
