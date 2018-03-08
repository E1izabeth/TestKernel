#include <types.h>

#ifndef TABLES_H
#define TABLES_H


typedef struct {
	ushort bits;
} raw_selector_t;

typedef struct {
	ushort index;
	bool useLdt;
	byte rplLevel;
} selector_info_t;

#pragma pack(push, 1)

typedef union {
	struct { byte b0, b1, b2, b3, b4, b5, b6, b7; } bf;
	byte ba[8];                 
	u64 n;
} raw_descriptor_t;

typedef struct
{
	ushort limit_low;           // ������� 16 ����� ���������� �������� limit.
	ushort base_low;            // ������� 16 ����� �������� ����.
	byte  base_middle;         // ��������� 8 ����� �������� ����.
	byte  access;              // ����� �������, ������������ � ����� ������ ����� ������������ ���� �������.
	byte  granularity;
	byte  base_high;
}descriptor_t;

typedef struct
{
	ushort limit;			// ��������
	uint adr;					// �����
}gdt_ptr_t;

#pragma pack(pop)

typedef struct
{
	raw_descriptor_t null_descr;			// ������� ����������
	raw_descriptor_t code32_descr;		// ���������� �������� ���� ����������� ������
	raw_descriptor_t data32_descr;		// ���������� �������� ������ ����������� ������
	raw_descriptor_t code16_descr;		// ���������� ���� ��������� ������
	raw_descriptor_t data16_descr;		// ���������� ������ ��������� ������
} gdt_segment_t;

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
} page_table_entry_info_t;

typedef struct
{
	int bits;
} page_table_entry_t;

typedef struct
{
	page_table_entry_t pages[1024];
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
}idt_descriptor_t;

typedef struct
{
	ushort limit;
	uint adr;
}idt_ptr_t;

#pragma pack(pop)


typedef struct
{
	u32 ds;                 
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; 
	u32 int_no, err_code;    
	u32 eip, cs, eflags, useresp, ss; 
}registers_t;

selector_info_t makeSelector(ushort index, bool useLdt, byte rplLevel);
raw_selector_t compileSelector(selector_info_t info);
selector_info_t parseSelector(raw_selector_t s);
void outportb(u16 port, u8 value);
byte inportb(u16 port);

extern void idt_flush(u32);
extern void gdt_flush(u32);
#endif
