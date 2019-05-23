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
	ushort limit_low;           // Младшие 16 битов граничного значения limit.
	ushort base_low;            // Младшие 16 битов адресной базы.
	byte  base_middle;         // Следующие 8 битов адресной базы.
	byte  access;              // Флаги доступа, определяющие в каком кольце можно использовать этот сегмент.
	byte  granularity;
	byte  base_high;
} descriptor_t;

typedef struct {
	ushort limit;			// селектор
	uint adr;					// адрес
} gdt_ptr_t;

typedef struct
{
	raw_descriptor_t null_descr;			// нулевой дескриптор
	raw_descriptor_t code32_descr;		// дескриптор сегмента кода защищЄнного режима
	raw_descriptor_t data32_descr;		// дескриптор сегмента данных защищЄнного режима
	raw_descriptor_t code16_descr;		// дескриптор кода реального режима
	raw_descriptor_t data16_descr;		// дескриптор данных реального режима
} gdt_segment_t;

#pragma pack(pop)

typedef struct
{
	bool isPresented;	//бит 0 : – Ч страница / таблица присутствует.?сли этот бит Ч 0, остальные биты элемента система может использовать по своему усмотрению, например, чтобы хранить информацию о том, где физически находитс¤ отсутствующа¤ страница
	bool isWriteable;	//бит 1 : W Ч страница / таблица доступна дл¤ записи
	bool isUserAccessible;	//бит 2 : U Ч страница / таблица доступна дл¤ программ с CPL = 3
	bool isCacheWriteThough;	//бит 3 : PWT Ч бит разрешени¤ сквозной записи
	bool isCashingDisabled;	//бит 4 : PCD Ч бит запрещени¤ кэшировани¤
	bool wasAccessed;	//бит 5: ј Ч бит доступа(устанавливаетс¤ в 1 при любом обращении к таблице страниц или отдельной странице)
	bool wasWritten;	//бит 6 : D Ч Ђгр¤зна¤ страницаї Ч устанавливsетс¤ в 1 при записи в страницу; всегда равен нулю дл¤ элементов каталога страниц
	bool isExpanded;	//бит 7 : PS Ч размер страницы. 1 Ч дл¤ страницы размером 2 или 4 мегабайта, иначе Ч 0
	bool isGlobal;	//бит 8 : G Ч Ђглобальна¤ страницаї Ч страница не удал¤етс¤ из буфера TLB при переключении задач или перезагрузке регистра CR3(только на Pentium Pro, если установлен бит PGE регистра CR4)

	// byte info;//011101010
	byte customBits;// : 3;доступны для использовани¤ операционной системой
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
