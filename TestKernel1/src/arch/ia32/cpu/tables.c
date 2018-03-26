#include <arch/ia32/cpu/tables.h>

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
