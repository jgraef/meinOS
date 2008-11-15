;    meinOS - A unix-like x86 microkernel operating system
;    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.

global _start
global init_stack
global init_stacksize
extern main

; setting up the Multiboot header - see GRUB docs for details
; Flags:
;  ModuleAlign: false
;  MemoryMap:   true
;  VideoMode:   true
;
MST_FLAGS    equ  0000000000000000b
OPT_FLAGS    equ  0000000000000110b
FLAGS        equ  (MST_FLAGS<<16)|OPT_FLAGS
MAGIC        equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM     equ -(MAGIC + FLAGS)        ; checksum required
; Video Info
VIDEOMODE    equ 0   ; Textmode
VIDEOWIDTH   equ 80  ; 80 Cols
VIDEOHEIGHT  equ 25  ; 25 Lines
VIDEODEPTH   equ 0   ; Depth
; reserve initial kernel stack space
STACKSIZE    equ 4*1024

section .text
align 4

MultiBootHeader:
	dd MAGIC       ; Magic number
	dd FLAGS       ; Flags
	dd CHECKSUM    ; Checksum
	times 5 dd 0   ; unused fields
	dd VIDEOMODE   ; Videomode
	dd VIDEOWIDTH  ; Videowidth
	dd VIDEOHEIGHT ; Videoheight
	dd VIDEODEPTH  ; Videodepth

_start:
	mov esp, init_stack+STACKSIZE-4 ; set up the stack

	push eax ; pass Multiboot magic number
	push ebx ; pass Multiboot info structure
	call main ; call kernel function
	add esp,8

	cli ; halt machine should kernel return
	hlt

init_stacksize dd STACKSIZE

section .bss
align 4

init_stack:
	resb STACKSIZE
