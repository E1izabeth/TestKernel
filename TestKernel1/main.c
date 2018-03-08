#include <cpu/tables.h>
#include <cpu/multiboot.h>
#define VIDEO_HEIGHT 25   //высота экрана
#define VIDEO_WIDTH 80    //ширина экрана
#define VIDEO_RAM 0xb8000 //адрес видеопамяти

int tty_cursor;    //положение курсора
int tty_attribute; //текущий аттрибут символа


void init_tty()
{
	tty_cursor = 0;
	tty_attribute = 7;
}

//Смена текущего аттрибута символа
void textcolor(char c)
{
	tty_attribute = c;
}

//Очистка экрана
void clear()
{
	char *video = (char*)VIDEO_RAM;
	int i;

	for (i = 0; i < VIDEO_HEIGHT*VIDEO_WIDTH; i++) 
	{
		*(video + i * 2) = ' ';
	}

	tty_cursor = 0;
}


//Вывод одного символа в режиме телетайпа
void putchar(char c)
{
	char *video = (char*)VIDEO_RAM;
	int i;

	switch (c) {
	case '\n': //Если это символ новой строки
		tty_cursor += VIDEO_WIDTH;
		tty_cursor -= tty_cursor % VIDEO_WIDTH;
		break;

	default:
		*(video + tty_cursor * 2) = c;
		*(video + tty_cursor * 2 + 1) = tty_attribute;
		tty_cursor++;
		break;
	}

	//Если курсор вышел за границу экрана, сдвинем экран вверх на одну строку
	if (tty_cursor > VIDEO_WIDTH*VIDEO_HEIGHT) 
	{
		for (i = VIDEO_WIDTH * 2; i <= VIDEO_WIDTH * VIDEO_HEIGHT * 2 + VIDEO_WIDTH * 2; i++) 
		{
			*(video + i - VIDEO_WIDTH * 2) = *(video + i);
		}
		tty_cursor -= VIDEO_WIDTH;
	}
}

//Вывод строки, заканчивающейся нуль-символом
void puts(const char *s)
{
	int i = 0;
	while (s[i])
	{
		putchar(s[i++]);
	}
	/*while (*s) {
		putchar(*s);
		*++s;
	}*/
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

//void isr_handler(registers regs)
//{
//	puts("recieved interrupt: ");
//	puts(itoa(regs.int_no));
//	puts("\n");
//}

void irq_handler(registers_t regs)
{
	if (regs.int_no != 32 && regs.int_no != 33)
	{
		puts("recieved interrupt: ");
		puts(itoa(regs.int_no));
		puts("\n");
	}
	
	// Посылает сигнал EOI (завершение прерывания) в устройства PIC.
	// Если к возникновению прерывания причастно подчиненное устройство.
	if (regs.int_no >= 40)
	{
		// Send reset signal to slave.
		outportb(0xA0, 0x20);
	}
	// Посылает сигнал перезагрузки в главное устройство (а также в подчиненное устройство, если это необходимо).
	outportb(0x20, 0x20);
	
	if (regs.int_no == 33)
	{
		byte x = inportb(0x60);
		if (x < 128)
		{
			puts("recieved interrupt: ");
			puts(itoa(regs.int_no));
			puts("  You pressed key with scancode ");
			puts(utoa(x));
			puts("\n");
		}
	}
}

extern gdt_ptr_t init_gdt();
extern idt_ptr_t init_idt();

idt_ptr_t idt;
gdt_ptr_t gdt;

void kernel_main(struct multiboot_info multiboot)
{
	int x = sizeof(gdt_ptr_t);
	puts(itoa(x));

	init_tty();
	clear();
	gdt = init_gdt();
	idt = init_idt();
	
	
	puts("Hello world from my custom kernel! ;)\n");
	asm("int3");

	/*puts(1, "msg on term 1");
	puts(2, "msg on term 2");
	puts(3, "msg on term 3");
	puts(4, "msg on term 4");*/
	
	for (;;)
		;
	
	asm("hlt");
}





