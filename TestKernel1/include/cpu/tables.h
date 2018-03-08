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
	ushort limit_low;           // Младшие 16 битов граничного значения limit.
	ushort base_low;            // Младшие 16 битов адресной базы.
	byte  base_middle;         // Следующие 8 битов адресной базы.
	byte  access;              // Флаги доступа, определяющие в каком кольце можно использовать этот сегмент.
	byte  granularity;
	byte  base_high;
}descriptor_t;

typedef struct
{
	ushort limit;			// селектор
	uint adr;					// адрес
}gdt_ptr_t;

#pragma pack(pop)

typedef struct
{
	raw_descriptor_t null_descr;			// нулевой дескриптор
	raw_descriptor_t code32_descr;		// дескриптор сегмента кода защищЄнного режима
	raw_descriptor_t data32_descr;		// дескриптор сегмента данных защищЄнного режима
	raw_descriptor_t code16_descr;		// дескриптор кода реального режима
	raw_descriptor_t data16_descr;		// дескриптор данных реального режима
} gdt_segment_t;

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
