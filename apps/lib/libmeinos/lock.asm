global lock_p,lock_v

lock_p:
	add esp,4
	pop edx
	sub esp,8

	mov eax,1
.spin:
	xchg dword ptr [edx],eax
	pause
	test eax,eax
	jnz .spin
	ret

lock_v:
	mov eax,0
	xchg dword ptr [edx],eax
	ret
