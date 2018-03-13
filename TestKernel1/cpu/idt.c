#include <types.h>
#include <cpu/tables.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

void* memcpy(void* to, void* from, int length)
{
	byte* dst = to;
	byte* src = from;
	byte* end = to + length;

	while (dst < end)
		*(dst++) = *(src++);

	return dst;
}

void* memset(void* to, byte value, int length)
{
	byte* dst = to;
	byte* end = to + length;

	while (dst < end)
		*(dst++) = value;

	return dst;
}

#pragma pack(push, 1)

typedef struct {
	byte opCliCode; // 0xFA
	byte opPush1Code; // 0x6A 
	byte opPush1Arg; 
	byte opPush2Code; // 0x6A
	byte opPush2Arg;
	byte opJmpCode; // 0xE9
	uint opJmpArg; // (&opNop-target)
	byte opNop[6]; // 0x90
} raw_custom_handler_instance_t;

#pragma pack(pop)

extern void irq_common_stub(uint);
extern void isr_common_stub(uint);

void makeCustomHandler(raw_custom_handler_instance_t* h, byte n)
{
	h->opCliCode = 0xFA;
	h->opPush1Code = 0x6A;
	h->opPush1Arg = 0;
	h->opPush2Code = 0x6A;
	h->opPush2Arg = n;
	h->opJmpCode = 0xE9;
	h->opJmpArg = ((byte*)&irq_common_stub - (byte*)h->opNop);
	memset(h->opNop, 0x90, 6);
}


#define handlersCount 0x30
raw_custom_handler_instance_t custom_handlers[handlersCount];
idt_descriptor_t idt_seg[handlersCount];

void idt_set(byte num, uint base, ushort sel, byte flags)
{
	idt_seg[num].base_1 = base & 0xFFFF;
	idt_seg[num].base_2 = (base >> 16) & 0xFFFF;
	idt_seg[num].sel = sel;
	idt_seg[num].always0 = 0;
	idt_seg[num].flags = flags;
}

void PIC_remap(byte offset1, byte offset2)
{
	byte a1, a2;

	a1 = inportb(PIC1_DATA);                        // save masks
	a2 = inportb(PIC2_DATA);

	outportb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outportb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
	
	outportb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outportb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	
	outportb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outportb(PIC2_DATA, 2);                   // ICW3: tell Slave PIC its cascade identity (0000 0010)

	outportb(PIC1_DATA, ICW4_8086);
	outportb(PIC2_DATA, ICW4_8086);

	outportb(PIC1_DATA, 0xFF); //disable all IRQs
	outportb(PIC1_DATA, a1);   // restore saved masks.
	outportb(PIC2_DATA, a2);
}

idt_ptr_t init_idt()
{
	idt_ptr_t ptr;

	ptr.limit = sizeof(idt_descriptor_t) * handlersCount - 1;
	ptr.adr = (uint)&idt_seg;

		
	
	for (uint i = 0; i < handlersCount; i++)
	{		
		makeCustomHandler(&(custom_handlers[i]), i);
		idt_set(i, (u32)&(custom_handlers[i]), compileSelector(makeSelector(3, false, 0)).bits, 0x8E);
	}

	PIC_remap(0x20, 0x28); //0x20 0x28

	idt_flush((u32)&ptr);
	return ptr;
}