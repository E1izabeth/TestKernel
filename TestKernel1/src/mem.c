#include <types.h>

void* memcpy(void* to, void* from, int length)
{
	byte* dst = to;
	byte* src = from;
	byte* end = to + length;

	while (dst < end)
		*(dst++) = *(src++);

	return dst;
}

void* memset(void* to, byte value, int length)
{
	byte* dst = to;
	byte* end = to + length;

	while (dst < end)
		*(dst++) = value;

	return dst;
}

char *utoa(uint i)
{
	char buf[12];
	char *ptr = buf + sizeof(buf) - 1;
	unsigned int u = i;

	*ptr = '\0';
	do
		*--ptr = '0' + (u % 10);
	while (u /= 10);

	return ptr;
}

char *itoa(int i)
{
	char buf[12];
	char *ptr = buf + sizeof(buf) - 1;
	unsigned int u;
	bool minus = 0;

	if (i < 0)
	{
		minus = 1;
		u = -i; // ((unsigned int)(-(1 + i))) + 1;
	}
	else
		u = i;

	*ptr = '\0';
	do
		*--ptr = '0' + (u % 10);
	while (u /= 10);

	if (minus)
		*--ptr = '-';


	return ptr;
}

int atoi(char* s)
{
	int n = 0;
	while (*s >= '0' && *s <= '9') {
		n *= 10;
		n += *s++;
		n -= '0';
	}
	return n;
}
