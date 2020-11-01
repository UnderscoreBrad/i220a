	.text
	.globl get_parity
#edi contains n	
get_parity:
	testl	%edi, %edi
	jpe	even
	mov	$0x0, %eax
	ret
even:
	mov	$0x1, %eax	
	#@TODO: add code here to set eax to 1 iff edi has even parity
	ret
	
