#include <types.h>
#include <arch/ia32/cpu/tables.h>


descriptor_t compileDescriptor(int base, int limit, byte access, byte gran)
{
	descriptor_t d = {};
	d.base_low = (base & 0xFFFF);
	d.base_middle = (base >> 16) & 0xFF;
	d.base_high = (base >> 24) & 0xFF;

	d.limit_low = (limit & 0xFFFF);
	d.granularity = (limit >> 16) & 0x0F;

	d.granularity |= gran & 0xF0;
	d.access = access;
	return d;
}

raw_descriptor_t makeDescriptor(descriptor_t info)
{
	raw_descriptor_t d = { 0 };
	d.bf.b7 = info.base_high & 0xff;
	d.bf.b6 = info.granularity & 0xff;
	d.bf.b5 = info.access;
	d.bf.b4 = info.base_middle & 0xff;
	d.bf.b3 = (info.base_low >> 8) & 0xff;
	d.bf.b2 = (info.base_low >> 0) & 0xff;
	d.bf.b1 = (info.limit_low >> 8) & 0xff;
	d.bf.b0 = (info.limit_low >> 0) & 0xff;
	return d;
}

descriptor_t parseDescriptor(raw_descriptor_t s)
{
	descriptor_t info;
	info.base_high = s.bf.b7 & 0xff;
	info.granularity = s.bf.b6 & 0xf0;
	info.access = s.bf.b5;
	info.base_middle = s.bf.b4 & 0xff;
	info.base_low = (s.bf.b3 << 8) & 0xff;
	info.base_low |= (s.bf.b2 << 0) & 0xff;
	info.limit_low = (s.bf.b1 << 8) & 0xff;
	info.limit_low |= (s.bf.b0 << 0) & 0xff;
	return info;
}

gdt_segment_t seg;

gdt_ptr_t init_gdt()
{
	gdt_ptr_t ptr;
	ptr.limit = (sizeof(descriptor_t) * 5 - 1);
	ptr.adr = (uint)&seg;

	//Descriptor descr = { 0xabcd, 0xdef0, 1, 2, 3, 4 };

	seg.null_descr = makeDescriptor(compileDescriptor(0, 0, 0, 0));
	seg.code16_descr = makeDescriptor(compileDescriptor(0, 0xFFFFFFFF, 0x9A, 0xCF));//9A  CF
	seg.data16_descr = makeDescriptor(compileDescriptor(0, 0xFFFFFFFF, 0x92, 0xCF));//92 CF
	seg.code32_descr = makeDescriptor(compileDescriptor(0, 0xFFFFFFFF, 0xFA, 0xCF));//FA CF
	seg.data32_descr = makeDescriptor(compileDescriptor(0, 0xFFFFFFFF, 0xF2, 0xCF));//F2 CF
	

	raw_selector_t codeSelector = compileSelector(makeSelector(3, false, 0));
	raw_selector_t dataSelector = compileSelector(makeSelector(4, false, 0));

	asm("lgdt 0(,%0,)"::"a"(&ptr));
	asm("movw %0, %%ax" :: "r"(dataSelector));
	asm("movw %ax, %ds");
	asm("movw %ax, %es");
	asm("movw %ax, %gs");
	asm("movw %ax, %fs");
	asm("movw %ax, %ss");
	asm("movw %0, %%ax" :: "r"(codeSelector));
	asm("pushl %eax");
	asm("movl $continue_pm, %eax");
	asm("pushl %eax");
	asm("retf");
	asm("continue_pm:");
	
	//gdt_flush((u32)&ptr);

	return ptr;
}

selector_info_t makeSelector(ushort index, bool useLdt, byte rplLevel)
{
	selector_info_t info = { index, useLdt, rplLevel };
	return info;
}

raw_selector_t compileSelector(selector_info_t info)
{
	raw_selector_t s = { 0 };
	s.bits |= (info.index << 3) & 0xfff8; // bits 15-3
	s.bits |= ((info.useLdt ? 1 : 0) << 2) & 0x0040; // bit 2
	s.bits |= (info.rplLevel & 0x0003); // bits 1-0
	return s;
}

selector_info_t parseSelector(raw_selector_t s)
{
	selector_info_t info;
	info.index = (s.bits & 0xfff8) >> 3;
	info.useLdt = (s.bits & 0x0040) != 0;
	info.rplLevel = (s.bits & 0x0003);
	return info;
}
