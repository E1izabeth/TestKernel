#include <types.h>

#ifndef TABLES_H
#define TABLES_H


typedef struct {
	ushort bits;
} RawSelector;

typedef struct {
	ushort index;
	bool useLdt;
	byte rplLevel;
} SelectorInfo;

#pragma pack(push, 1)

typedef union {
	struct { byte b0, b1, b2, b3, b4, b5, b6, b7; } bf;
	byte ba[8];                 
	u64 n;
} RawDescriptor;

typedef struct
{
	ushort limit_low;           // Младшие 16 битов граничного значения limit.
	ushort base_low;            // Младшие 16 битов адресной базы.
	byte  base_middle;         // Следующие 8 битов адресной базы.
	byte  access;              // Флаги доступа, определяющие в каком кольце можно использовать этот сегмент.
	byte  granularity;
	byte  base_high;
}Descriptor;

typedef struct
{
	ushort limit;			// селектор
	uint adr;					// адрес
}gdtPtr;

#pragma pack(pop)

typedef struct
{
	RawDescriptor null_descr;			// нулевой дескриптор
	RawDescriptor code32_descr;		// дескриптор сегмента кода защищЄнного режима
	RawDescriptor data32_descr;		// дескриптор сегмента данных защищЄнного режима
	RawDescriptor code16_descr;		// дескриптор кода реального режима
	RawDescriptor data16_descr;		// дескриптор данных реального режима
} GdtSegment;

typedef struct
{
	//uint present : 1;	//бит 0 : – Ч страница / таблица присутствует.?сли этот бит Ч 0, остальные биты элемента система может использовать по своему усмотрению, например, чтобы хранить информацию о том, где физически находитс¤ отсутствующа¤ страница
	//uint rw : 1;	//бит 1 : W Ч страница / таблица доступна дл¤ записи
	//uint u : 1;	//бит 2 : U Ч страница / таблица доступна дл¤ программ с CPL = 3
	//uint pwt : 1;	//бит 3 : PWT Ч бит разрешени¤ сквозной записи
	//uint psd : 1;	//бит 4 : PCD Ч бит запрещени¤ кэшировани¤
	//uint access : 1;	//бит 5: ј Ч бит доступа(устанавливаетс¤ в 1 при любом обращении к таблице страниц или отдельной странице)
	//uint dirty : 1;	//бит 6 : D Ч Ђгр¤зна¤ страницаї Ч устанавливаетс¤ в 1 при записи в страницу; всегда равен нулю дл¤ элементов каталога страниц
	//uint pd : 1;	//бит 7 : PS Ч размер страницы. 1 Ч дл¤ страницы размером 2 или 4 мегабайта, иначе Ч 0
	//uint global : 1;	//бит 8 : G Ч Ђглобальна¤ страницаї Ч страница не удал¤етс¤ из буфера TLB при переключении задач или перезагрузке регистра CR3(только на Pentium Pro, если установлен бит PGE регистра CR4)

	byte info;//011101010
	byte system_used;// : 3;доступны для использовани¤ операционной системой
	uint frame; //: 20
} PageTableEntryInfo;

typedef struct
{
	int bits;
} PageTableEntry;

typedef struct
{
	PageTableEntry pages[1024];
} PageTable;

typedef struct
{
	PageTable tables[1024];
}PageDirectory;

#pragma pack(push, 1)

typedef struct
{
	ushort base_1;
	ushort sel;
	byte  always0;
	byte  flags;
	ushort base_2;           
}idtDescriptor;

typedef struct
{
	ushort limit;
	uint adr;
}idtPtr;

typedef struct
{
	uint base;
	uint count;
}isr_t;

#pragma pack(pop)


typedef struct
{
	u32 ds;                 
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; 
	u32 int_no, err_code;    
	u32 eip, cs, eflags, useresp, ss; 
}registers;

SelectorInfo makeSelector(ushort index, bool useLdt, byte rplLevel);
RawSelector compileSelector(SelectorInfo info);
SelectorInfo parseSelector(RawSelector s);
void outportb(u16 port, u8 value);

extern void idt_flush(u32);
extern void gdt_flush(u32);
extern void isr0(u32);
extern void isr1(u32);
extern void isr2(u32);
extern void isr3(u32);
extern void isr4(u32);
extern void isr5(u32);
extern void isr6(u32);
extern void isr7(u32);
extern void isr8(u32);
extern void isr9(u32);
extern void isr10(u32);
extern void isr11(u32);
extern void isr12(u32);
extern void isr13(u32);
extern void isr14(u32);
extern void isr15(u32);
extern void isr16(u32);
extern void isr17(u32);
extern void isr18(u32);
extern void isr19(u32);
extern void isr20(u32);
extern void isr21(u32);
extern void isr22(u32);
extern void isr23(u32);
extern void isr24(u32);
extern void isr25(u32);
extern void isr26(u32);
extern void isr27(u32);
extern void isr28(u32);
extern void isr29(u32);
extern void isr30(u32);
extern void isr31(u32);
extern void irq0(u32);
extern void irq1(u32);
extern void irq2(u32);
extern void irq3(u32);
extern void irq4(u32);
extern void irq5(u32);
extern void irq6(u32);
extern void irq7(u32);
extern void irq8(u32);
extern void irq9(u32);
extern void irq10(u32);
extern void irq11(u32);
extern void irq12(u32);
extern void irq13(u32);
extern void irq14(u32);
extern void irq15(u32);
#endif
