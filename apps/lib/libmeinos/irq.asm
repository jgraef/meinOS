global _irq_handler_stub
extern _irq_handler

section .data

irq dd 0

section .code

_irq_handler_stub:
	add esp,4
	pop DWORD[irq]
	sub esp,8

	pushad

	mov eax,[irq]
	push eax
	call _irq_handler
	add esp,4

	popad

	ret