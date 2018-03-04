#include <types.h>
#include <cpu/tables.h>

idtDescriptor idt_seg[256];

void idt_set(byte num, uint base, ushort sel, byte flags)
{
	idt_seg[num].base_1 = base & 0xFFFF;
	idt_seg[num].base_2 = (base >> 16) & 0xFFFF;
	idt_seg[num].sel = sel;
	idt_seg[num].always0 = 0;
	idt_seg[num].flags = flags;
}

idtPtr init_idt()
{
	idtPtr ptr;

	ptr.limit = sizeof(idtDescriptor) * 32 - 1;
	ptr.adr = (uint)&idt_seg;

	/*for (uint i = 0; i < 32; i++)
	{
	idt_set(i, (u32)isr, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	}*/

	// Remap the irq table.
	outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
	outportb(0x21, 0x20);
	outportb(0xA1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
	outportb(0x21, 0x0);
	outportb(0xA1, 0x0);

	idt_set(0, (uint)isr0, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(1, (uint)isr1, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(2, (uint)isr2, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(3, (uint)isr3, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(4, (uint)isr4, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(5, (uint)isr5, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(6, (uint)isr6, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(7, (uint)isr7, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(8, (uint)isr8, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(9, (uint)isr9, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(10, (uint)isr10, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(11, (uint)isr11, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(12, (uint)isr12, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(13, (uint)isr13, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(14, (uint)isr14, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(15, (uint)isr15, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(16, (uint)isr16, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(17, (uint)isr17, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(18, (uint)isr18, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(19, (uint)isr19, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(20, (uint)isr20, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(21, (uint)isr21, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(22, (uint)isr22, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(23, (uint)isr23, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(24, (uint)isr24, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(25, (uint)isr25, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(26, (uint)isr26, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(27, (uint)isr27, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(28, (uint)isr28, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(29, (uint)isr29, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(30, (uint)isr30, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	idt_set(31, (uint)isr31, compileSelector(makeSelector(3, false, 0)).bits, 0x8E);

	idt_flush((u32)&ptr);
	return ptr;
}