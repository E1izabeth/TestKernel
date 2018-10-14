#include <arch/ia32/semaphore.h>

static void wait(semaphore_t* sem)
{
	slockCapture(&sem->spinlock);

	if (sem->counter < sem->max)
	{
		sem->counter++;

		slockRelease(&sem->spinlock);
	}
	else
	{
		slockRelease(&sem->spinlock);
		//sem->guard._->waitOne(&sem->guard);
		queue_waiting_thread(&sem->threadsQueue);
		while (sem->counter == sem->max)
		{

		}
		sem->counter;
	}

}

static int release(semaphore_t* sem)
{
	slockCapture(&sem->spinlock);
	int counter = sem->counter;
	if (sem->counter < sem->min)
	{
		return -1;
		//throw new InvalidOperationException("Semaphore full");
	}
	--sem->counter;

	slockRelease(&sem->spinlock);
	//sem->guard._->set(&sem->guard);
	wake_queued_thread(&sem->threadsQueue);
	return counter;
}


semaphoreMethods_t semaphoreMethods = { release, wait };

semaphore_t newSemaphore(int max, int min)
{
	semaphore_t sem;
	sem._ = &semaphoreMethods;
	sem.spinlock = slockInit();
	sem.threadsQueue = init_threads_queue();
	sem.guard = newAutoResetEvent(true);
	sem.counter = max;
	sem.max = max;
	sem.min = min;
	return sem;
}
