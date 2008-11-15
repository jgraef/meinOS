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

#ifndef _ISR_H_
#define _ISR_H_

#define ISR_NUM 0x38

void isr00();
void isr01();
void isr02();
void isr03();
void isr04();
void isr05();
void isr06();
void isr07();
void isr08();
void isr09();
void isr0A();
void isr0B();
void isr0C();
void isr0D();
void isr0E();
void isr0F();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr1A();
void isr1B();
void isr1C();
void isr1D();
void isr1E();
void isr1F();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr2A();
void isr2B();
void isr2C();
void isr2D();
void isr2E();
void isr2F();
void isr30();
void isr31();
void isr32();
void isr33();
void isr34();
void isr35();
void isr36();
void isr37();

void **isr;

int isr_uselapic;

#endif
