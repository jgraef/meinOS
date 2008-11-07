global lapic_timer_init,lapic_timer_calibrate

LAPIC_ADDRESS           equ 0xFEE00000
LAPIC_REG_TIMER_CURRENT equ 0x390

apicregs dd LAPIC_ADDRESS
apiczaehler dd LAPIC_ADDRESS+LAPIC_REG_TIMER_CURRENT

lapic_timer_init:
	xor ebp,ebp								;EBP nullen, damit nachher in PIT-ISR erkenbar, wie oft der Timer-Interrupt aufgerufen wurde

	mov al,00110110b							;den PIT auf den gewünschten Wert (hier 10ms) einstellen
	out 43h,al

	mov ax,11931
	out 40h,al
	mov al,ah
	out 40h,al

	sti									;IRQs freigeben
timerschleife:									;Solange Warten, bis das Einschalten und Messen des APIC Timers sicher fertig ist
	cmp ebp,0x3								;Die Zählvariable hat dann den Wert 3
	jne timerschleife
	cli

	mov eax,[apicregs]							;Den zuvor ermittelten Platz der APIC Register laden
	mov ecx,DWORD[apiczaehler]						;Den Gemessenen Wert für den Initial Count des APIC Timers nach ECX laden

	mov ebp,[eax+0x3e0]							;Alle Register des APIC Timers neu schreiben, um den neuen Initial Count einzustellen
	mov DWORD[eax+0x3e0],0xb

	mov ebp,[eax+0x320]							;Zuerst irgendein Register lesen, wegen Bug in manchem APICS
	mov DWORD[eax+0x320],0x20031

	mov ebp,[eax+0x380]							;Nach Lesen eines Wertes, den neuen Wert des Zählers aus ECX ins Initial Count Register schreiben
	mov [eax+0x380],ecx


lapic_timer_calibrate:
	inc ebp									;Variable um zwischen Einschalten und Messen (des Current Count) des APIC Timers zu unterscheiden
	mov eax,[apicregs]							;Den zuvor ermittelten Platz der APIC Register laden

	cmp ebp,0x1
	jne ApicSchonGestartet


	mov edx,[eax+0x3e0]							;Zuerst irgendein Wert auslesen, dannach Divide Configuration Register auf Divide Value = 1 setzen
	mov DWORD[eax+0x3e0],0xb

	mov edx,[eax+0x320]							;Zuerst Wert auslesen, dannach LVT-Timer Register setzen (peridischer Timer auf Vektor 31)
	mov DWORD[eax+0x320],0x20031

	mov edx,[eax+0x380]							;Initial Count Register auf das Maximum setzen --> möglichst viel Werte zum Kalibrieren möglich
	mov DWORD[eax+0x380],0xFFFFFFFF

	jmp schlussKalib


ApicSchonGestartet:
	cmp ebp,0x3								;Wenn der Timer schon zum 3. Mal ausgelöst, muss nichts mehr getan werden
	je schlussKalib

										;Wenn Timer schon gestartet wurde, dann wird jetzt das Current Count Register gemessen
	mov ecx,0xFFFFFFFF							;Unterschied des Current Count in der Zeit des PITs ausrechnen
	sub ecx,[eax+0x390]
	mov DWORD[apiczaehler],ecx

schlussKalib:
	mov al,20h								;EOI senden an den PIC, da dieser verwendet wurde zum Kalibrieren
       	out 20h,al

	ret