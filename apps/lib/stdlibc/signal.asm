global _signal_handler_stub
extern _signal_handler

section .data

sig dd 0

section .code

_signal_handler_stub:
	add esp,4
	pop DWORD [sig]
	sub esp,8

	pushad

	mov eax,[sig]
	push eax
	call _signal_handler
	add esp,4

	popad

	ret
