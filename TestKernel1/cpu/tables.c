#include <types.h>
#include <cpu/tables.h>

void outportb(u16 port, u8 value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

//?
PageTableEntry encodePageTableEntry(PageTableEntryInfo info)
{
	PageTableEntry entry;
	entry.bits |= info.info & 0xEA;
	entry.bits |= (info.system_used << 8) & 0x07;
	entry.bits |= (info.frame << 11) & 0xFF;
	return entry;
}

//?
PageTableEntryInfo decodePageTableEntry(PageTableEntry e)
{
	PageTableEntryInfo info;
	info.info = e.bits & 0xEA;
	info.system_used = (e.bits & 0x07) >> 8;
	info.frame = (e.bits & 0xFF) >> 11;
}

void setPageTableEntry(PageTableEntry* table, int index, PageTableEntryInfo info)
{
	table[index] = encodePageTableEntry(info);
}

PageTableEntryInfo getPageTableEntry(PageTableEntry* table, int index)
{
	return decodePageTableEntry(table[index]);
}
