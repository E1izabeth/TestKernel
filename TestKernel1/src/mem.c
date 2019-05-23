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

char *utoa(uint i, char* buff, int buffLen)
{
	memset(buff, 0, buffLen);
	unsigned int u = i;

	*buff = '\0';
	--buffLen;
	do {
		*--buff = '0' + (u % 10);
		--buffLen;
		u /= 10;
	} while (u > 0 && buffLen > 0);

	return buff;
}

char* itoa(int i, char* buff, int buffLen)
{
	memset(buff, 0, buffLen);
	unsigned int u;
	bool minus = 0;

	if (i < 0)
	{
		minus = 1;
		u = -i; // ((unsigned int)(-(1 + i))) + 1;
	}
	else
		u = i;

	buff += buffLen - 1;

	*buff = '\0';
	--buffLen;
	do {
		*--buff = '0' + (u % 10);
		--buffLen;
		u /= 10;
	} while (u > 0 && buffLen > 0);

	if (minus)
		*--buff = '-';

	return buff;
}

int atoi(char* s, int n)
{
	n = 0;
	while (*s >= '0' && *s <= '9') {
		n *= 10;
		n += *s++;
		n -= '0';
	}
	return n;
}

static char _hexMap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

char* utox(uint n, char* buff, int buffLength)
{
	memset(buff, 0, buffLength);

	for (int i = 0, j = 0; i < 32 && j < buffLength; i += 4, j++)
	{
		uint q = (n >> (28 - i)) & 0x0f;
		buff[j] = _hexMap[q];
	}

	return buff;
}

char* ultox(ulong n, char* buff, int buffLength)
{
	memset(buff, 0, buffLength);

	for (int i = 0, j = 0; i < 64 && j < buffLength; i += 4, j++)
	{
		uint q = (n >> (60 - i)) & 0x0f;
			buff[j] = _hexMap[q];
	}

	return buff;
}
