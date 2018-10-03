#include <arch/ia32/monitor.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/display.h>

static byte stack[4][1024];

static monitor_t sync;
static int value = 0;

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

static void run()
{
	print("own:: Thread started");
	sync._->capture(&sync);
	if (value == 0)
	{
		print("own:: Waiting\n");
		sync._->wait(&sync);
		print("own:: Running again\n");
	}
	print("own:: data.value = ");
	puts(4, utoa(value));
	sync._->release(&sync);
}

void monitorExample()
{
	sync = newMonitor();
	create_thread(&run, stack[0], sizeof(stack[0]));
	
	print("main::Sleeping\n");
	sleep(300);
	
	print("main::capturing\n");
	sync._->capture(&sync);
	print("main::setting value to 1\n");
	value = 1;
	print("main::notifying thread\n");
	sync._->notify(&sync);
	print("main::Thread notified\n");
	sync._->release(&sync);

	print("Finished\n");
}
