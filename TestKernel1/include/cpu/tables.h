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
	ushort limit_low;           // ������� 16 ����� ���������� �������� limit.
	ushort base_low;            // ������� 16 ����� �������� ����.
	byte  base_middle;         // ��������� 8 ����� �������� ����.
	byte  access;              // ����� �������, ������������ � ����� ������ ����� ������������ ���� �������.
	byte  granularity;
	byte  base_high;
}Descriptor;

typedef struct
{
	ushort limit;			// ��������
	uint adr;					// �����
}gdtPtr;

#pragma pack(pop)

typedef struct
{
	RawDescriptor null_descr;			// ������� ����������
	RawDescriptor code32_descr;		// ���������� �������� ���� ����������� ������
	RawDescriptor data32_descr;		// ���������� �������� ������ ����������� ������
	RawDescriptor code16_descr;		// ���������� ���� ��������� ������
	RawDescriptor data16_descr;		// ���������� ������ ��������� ������
} GdtSegment;

typedef struct
{
	//uint present : 1;	//��� 0 : � � �������� / ������� ������������.?��� ���� ��� � 0, ��������� ���� �������� ������� ����� ������������ �� ������ ����������, ��������, ����� ������� ���������� � ���, ��� ��������� �������� ������������ ��������
	//uint rw : 1;	//��� 1 : W � �������� / ������� �������� �� ������
	//uint u : 1;	//��� 2 : U � �������� / ������� �������� �� �������� � CPL = 3
	//uint pwt : 1;	//��� 3 : PWT � ��� ��������� �������� ������
	//uint psd : 1;	//��� 4 : PCD � ��� ��������� ����������
	//uint access : 1;	//��� 5: � � ��� �������(�������������� � 1 ��� ����� ��������� � ������� ������� ��� ��������� ��������)
	//uint dirty : 1;	//��� 6 : D � ������ �������� � �������������� � 1 ��� ������ � ��������; ������ ����� ���� �� ��������� �������� �������
	//uint pd : 1;	//��� 7 : PS � ������ ��������. 1 � �� �������� �������� 2 ��� 4 ���������, ����� � 0
	//uint global : 1;	//��� 8 : G � ���������� �������� � �������� �� ������� �� ������ TLB ��� ������������ ����� ��� ������������ �������� CR3(������ �� Pentium Pro, ���� ���������� ��� PGE �������� CR4)

	byte info;//011101010
	byte system_used;// : 3;�������� ��� ������������ ������������ ��������
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
