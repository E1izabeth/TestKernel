#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/display.h>
#include <arch/ia32/threads.h>
#include <types.h>
#include <mem.h>
#include <arch/ia32/cpu/cpu.h>

extern raw_selector_t codeSelector, dataSelector;

typedef struct StateInfo {
	int prev;
	int retAddr;
} StateInfo;


volatile thread_t threads[MAX_TOTAL_THREADS];
volatile bool threading_enabled = false;
int currThreadsCount = 1;
int currThreadIndex = 0;

void threading_start()
{
	threading_enabled = true;
}

int create_thread(void* start_function, void* stPointer, int stackSize)
{
	byte* stackPtr = ((byte*)stPointer) + stackSize;
	stackPtr -= sizeof(registers_t);
	threads[currThreadsCount].stackPtr = stackPtr;

	registers_t* regs = (registers_t*)stackPtr;
	memset(regs, 0, sizeof(registers_t));

	regs->gen_regs.esp = stackPtr + sizeof(u32) + sizeof(registers_generic_t);
	regs->gen_regs.ebp = 0;
	regs->eflags = 0x202;
	regs->eip = start_function;
	regs->cs = (u32)codeSelector.bits;	//0x18
	// regs->ss = (u32)dataSelector.bits;	//0x20
	regs->ds = (u32)dataSelector.bits;	//0x20

	currThreadsCount++;
}

void change_thread(registers_t** regs)
{
	if (!threading_enabled)
		return;

	threads[currThreadIndex].stackPtr = *regs;
	currThreadIndex = (currThreadIndex + 1) % currThreadsCount;
	*regs = threads[currThreadIndex].stackPtr;
}