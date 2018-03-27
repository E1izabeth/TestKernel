#include <types.h>

void outportb(u16 port, u8 value)
{
	asm volatile ("outb %b0, %w1" : : "a" (value), "d" (port));
}

byte inportb(u16 port)
{
	byte value;
	asm volatile ("inb %w1, %b0" : "=a" (value) : "d" (port));
	return value;
}


