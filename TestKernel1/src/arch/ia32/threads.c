#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/display.h>
#include <arch/ia32/threads.h>
#include <types.h>
#include <arch/ia32/cpu/cpu.h>

extern raw_selector_t codeSelector, dataSelector;

typedef struct StateInfo {
	int prev;
	int retAddr;
} StateInfo;

int currThreadIndex = 0;

volatile thread_t threads[MAX_TOTAL_THREADS];
volatile bool threading_enabled = false;
int currThreadsCount = 1;

void threading_start()
{
	threading_enabled = true;
}

int create_thread(void* start_function, void* stPointer)
{
	registers_t* regs = &threads[currThreadsCount].regs;

	regs->esp = stPointer;
	regs->ebp = 0;
	regs->eflags = 0x202;
	regs->eip = start_function;
	regs->cs = (u32)codeSelector.bits;
	regs->ss = (u32)dataSelector.bits;
	regs->ds = (u32)dataSelector.bits;

	currThreadsCount++;
}

void change_thread(registers_t* regs)
{
	if (!threading_enabled)
		return;

	threads[currThreadIndex].regs = *regs;
	currThreadIndex = (currThreadIndex + 1) % currThreadsCount;
	*regs = threads[currThreadIndex].regs;
}
