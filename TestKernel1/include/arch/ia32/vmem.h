#ifndef VMEM_H
#define VMEM_H

#include <types.h>
#include <arch/ia32/multiboot/multiboot.h>

#define VALLOC_COMMIT 0x01
#define VALLOC_RESERVE 0x02

// void* valloc(void* address, uint size, uint flags);
// void vfree(void* address);

void *pinit(struct multiboot_info *multiboot);
void* palloc(void* address, uint size);
void pfree(void* address);

#endif
