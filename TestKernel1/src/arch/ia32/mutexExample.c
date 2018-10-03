#include <arch/ia32/mutex.h>
#include <arch/ia32/threads.h>


static mutex_t mut;
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

	//private static MutexImpl mut = new MutexImpl();
	static int numIterations = 1;
	static int numThreads = 3;

	// This method represents a resource that must be synchronized
	// so that only one thread at a time can enter.
	static void UseResource()
	{
		// Wait until it is safe to enter.
		print("{0} is requesting the mutex");
		mut._->wait(&mut);

		print("has entered the protected area");

		// Place code to access non-reentrant resources here.

		// Simulate some work.
		sleep(500);

		print("is leaving the protected area");

		// Release the Mutex.
		mut._->release(&mut);
		print("{0} has released the mutex");
	}

	static void ThreadProc()
	{
		for (int i = 0; i < numIterations; i++)
		{
			UseResource();
		}
	}
	void mutexExample()
	{
		mut = newMutex();
		// Create the threads that will use the protected resource.
		for (int i = 0; i < numThreads; i++)
		{
			create_thread(ThreadProc, stack[i], sizeof(stack[i]));
		}

		// The main thread exits, but the application continues to
		// run until all foreground threads have exited.
	}



