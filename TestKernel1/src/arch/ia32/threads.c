#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/display.h>
#include <arch/ia32/threads.h>
#include <types.h>

int tmp_thread_num;

int change_thread(thread_t threads[NUM_THREADS], registers_t regs)
{
	threads[tmp_thread_num].eip = regs.eip;
	int i;
	for (i = 0; i < NUM_THREADS; ++i)
	{
		if (i != tmp_thread_num && threads[i].processed == true)
		{
			break;
		}
	}
	if (i < NUM_THREADS)
	{
		regs.eip = threads[i].eip;
		tmp_thread_num = i;
	}
	return tmp_thread_num;
}

void init_threads(thread_t threads[NUM_THREADS])
{
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads[i].num = i;
		threads[i].eip = 0;//TODO:?
		threads[i].processed = true;
	}
}
