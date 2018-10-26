#include <arch/ia32/threads.h>

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


void sleep(int mills)
{
	thread_t* thread = get_thread();
	thread->state = THREADSTATE_WAITSLEEPJOIN;

	for (int i = 0; i < mills / currThreadsCount; i++)
	{
		asm("hlt");
	}

	thread->state = THREADSTATE_RUNNING;
}

void sys_call_interrupt(byte n)
{
#pragma pack(push, 1)
	struct {
		// byte popEaxOpCode;
		byte intOpCode;
		byte intOpArg;
		byte retOpCode;
	} code = { /*0x58,*/ 0xCD, n, 0xC3 };
#pragma pack(pop)

	((void(*)())(&code))();
}

#define SYS_CALL_INTERRUPT(n) { asm("int " #n); }

void th_main_func()
{
	thread_t* thread = get_thread();
	thread->state = THREADSTATE_RUNNING;
	thread->startFunc(0);
	thread->isUnderSheduling = false;
	thread->state = THREADSTATE_STOPPED;
	
	// sys_call_interrupt(3);
	// SYS_CALL_INTERRUPT(3);

	for (;;) 
		asm("hlt");
}

bool wake_thread(int id)
{
	bool done;

	if (id >= 0 && id < currThreadsCount)
	{
		volatile thread_t* th = &threads[id];
		if (th->state == THREADSTATE_WAITSLEEPJOIN && !th->isUnderSheduling)
		{
			th->state = THREADSTATE_RUNNING;
			th->isUnderSheduling = true;
		}
		else
		{
			done = false;
		}
	}
	else
	{
		done = false;
	}

	return done;
}

void pause_curr_thread()
{
	thread_t* thread = get_thread();
	thread->state = THREADSTATE_WAITSLEEPJOIN;
	thread->isUnderSheduling = false;
	sys_call_interrupt(3);
}

void passive_wait_cond(bool(*cond)(void*), void* args)
{
	while (!cond(args))
	{
		pause_curr_thread();
	}
}

void active_wait_cond(bool(*cond)(void*), void* args)
{
	while (!cond(args))
	{
		asm("hlt");
	}
}

sleeping_threads_queue_t init_threads_queue()
{
	sleeping_threads_queue_t q;
	q.head = 0;
	q.tail = 0;
	q.count = 0;
	q.lock = slockInit();

	return q;
}

void wake_queued_thread(sleeping_threads_queue_t* q)
{
	slockCapture(&q->lock);

	sleeping_thread_entry_t* l = q->head;

	if (l != 0)
	{
		int threadId = l->threadId;

		sleeping_thread_entry_t* next = l->next;
		q->head = next;

		if (next == 0)
			q->tail = 0;
	
		q->count--;

		wake_thread(threadId);
	}

	slockRelease(&q->lock);
}

void wake_queued_threads(sleeping_threads_queue_t* q)
{
	slockCapture(&q->lock);

	sleeping_thread_entry_t* l = q->head;

	while (l != 0)
	{
		int threadId = l->threadId;

		sleeping_thread_entry_t* next = l->next;
		q->head = next;

		if (next == 0)
			q->tail = 0;

		q->count--;

		wake_thread(threadId);
		l = q->head;
	}

	slockRelease(&q->lock);
}

void queue_waiting_thread(sleeping_threads_queue_t* q)
{
	slockCapture(&q->lock);
	
	sleeping_thread_entry_t entry;
	entry.threadId = get_current_thread_id();
	entry.next = 0;

	if (q->head == 0)
	{
		q->head = &entry;
	}
	else
	{
		q->tail->next = &entry;
	}
	
	q->tail = &entry;
	q->count++;

	slockRelease(&q->lock);
	
	pause_curr_thread();
}

void init_main_thread()
{
	threads[0].isUnderSheduling = true;
	threads[0].state = THREADSTATE_RUNNING;
}

int create_thread(void* (*start_function)(void*), void* stPointer, int stackSize)
{
	byte* stackPtr = ((byte*)stPointer) + stackSize;
	stackPtr -= sizeof(registers_t);

	thread_t* th = (thread_t*)&threads[currThreadsCount];
	th->stackPtr = stackPtr;
	th->id = currThreadsCount;
	th->isUnderSheduling = true;
	th->state = THREADSTATE_UNSTARTED;
	th->startFunc = start_function;

	registers_t* regs = (registers_t*)stackPtr;
	memset(regs, 0, sizeof(registers_t));

	regs->gen_regs.esp = (u32)(stackPtr + sizeof(u32) + sizeof(registers_generic_t));
	regs->gen_regs.ebp = 0;
	regs->eflags = 0x202;

	regs->eip = (u32)(void*)th_main_func;
	regs->cs = (u32)codeSelector.bits;	//0x18
	// regs->ss = (u32)dataSelector.bits;	//0x20
	regs->ds = (u32)dataSelector.bits;	//0x20

	currThreadsCount++;

	return th->id;
}

thread_t* get_thread()
{
	return (thread_t*)&threads[currThreadIndex];
}

int get_current_thread_id()
{
	return currThreadIndex;
}

void change_thread(registers_t** regs)
{
	if (!threading_enabled)
		return;
	
	threads[currThreadIndex].stackPtr = (byte*)*regs;
	
	do
	{
		currThreadIndex = (currThreadIndex + 1) % currThreadsCount;
	} while (!threads[currThreadIndex].isUnderSheduling);

	*regs = (registers_t*)threads[currThreadIndex].stackPtr;
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

void slockSet(slock_t *lock)
{
	lock->state = 1;
}