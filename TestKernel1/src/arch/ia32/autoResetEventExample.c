#include <arch/ia32/autoResetEvent.h>
#include <arch/ia32/threads.h>


static autoResetEvent_t event_1;
static autoResetEvent_t event_2;
static byte stack[4][4096];

static slock_t lock;

static void print(char* str)
{
	//slockCapture(&lock);

	for (int i = 0; i < get_current_thread_id(); i++)
		puts(4, "\t");

	puts(4, utoa(get_current_thread_id()));
	puts(4, ": ");
	puts(4, str);
	puts(4, "\n");

	//slockRelease(&lock);
}

static void ThreadProc(void * arg)
{
	print("waits on AutoResetEvent #1.");
	event_1._->waitOne(&event_1);
	print("released from AutoResetEvent #1.");
	print("waiting on AutoResetEvent #2.");
	event_2._->waitOne(&event_2);
	print("released from AutoResetEvent #2.");
	print("finished.");
}
void autoResetEventExample()
{
	print("Starting autoResetEventExample");
	
	event_1 = newAutoResetEvent(true);
	event_2 = newAutoResetEvent(false);

	for (int i = 1; i < 4; i++)
	{
		create_thread(&ThreadProc, stack[i], 4096);
	}
	
	print("Threads created");

	sleep(100);

	print("Continuing");
	
	for (int i = 0; i < 2; i++)
	{
		event_1._->set(&event_1);
		print("Event 1 raised");
	}
	
	sleep(100);
	
	print("All threads are now waiting on AutoResetEvent #2.");

	for (int i = 0; i < 3; i++)
	{
		event_2._->set(&event_2);
		print("Event 2 raised");
	}

	print("Main thread finished");
}

