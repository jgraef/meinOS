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