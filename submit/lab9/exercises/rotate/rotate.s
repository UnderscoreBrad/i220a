	.file	"rotate.c"
	.text
	.globl	f
	.type	f, @function
f:
.LFB2:
	.cfi_startproc
	movl	%edx, %ecx
	roll	%cl, %edi
	rorl	%cl, %esi
	leal	(%rdi,%rsi), %eax
	ret
	.cfi_endproc
.LFE2:
	.size	f, .-f
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
