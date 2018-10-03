#include <arch/ia32/display.h>

void init_tty(int term_num)
{
	terminals[term_num].tty_cursor = 0;
	terminals[term_num].tty_attribute = 7;
}

void buff_to_display(char buff[2 * VIDEO_HEIGHT * VIDEO_WIDTH])
{
	for (int i = 0; i < 2 * VIDEO_HEIGHT * VIDEO_WIDTH; i++)
	{
		*((char*)VIDEO_RAM + i) = *((char*)buff + i);
	}
}

void init_terminal(int term_num)
{
	clear(term_num);
	if (term_num == 0)
		terminals[term_num].is_visible = true;
	else
		terminals[term_num].is_visible = false;
	terminals[term_num].terminal_num = term_num;
	init_tty(term_num);
}

void init_display()
{
	for (uint i = 0; i < NUM_TERMINALS; i++)
	{
		init_terminal(i);
	}
	display.tmp_terminal_num = 0;
	buff_to_display(terminals[0].buff);
}

void change_terminal(int term_num)
{
	terminals[display.tmp_terminal_num].is_visible = false;
	display.tmp_terminal_num = term_num;
	terminals[term_num].is_visible = true;
	buff_to_display(terminals[term_num].buff);
}

//Смена текущего аттрибута символа
void textcolor(int term_num, char c)
{
	terminals[term_num].tty_attribute = c;
}

//Очистка экрана
void clear(int term_num)
{
	//char *video = (char*)VIDEO_RAM;
	char* video = (char*)terminals[term_num].buff;
	int i;

	for (i = 0; i < VIDEO_HEIGHT * VIDEO_WIDTH; i++) 
	{
		*(video + i * 2) = ' ';
	}

	terminals[term_num].tty_cursor = 0;
	if (terminals[term_num].is_visible)
		buff_to_display(terminals[term_num].buff);
}


//Вывод одного символа в режиме телетайпа
void putchar(int term_num, char c)
{
	//char *video = (char*)VIDEO_RAM;
	char* video = (char*)terminals[term_num].buff;
 	int i;

	switch (c) {
	case '\n': //Если это символ новой строки
		terminals[term_num].tty_cursor += VIDEO_WIDTH;
		terminals[term_num].tty_cursor -= terminals[term_num].tty_cursor % VIDEO_WIDTH;
		break;
	case '\t': //Если это символ новой строки
		terminals[term_num].tty_cursor += 4;
		break;

	default:
		*(video + terminals[term_num].tty_cursor * 2) = c;
		*(video + terminals[term_num].tty_cursor * 2 + 1) = terminals[term_num].tty_attribute;
		terminals[term_num].tty_cursor++;
		break;
	}

	//Если курсор вышел за границы экрана
	if (terminals[term_num].tty_cursor >= VIDEO_WIDTH * VIDEO_HEIGHT)
	{
		for (i = VIDEO_WIDTH * 2; i <= VIDEO_WIDTH * VIDEO_HEIGHT * 2 + VIDEO_WIDTH * 2; i++) 
		{
			*(video + i - VIDEO_WIDTH * 2) = *(video + i);
		}
		terminals[term_num].tty_cursor -= VIDEO_WIDTH;
	}
	/*if (terminals[term_num].tty_cursor >= VIDEO_WIDTH * VIDEO_HEIGHT)
	{
		clear(term_num);
	}*/
	
	if (terminals[term_num].is_visible)
		buff_to_display(video);
}

//Вывод строки, заканчивающейся нуль-символом
void puts(int term_num, const char *s)
{
	int i = 0;
	while (s[i])
	{
		putchar(term_num, s[i++]);
	}
	/*while (*s) {
		putchar(*s);
		*++s;
	}*/
}
