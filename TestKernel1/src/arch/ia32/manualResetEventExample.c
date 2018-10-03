#include <arch/ia32/manualResetEvent.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/display.h>


manualResetEvent_t mre;
byte stack[5][1024];

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


void ThreadProc()
{
	print(" starts and calls mre.WaitOne()");

	mre._->wait(&mre);

	print("ends\n");
}

void manualResetEventExample()
{
	mre = newManualResetEvent(false);

	print("Start 3 named threads that block on a ManualResetEvent:\n");

	for (int i = 0; i <= 2; i++)
	{
		create_thread(ThreadProc, stack[i], sizeof(stack[i]));
	}

	sleep(100);

	print("set");
	mre._->set(&mre);

	sleep(500);

	for (int i = 3; i <= 4; i++)
	{
		create_thread(ThreadProc, stack[i], sizeof(stack[i]));
	}

	sleep(500);

	print("reset");
	mre._->reset(&mre);

	// Start a thread that waits on the ManualResetEvent.
	create_thread(ThreadProc, stack[5], sizeof(stack[5]));

	sleep(500);
	
	print("set");
	mre._->set(&mre);

}



