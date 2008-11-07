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
