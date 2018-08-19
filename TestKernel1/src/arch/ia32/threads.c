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
	threads[currThreadsCount].id = currThreadsCount;

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

thread_t* get_thread()
{
	return &threads[currThreadIndex];
}

void change_thread(registers_t** regs)
{
	if (!threading_enabled)
		return;

	threads[currThreadIndex].stackPtr = *regs;
	currThreadIndex = (currThreadIndex + 1) % currThreadsCount;
	*regs = threads[currThreadIndex].stackPtr;
}

#define	_spin_try_lock(p)\
	(!({  register int _r__; \
	    __asm__ volatile("movl $1, %0; \n\
			  xchgl %0, %1" \
			: "=&r" (_r__), "=m" (*(p)) ); \
	    _r__; }))

slock_t slockInit()
{
	slock_t lock = { 0 };
	return lock;
}

void slockCapture(slock_t *lock)
{
	while (!_spin_try_lock(&(lock->state)))
		while (lock->state)
			; // do nothing
}

void slockRelease(slock_t *lock)
{
	if (lock->state == 1)
	{
		lock->state = 0;
	}
}