#include <arch/ia32/vmem.h>

extern byte* _end;

typedef struct dirty_block_t
{
	void* begin_address;
	void* end_address;
	struct dirty_block_t* next;
} dirty_block_t;

typedef struct mem_part_t
{
	void* begin_address;
	void* end_address;
	dirty_block_t* dirty_block;
} mem_part_t;

mem_part_t* memory;

void add_dirty_block(void* begin_address, void* end_address)
{
	dirty_block_t* new_block;
	new_block->begin_address = begin_address;
	new_block->end_address = end_address;
	if (memory->dirty_block == 0)
	{
		new_block->next = 0;
		memory->dirty_block = new_block;
	}
	else
	{
		dirty_block_t* temp = memory->dirty_block;
		dirty_block_t* prev = 0;
		while (temp->begin_address < begin_address && temp->begin_address != 0)
		{
			prev = temp;
			temp = temp->next;
		}
		prev->next = new_block;
		new_block->next = temp;
	}
}

bool remove_dirty_block(void* begin_address)
{
	dirty_block_t* temp = memory->dirty_block;
	dirty_block_t* prev = 0;
	while (temp->begin_address < begin_address && temp->begin_address != 0)
	{
		prev = temp;
		temp = temp->next;
	}
	
	if (temp->begin_address == 0)
	{
		return false;
	}
	
	prev->next = temp->next;
	return true;
}

void take_multiboot_info(struct multiboot_info *info)
{
	if (info->flags >> 6 & 0x01)
	{
		int i = 0;
		void* prev_addr;
		for (multiboot_memory_map_t* entry = (multiboot_memory_map_t*)(void*)info->mmap_addr;
			(int)entry < (info->mmap_addr + info->mmap_length);
			entry = (multiboot_memory_map_t*)((void*)entry + entry->size + 4))
		{
			if (entry->type == 0x01)
			{
				if (i == 0)
				{
					memory->begin_address = entry->addr;
					memory->end_address = entry->addr + entry->len;
				}
				else
				{
					dirty_block_t dirty;
					dirty.begin_address = prev_addr;
					dirty.end_address = entry->addr;
				}
				
				prev_addr = entry->addr + entry->len;
			}
		}
	}
}

void *pinit(struct multiboot_info *multiboot)
{
	take_multiboot_info(multiboot);
}

//TODO: add check _end

void* palloc(void* address, uint size)
{
	dirty_block_t* new_block;

	if (address == 0)
	{
		address = memory->begin_address;
		new_block->begin_address = memory->begin_address;
		new_block->end_address = memory->begin_address + size;
		memory->begin_address += 0x1000;
		new_block->next = memory->dirty_block;
		memory->dirty_block = new_block;
	}
	else if (address >= _end)
	{
		address = 0;
	}
	else
	{
		uint x = size / 0x1000;
		uint r = size % 0x1000;
		if (r > 0)
			size = (x + 1) * 0x1000;
		else
			size = x * 0x1000;

		if (memory->dirty_block != 0)
		{
			dirty_block_t* prev;
			dirty_block_t* temp = memory->dirty_block;
			uint len = temp->begin_address - memory->begin_address;
			while(size  > len && temp != 0)
			{
				prev = temp;
				temp = temp->next;
				len = prev->end_address - temp->begin_address;
			}
			if (size > len)
			{
				add_dirty_block(prev->end_address, prev->end_address + size);
			}
		}
		else
		{
			uint x = (memory->begin_address - address) / 0x1000;
			uint r = (memory->begin_address - address) % 0x1000;
			if (r > 0)
				address = (x + 1) * 0x1000;
			else
				address = x * 0x1000;
			add_dirty_block(address, size);
		}
	}
	return address;
}

void pfree(void* address)
{
	remove_dirty_block(address);
}
