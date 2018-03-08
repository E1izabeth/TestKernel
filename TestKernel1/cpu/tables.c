#include <types.h>
#include <cpu/tables.h>

void outportb(u16 port, u8 value)
{
	asm volatile ("outb %b0, %w1" : : "a" (value), "d" (port));
}

byte inportb(u16 port)
{
	byte value;
	asm volatile ("inb %w1, %b0" : "=a" (value): "d" (port) );
	return value;
}

//?
page_table_entry_t encodePageTableEntry(page_table_entry_info_t info)
{
	page_table_entry_t entry;
	entry.bits |= info.info & 0xEA;
	entry.bits |= (info.system_used << 8) & 0x07;
	entry.bits |= (info.frame << 11) & 0xFF;
	return entry;
}

//?
page_table_entry_info_t decodePageTableEntry(page_table_entry_t e)
{
	page_table_entry_info_t info;
	info.info = e.bits & 0xEA;
	info.system_used = (e.bits & 0x07) >> 8;
	info.frame = (e.bits & 0xFF) >> 11;
}

void setPageTableEntry(page_table_entry_t* table, int index, page_table_entry_info_t info)
{
	table[index] = encodePageTableEntry(info);
}

page_table_entry_info_t getPageTableEntry(page_table_entry_t* table, int index)
{
	return decodePageTableEntry(table[index]);
}
