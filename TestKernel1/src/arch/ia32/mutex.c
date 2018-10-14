#include <arch/ia32/mutex.h>

static void wait(mutex_t* mut)
{
	slockCapture(&mut->lock);

	if (mut->ownerThreadId == get_current_thread_id())
	{
		mut->enterCounter++;
		slockRelease(&mut->lock);
	}
	else
	{
		while (mut->ownerThreadId != -1)
		{
			slockRelease(&mut->lock);
			//mut->releaseSignal._->waitOne(&mut->releaseSignal);
			queue_waiting_thread(&mut->threadsQueue);
			slockCapture(&mut->lock);
		}

		mut->ownerThreadId = get_current_thread_id();
		mut->enterCounter++;
		slockRelease(&mut->lock);
	}
}

static void release(mutex_t* mut)
{
	slockCapture(&mut->lock);
	bool released;
	if (mut->ownerThreadId == get_current_thread_id())
	{
		mut->enterCounter--;

		if (mut->enterCounter == 0)
		{
			mut->ownerThreadId = -1;

			//mut->releaseSignal._->set(&mut->releaseSignal);
			wake_queued_thread(&mut->threadsQueue);
		}

		released = true;
	}
	else
	{
		released = false;
	}

	slockRelease(&mut->lock);
}

mutexMethods_t mutexMethods = { wait, release };

mutex_t newMutex()
{
	mutex_t mut;
	mut._ = &mutexMethods;
	mut.releaseSignal = newAutoResetEvent(false);
	mut.enterCounter = 0;
	mut.ownerThreadId = -1;
	mut.lock = slockInit();
	mut.threadsQueue = init_threads_queue();
	return mut;
}
