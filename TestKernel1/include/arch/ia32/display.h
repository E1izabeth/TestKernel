#include <types.h>

#ifndef DISPLAY_H
#define DISPLAY_H

#define VIDEO_HEIGHT 25   //высота экрана
#define VIDEO_WIDTH 80    //ширина экрана
#define VIDEO_RAM 0xb8000 //адрес видеопамяти

#define NUM_TERMINALS 10

typedef struct
{
	int tmp_terminal_num;
}display_t;

typedef struct
{
	byte buff[2 * VIDEO_HEIGHT * VIDEO_WIDTH + VIDEO_WIDTH * 2];
	int tty_cursor;    //положение курсора
	int tty_attribute; //текущий аттрибут символа
	int terminal_num;
	bool is_visible;
}terminal_t;


terminal_t terminals[NUM_TERMINALS];
display_t display;


void init_tty(int term_num);
void init_terminal(int term_num);
void init_display();
void buff_to_display(char buff[2 * VIDEO_HEIGHT * VIDEO_WIDTH]);
void change_terminal(int term_num);
void textcolor(int term_num, char c);
void clear(int term_num);
void putchar(int term_num, char c);
void puts(int term_num, const char *s);


#endif