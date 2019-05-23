#include <types.h>

#ifndef MEM_H
#define MEM_H

void* memset(void* to, byte value, int length);
void* memcpy(void* to, void* from, int length);
char *utoa(uint i, char* buff, int buffLength);
char *utox(uint i, char* buff, int buffLength);
char *itoa(int i, char* buff, int buffLength);
char *ultox(ulong i, char* buff, int buffLength);
int atoi(char* s, int n);

#endif
