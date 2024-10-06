.intel_syntax noprefix
	.text
	.section .rodata
even: .string  "Number is even."
odd: .string  "Number is odd."
 
	.text
	.globl my_asm_func
my_asm_func:
	push rbp
	mov rbp, rsp
 
	xor eax, eax
	mov rbx, rdi
	test bl, 1
	jnz is_odd
	lea rdi, even[rip]
	jmp end
is_odd:
	lea rdi, odd[rip]
end:
	call printf
	leave
	ret