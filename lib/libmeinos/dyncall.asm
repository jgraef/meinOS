;    meinOS - A unix-like x86 microkernel operating system
;    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU Lesser General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU Lesser General Public License for more details.
;
;    You should have received a copy of the GNU Lesser General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.

global dyn_call,dyn_int

dyn_call:
	push ebp
	push ebx
	mov ebp,esp

	; get function arguments
	mov eax,[ebp+12] ; function pointer
	mov ebx,[ebp+16] ; parameters
	mov edx,[ebp+20] ; num parameters

	; point ebx to last parameter
	push eax
	push edx
	mov eax,4
	mul edx
	add ebx,eax
	pop edx
	pop eax

	; push parameters on stack
	or edx,edx
	jz .lpe
	mov ecx,edx
.lpb:
	sub ebx,4
	push DWORD [ebx]
	loop .lpb
.lpe:

	; Call function
	call eax

	; return
	mov esp,ebp
	pop ebx
	pop ebp
	ret

section .data

dyn_int:
	push ebp
	push ebx
	mov ebp,esp

	; get function arguments
	mov eax,[ebp+12] ; function pointer
	mov ebx,[ebp+16] ; parameters
	mov edx,[ebp+20] ; num parameters

	; point ebx to last parameter
	push eax
	push edx
	mov eax,4
	mul edx
	add ebx,eax
	pop edx
	pop eax

	; push parameters on stack
	or edx,edx
	jz .lpe
	mov ecx,edx
.lpb:
	sub ebx,4
	push DWORD [ebx]
	loop .lpb
.lpe:

	; Call function
	mov [.interrupt+1],al
.interrupt:
	int 42

	; return
	mov esp,ebp
	pop ebx
	pop ebp
	ret
