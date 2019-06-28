#include <arch/ia32/cpu/tables.h>

page_table_entry_t encode_page_table_entry(page_table_entry_info_t info)
{
	page_table_entry_t entry = { 0 };

	entry.bits |= (info.isPresented ? 1 : 0) << 0;	//бит 0 : – страница / таблица присутствует. если этот бит 0, остальные биты элемента система может использовать по своему усмотрению, например, чтобы хранить информацию о том, где физически находится отсутствующая страница
	entry.bits |= (info.isWriteable ? 1 : 0) << 1;	//бит 1 : W страница / таблица доступна для записи
	entry.bits |= (info.isUserAccessible ? 1 : 0) << 2;	//бит 2 : U страница / таблица доступна для программ с CPL = 3
	entry.bits |= (info.isCacheWriteThough ? 1 : 0) << 3;	//бит 3 : PWT бит разрешения сквозной записи
	entry.bits |= (info.isCashingDisabled ? 1 : 0) << 4;	//бит 4 : PCD бит запрещения кэширования
	entry.bits |= (info.wasAccessed ? 1 : 0) << 5;	//бит 5: ј бит доступа(устанавливаетс¤ в 1 при любом обращении к таблице страниц или отдельной странице)
	entry.bits |= (info.wasWritten ? 1 : 0) << 6;	//бит 6 : D грязная страница устанавливsетс¤ в 1 при записи в страницу; всегда равен нулю дл¤ элементов каталога страниц
	entry.bits |= (info.isExpanded ? 1 : 0) << 7;	//бит 7 : PS размер страницы. 1 для страницы размером 2 или 4 мегабайта, иначе 0
	entry.bits |= (info.isGlobal ? 1 : 0) << 8;	//бит 8 : G глобальная страница страница не удаляется из буфера TLB при переключении задач или перезагрузке регистра CR3(только на Pentium Pro, если установлен бит PGE регистра CR4)
	entry.bits |= (info.customBits & 0x07) << 9;
	entry.bits |= (info.physicalAddress & 0xfffff000);

	return entry;
}

page_table_entry_info_t decode_page_table_entry(page_table_entry_t e)
{
	page_table_entry_info_t info;
	info.isPresented = ((e.bits >> 0) & 0x1) != 0;
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

page_directory current_directory __attribute__((aligned(4096)));
page_directory kernel_directory __attribute__((aligned(4096)));

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
		entry.physicalAddress = (uint)&kernel_directory.tables[i];

		kernel_directory.catalog.pages[i] = encode_page_table_entry(entry);

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
			entry.physicalAddress = (i * 1024 + j) * 4096;

			kernel_directory.tables[i].pages[j] = encode_page_table_entry(entry);
		}
	}

	asm("movl %0, %%eax" :: "a"(&kernel_directory.catalog));
	asm("movl %eax, %cr3");

	int n;

	asm("movl %cr0, %eax");
	asm("movl %%eax, %0" : "=r"(n) : );
	n |= 0x80000000;
	asm("movl %0, %%eax" :: "r"(n));
	asm("movl %eax, %cr0");
}


typedef struct free_page_t
{
	int table_index;
	int page_index;
	struct free_page_t* next;
} free_page_t;

//will cashing be broken?
free_page_t* free_pages_head = 0;
int last_table_index = 0;
int last_page_index = 0;

page_directory* create_directory()
{
	page_directory* dir = (page_directory*)palloc(&dir, sizeof(page_directory));

	for (int i = 0; i < 1024; i++)
	{
		page_table_entry_info_t entry;
		entry.isPresented = 0;
		entry.isWriteable = 0;
		entry.isUserAccessible = 0;
		entry.isCacheWriteThough = 1;
		entry.isCashingDisabled = 0;
		entry.wasAccessed = 0;
		entry.wasWritten = 0;
		entry.isExpanded = 0;
		entry.isGlobal = 0;
		entry.customBits = 0;
		entry.physicalAddress = (uint)&(dir->tables[i]);

		kernel_directory.catalog.pages[i] = encode_page_table_entry(entry);

		for (int j = 0; j < 1024; j++)
		{
			page_table_entry_info_t entry;
			entry.isPresented = 0;
			entry.isWriteable = 0;
			entry.isUserAccessible = 0;
			entry.isCacheWriteThough = 1;
			entry.isCashingDisabled = 0;
			entry.wasAccessed = 0;
			entry.wasWritten = 0;
			entry.isExpanded = 0;
			entry.isGlobal = 0;
			entry.customBits = 0;
			entry.physicalAddress = (i * 1024 + j) * 4096;

			dir->tables[i].pages[j] = encode_page_table_entry(entry);
		}
	}

	alloc_page(0, 1);

	return dir;
}

void switch_directory(page_directory* dir)
{
	current_directory = *dir;
	asm volatile("mov %0, %%cr3":: "r"(dir->catalog));
	u32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_table_entry_t alloc_page(int is_kernel, int is_writeable)
{
	page_table_entry_info_t entry;
	entry.isPresented = 1;
	entry.isWriteable = (is_writeable == 1) ? 1 : 0;
	entry.isUserAccessible = (is_kernel == 1) ? 0 : 1;

	page_table_entry_t page;
	if (free_page != 0)
	{
		page = current_directory.tables[free_pages_head->table_index].pages[free_pages_head->page_index];
		free_pages_head = free_pages_head->next;
	}
	else
	{
		if (last_page_index != 1024)
		{
			last_page_index++;
		}
		else if (last_table_index != 1024)
		{
			last_page_index = 0;
			last_table_index++;
		}
		else
		{
			//page fault
		}
		page = current_directory.tables[last_table_index].pages[last_page_index];
	}
	entry = decode_page_table_entry(page);
	entry.isPresented = 1;
	entry.isWriteable = (is_writeable == 1) ? 1 : 0;
	entry.isUserAccessible = (is_kernel == 1) ? 0 : 1;
	page = encode_page_table_entry(entry);
	return page;
}

void free_page(page_table_entry_t* page)
{
	page_table_entry_info_t info = decode_page_table_entry(*page);
	info.isPresented = 0;
	*page = encode_page_table_entry(info);

	free_page_t* new_page;
	new_page->next = free_pages_head;
	new_page->page_index = (info.physicalAddress - (uint)&current_directory) / 4096 / 1024;
	new_page->table_index = (info.physicalAddress - (uint)&current_directory) / 4096 / 1024;

	free_pages_head = new_page;
}

void switch_page_directory(page_directory* dir)
{
	current_directory = *dir;
	asm volatile("mov %0, %%cr3":: "r"(&dir->catalog));
	u32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}
