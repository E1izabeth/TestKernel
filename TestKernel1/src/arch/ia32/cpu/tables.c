#include <arch/ia32/cpu/tables.h>

page_table_entry_t encode_page_table_entry(page_table_entry_info_t info)
{
	page_table_entry_t entry = { 0 };

	entry.bits |= (info.isPresented ? 1 : 0) << 0;	//бит 0 : – Ч страница / таблица присутствует.?сли этот бит Ч 0, остальные биты элемента система может использовать по своему усмотрению, например, чтобы хранить информацию о том, где физически находитс¤ отсутствующа¤ страница
	entry.bits |= (info.isWriteable ? 1 : 0) << 1;	//бит 1 : W Ч страница / таблица доступна дл¤ записи
	entry.bits |= (info.isUserAccessible ? 1 : 0) << 2;	//бит 2 : U Ч страница / таблица доступна дл¤ программ с CPL = 3
	entry.bits |= (info.isCacheWriteThough ? 1 : 0) << 3;	//бит 3 : PWT Ч бит разрешени¤ сквозной записи
	entry.bits |= (info.isCashingDisabled ? 1 : 0) << 4;	//бит 4 : PCD Ч бит запрещени¤ кэшировани¤
	entry.bits |= (info.wasAccessed ? 1 : 0) << 5;	//бит 5: ј Ч бит доступа(устанавливаетс¤ в 1 при любом обращении к таблице страниц или отдельной странице)
	entry.bits |= (info.wasWritten ? 1 : 0) << 6;	//бит 6 : D Ч Ђгр¤зна¤ страницаї Ч устанавливsетс¤ в 1 при записи в страницу; всегда равен нулю дл¤ элементов каталога страниц
	entry.bits |= (info.isExpanded ? 1 : 0) << 7;	//бит 7 : PS Ч размер страницы. 1 Ч дл¤ страницы размером 2 или 4 мегабайта, иначе Ч 0
	entry.bits |= (info.isGlobal ? 1 : 0) << 8;	//бит 8 : G Ч Ђглобальна¤ страницаї Ч страница не удал¤етс¤ из буфера TLB при переключении задач или перезагрузке регистра CR3(только на Pentium Pro, если установлен бит PGE регистра CR4)
	entry.bits |= (info.customBits & 0x07) << 9;
	entry.bits |= (info.physicalAddress & 0xfffff000);

	return entry;
}

page_table_entry_info_t decode_page_table_entry(page_table_entry_t e)
{
	page_table_entry_info_t info;
	info.isPresented = ((e.bits >> 0) & 0x1) !=0;
	info.isWriteable = ((e.bits >> 1) & 0x1) != 0;
	info.isUserAccessible = ((e.bits >> 2) & 0x1) != 0;
	info.isCacheWriteThough = ((e.bits >> 3) & 0x1) != 0;
	info.isCashingDisabled = ((e.bits >> 4) & 0x1) != 0;
	info.wasAccessed = ((e.bits >> 5) & 0x1) != 0;
	info.wasWritten = ((e.bits >> 6) & 0x1) != 0;
	info.isExpanded = ((e.bits >> 7) & 0x1) != 0;
	info.isGlobal = ((e.bits >> 8) & 0x1) != 0;
	info.customBits = (e.bits >> 9) & 0x7;
	info.physicalAddress = (e.bits & 0xfffff000);

	return info;
}

void set_page_table_entry(page_table_entry_t* table, int index, page_table_entry_info_t info)
{
	table[index] = encode_page_table_entry(info);
}

page_table_entry_info_t get_page_table_entry(page_table_entry_t* table, int index)
{
	return decode_page_table_entry(table[index]);
}

page_directory directory __attribute__((aligned(4096)));

void init_directory()
{
	for (int i = 0; i < 1024; i++)
	{
		page_table_entry_info_t entry;
		entry.isPresented = 1;
		entry.isWriteable = 1;
		entry.isUserAccessible = 0;
		entry.isCacheWriteThough = 1;
		entry.isCashingDisabled = 0;
		entry.wasAccessed = 0;
		entry.wasWritten = 0;
		entry.isExpanded = 0;
		entry.isGlobal = 0;
		entry.customBits = 0;
		entry.physicalAddress = (uint)&directory.tables[i];
		
		directory.catalog.pages[i] = encode_page_table_entry(entry);

		for (int j = 0; j < 1024; j++)
		{
			page_table_entry_info_t entry;
			entry.isPresented = 1;
			entry.isWriteable = 1;
			entry.isUserAccessible = 0;
			entry.isCacheWriteThough = 1;
			entry.isCashingDisabled = 0;
			entry.wasAccessed = 0;
			entry.wasWritten = 0;
			entry.isExpanded = 0;
			entry.isGlobal = 0;
			entry.customBits = 0;
			entry.physicalAddress = i * 1024 * 4096 + j * 4096;

			directory.tables[i].pages[j] = encode_page_table_entry(entry);
		}
	}

	asm("movl %0, %%eax" :: "a"(&directory.catalog));
	asm("movl %eax, %cr3");
	//asm("orl 0x80000000, %cr0");

	/*
	asm("movl %cr0, %eax");
	asm("orl 0x80000000, %eax");
	asm("movl %eax, %cr0");
	*/
	
	int n;

	asm("movl %cr0, %eax");
	asm("movl %%eax, %0" : "=r"(n) :);
	n |= 0x80000000;
	asm("movl %0, %%eax" :: "r"(n));
	asm("movl %eax, %cr0");
}
