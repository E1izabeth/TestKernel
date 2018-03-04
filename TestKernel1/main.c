#include <cpu/tables.h>
#include <cpu/multiboot.h>
#define VIDEO_HEIGHT 25   //������ ������
#define VIDEO_WIDTH 80    //������ ������
#define VIDEO_RAM 0xb8000 //����� �����������

int tty_cursor;    //��������� �������
int tty_attribute; //������� �������� �������


void init_tty()
{
	tty_cursor = 0;
	tty_attribute = 7;
}

//����� �������� ��������� �������
void textcolor(char c)
{
	tty_attribute = c;
}

//������� ������
void clear()
{
	char *video = (char*)VIDEO_RAM;
	int i;

	for (i = 0; i < VIDEO_HEIGHT*VIDEO_WIDTH; i++) {
		*(video + i * 2) = ' ';
	}

	tty_cursor = 0;
}


//����� ������ ������� � ������ ���������
void putchar(char c)
{
	char *video = (char*)VIDEO_RAM;
	int i;

	switch (c) {
	case '\n': //���� ��� ������ ����� ������
		tty_cursor += VIDEO_WIDTH;
		tty_cursor -= tty_cursor % VIDEO_WIDTH;
		break;

	default:
		*(video + tty_cursor * 2) = c;
		*(video + tty_cursor * 2 + 1) = tty_attribute;
		tty_cursor++;
		break;
	}

	//���� ������ ����� �� ������� ������, ������� ����� ����� �� ���� ������
	if (tty_cursor>VIDEO_WIDTH*VIDEO_HEIGHT) {
		for (i = VIDEO_WIDTH * 2; i <= VIDEO_WIDTH * VIDEO_HEIGHT * 2 + VIDEO_WIDTH * 2; i++) {
			*(video + i - VIDEO_WIDTH * 2) = *(video + i);
		}
		tty_cursor -= VIDEO_WIDTH;
	}
}

//����� ������, ��������������� ����-��������
void puts(const char *s)
{
	while (*s) {
		putchar(*s);
		s++;
	}
}


char *itoa(int i)
{
	char buf[12];
	char *ptr = buf + sizeof(buf) - 1;
	unsigned int u;
	bool minus = 0;

	if (i < 0) {
		minus = 1;
		u = ((unsigned int)(-(1 + i))) + 1;
	}
	else
		u = i;
	*ptr = '\0';//0
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

void isr_handler(registers regs)
{
	puts("recieved interrupt: ");
	puts(itoa(regs.int_no));
	puts("\n");
}

void irq_handler(registers regs)
{
	puts("recieved interrupt: ");
	puts(itoa(regs.int_no));
	puts("\n");
	// �������� ������ EOI (���������� ����������) � ���������� PIC.
	// ���� � ������������� ���������� ��������� ����������� ����������.
	if (regs.int_no >= 40)
	{
		// Send reset signal to slave.
		outportb(0xA0, 0x20);
	}
	// �������� ������ ������������ � ������� ���������� (� ����� � ����������� ����������, ���� ��� ����������).
	outportb(0x20, 0x20);
//TODO:
}

extern gdtPtr init_gdt();
extern idtPtr init_idt();

idtPtr idt;
gdtPtr gdt;

void kernel_main(struct multiboot_info multiboot)
{
	int x = sizeof(gdtPtr);
	puts(itoa(x));

	init_tty();
	clear();
	gdt = init_gdt();
	idt = init_idt();
	
	
	puts("Hello world from my custom kernel! ;)\n");
	//for (;;);
	asm("int3");
	
	puts("Hello world from my custom kernel! ;)\n");
	puts(itoa(5));


	for (;;)
		;//	keyboard(regs);
	
	asm("hlt");
}





