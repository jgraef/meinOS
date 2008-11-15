/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

global isr00,isr01,isr02,isr03,isr04,isr05,isr06,isr07,isr08,isr09,isr0A,isr0B,isr0C,isr0D,isr0E,isr0F
global isr10,isr11,isr12,isr13,isr14,isr15,isr16,isr17,isr18,isr19,isr1A,isr1B,isr1C,isr1D,isr1E,isr1F
global isr20,isr21,isr22,isr23,isr24,isr25,isr26,isr27,isr28,isr29,isr2A,isr2B,isr2C,isr2D,isr2E,isr2F
global isr30,isr31,isr32,isr33,isr34,isr35,isr36,isr37

global isr_uselapic
global isr

extern syscall_handler
extern interrupt_handler
extern lapic_timer_calibrate

align 4

reg_eax       dd 0
reg_eip       dd 0
reg_esp       dd 0
reg_efl       dd 0
reg_cs        dw 0
reg_ds        dw 0
reg_ss        dw 0
reg_kernelesp dd 0
isr_uselapic  dd 0

; Pointer to ISRs as array
isr       dd isr_table
isr_table dd isr00,isr01,isr02,isr03,isr04,isr05,isr06,isr07,isr08,isr09,isr0A,isr0B,isr0C,isr0D,isr0E,isr0F, \
             isr10,isr11,isr12,isr13,isr14,isr15,isr16,isr17,isr18,isr19,isr1A,isr1B,isr1C,isr1D,isr1E,isr1F, \
             isr20,isr21,isr22,isr23,isr24,isr25,isr26,isr27,isr28,isr29,isr2A,isr2B,isr2C,isr2D,isr2E,isr2F, \
             isr30,isr31,isr32,isr33,isr34,isr35,isr36,isr37

%macro pushsr 0
	push DWORD ds
	push DWORD es
	push DWORD fs
	push DWORD gs
%endmacro

%macro popsr 0
	pop DWORD ds
	pop DWORD es
	pop DWORD fs
	pop DWORD gs
%endmacro

; Interrupt Service Routines
isr00:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x00
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr01:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x01
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr02:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x02
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr03:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x03
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr04:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x04
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr05:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x05
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr06:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x06
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr07:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x07
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr08:
	pushsr
	pushad
	push esp
	push DWORD 0x08
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr09:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x09
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr0A:
	pushsr
	pushad
	push esp
	push DWORD 0x0A
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr0B:
	pushsr
	pushad
	push esp
	push DWORD 0x0B
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr0C:
	pushsr
	pushad
	push esp
	push DWORD 0x0C
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr0D:
	pushsr
	pushad
	push esp
	push DWORD 0x0D
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr0E:
	pushsr
	pushad
	push esp
	push DWORD 0x0E
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr0F: ; Spurious
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x0F
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr10:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x10
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr11:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x11
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr12:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x12
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr13:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x13
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr14:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x14
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr15:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x15
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr16:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x16
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr17:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x17
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr18:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x18
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr19:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x19
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr1A:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x1A
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr1B:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x1B
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr1C:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x1C
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr1D:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x1D
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr1E:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x1E
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr1F:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x1F
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr20: ; PIT timer interrupt (for calibration of APIC timer)
	pushad
	mov eax,[isr_uselapic]
	test eax,eax
	jz .usepic
	call lapic_timer_calibrate
	popad
	iret
.usepic:
	popad
	jmp isr30
isr21:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x21
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr22:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x22
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr23:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x23
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr24:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x24
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr25:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x25
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr26:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x26
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr27:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x27
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr28:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x28
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr29:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x29
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr2A:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x2A
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr2B:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x2B
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr2C:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x2C
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr2D:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x2D
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr2E:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x2E
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr2F:
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x2F
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr30: ; LAPIC Timer
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x30
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr31: ; Thermal
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x31
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr32: ; Performance monitor
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x32
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr33: ; LINT0
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x33
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr34: ; LINT1
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x34
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr35: ; Error
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x35
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr36: ; IPCI
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	push esp
	push DWORD 0x36
	call interrupt_handler
	add esp,8
	popad
	popsr
	add esp,4
	iret
isr37: ; Syscall interrupt
	push DWORD 0 ; dummy error code
	pushsr
	pushad
	mov ax,ds
	push ax
	mov ax,ss
	mov ds,ax
	pop ax
	mov [reg_ds],ax

	; call syscallhandler
	push esp
	call syscall_handler
	add esp,4 ; pop esp

	popad
	popsr
	add esp,4 ; pop error code

;	mov edx,esp
;	add edx,44
;	mov eax,[edx]
;	push eax
;	call syscall_handler
;	add esp,9*4

	; restore ds
	mov dx,[reg_ds]
	mov ds,dx
	iret
