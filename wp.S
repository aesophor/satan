.global satan_asm_cr0_wp_disable
.global satan_asm_cr0_wp_enable

.text
satan_asm_cr0_wp_disable:
	push %rax
	mov  %cr0, %rax
	and  (~0x10000), %rax
	mov  %rax, %cr0
	pop  %rax
	ret

satan_asm_cr0_wp_enable:
	push %rax
	mov  %cr0, %rax
	or   0x10000, %rax
	mov  %rax, %cr0
	pop  %rax
	ret
