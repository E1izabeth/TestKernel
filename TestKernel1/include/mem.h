#include <types.h>

#ifndef MEM_H
#define MEM_H

void* memset(void* to, byte value, int length);
void* memcpy(void* to, void* from, int length);
char *utoa(uint i);
char *itoa(int i);
int atoi(char* s);

#endif
