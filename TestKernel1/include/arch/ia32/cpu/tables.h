#include <types.h>

#ifndef TABLES_H
#define TABLES_H

#pragma pack(push, 1)

typedef struct {
	ushort bits;
} raw_selector_t;

typedef struct {
	// Segment index 0x0000..0x1fff
	ushort index;
	// GDT/LDT switch
	bool useLdt;
	// Priviledges level 0..3
	byte rplLevel;
} selector_info_t;

typedef union {
	// bytes as struct
	struct { byte b0, b1, b2, b3, b4, b5, b6, b7; } bf;
	// bytes as array
	byte ba[8];                 
	// bytes as ulong
	u64 n;
} raw_descriptor_t;

typedef struct {
	ushort limit_low;           // ������� 16 ����� ���������� �������� limit.
	ushort base_low;            // ������� 16 ����� �������� ����.
	byte  base_middle;         // ��������� 8 ����� �������� ����.
	byte  access;              // ����� �������, ������������ � ����� ������ ����� ������������ ���� �������.
	byte  granularity;
	byte  base_high;
} descriptor_t;

typedef struct {
	ushort limit;			// ��������
	uint adr;					// �����
} gdt_ptr_t;

typedef struct
{
	raw_descriptor_t null_descr;			// ������� ����������
	raw_descriptor_t code32_descr;		// ���������� �������� ���� ����������� ������
	raw_descriptor_t data32_descr;		// ���������� �������� ������ ����������� ������
	raw_descriptor_t code16_descr;		// ���������� ���� ��������� ������
	raw_descriptor_t data16_descr;		// ���������� ������ ��������� ������
} gdt_segment_t;

#pragma pack(pop)

typedef struct
{
	bool isPresented;	//��� 0 : � � �������� / ������� ������������.?��� ���� ��� � 0, ��������� ���� �������� ������� ����� ������������ �� ������ ����������, ��������, ����� ������� ���������� � ���, ��� ��������� �������� ������������ ��������
	bool isWriteable;	//��� 1 : W � �������� / ������� �������� �� ������
	bool isUserAccessible;	//��� 2 : U � �������� / ������� �������� �� �������� � CPL = 3
	bool isCacheWriteThough;	//��� 3 : PWT � ��� ��������� �������� ������
	bool isCashingDisabled;	//��� 4 : PCD � ��� ��������� ����������
	bool wasAccessed;	//��� 5: � � ��� �������(�������������� � 1 ��� ����� ��������� � ������� ������� ��� ��������� ��������)
	bool wasWritten;	//��� 6 : D � ������ �������� � ����������s��� � 1 ��� ������ � ��������; ������ ����� ���� �� ��������� �������� �������
	bool isExpanded;	//��� 7 : PS � ������ ��������. 1 � �� �������� �������� 2 ��� 4 ���������, ����� � 0
	bool isGlobal;	//��� 8 : G � ���������� �������� � �������� �� ������� �� ������ TLB ��� ������������ ����� ��� ������������ �������� CR3(������ �� Pentium Pro, ���� ���������� ��� PGE �������� CR4)

	// byte info;//011101010
	byte customBits;// : 3;�������� ��� ������������ ������������ ��������
	uint physicalAddress; //: 20
} page_table_entry_info_t;

#pragma pack(push, 1)

typedef struct
{
	int bits;
} page_table_entry_t;

typedef struct
{
	page_table_entry_t pages[1024];
} page_table;

typedef struct
{
	page_table catalog;
	page_table tables[1024];
} page_directory;

typedef struct
{
	ushort base_1;
	ushort sel;
	byte  always0;
	byte  flags;
	ushort base_2;           
} idt_descriptor_t;

typedef struct
{
	ushort limit;
	uint adr;
}idt_ptr_t;

typedef struct
{
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
} registers_generic_t;

typedef struct
{
	u32 ds;           
	registers_generic_t gen_regs;
	u32 int_no, err_code;    
	u32 eip, cs, eflags; 
} registers_t;

#pragma pack(pop)

selector_info_t make_selector(ushort index, bool useLdt, byte rplLevel);
raw_selector_t compile_selector(selector_info_t info);
selector_info_t parse_selector(raw_selector_t s);

extern void idt_flush(u32);
extern void gdt_flush(u32);

void init_directory();

void set_page_table_entry(page_table_entry_t* table, int index, page_table_entry_info_t info);
page_table_entry_info_t get_page_table_entry(page_table_entry_t* table, int index);
page_table_entry_t encode_page_table_entry(page_table_entry_info_t info);
page_table_entry_info_t decode_page_table_entry(page_table_entry_t e);

#endif
