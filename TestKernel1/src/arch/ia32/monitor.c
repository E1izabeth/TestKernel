#include <arch/ia32/monitor.h>

static bool releaseImpl(monitor_t* mon)
{
	if (mon->ownerThreadId == get_current_thread_id())
	{
		mon->enterCounter--;

		if (mon->enterCounter == 0)
		{
			mon->ownerThreadId = -1;

			wake_queued_thread(&mon->threadsQueueRelease);
			//mon->releaseSignal._->set(&mon->releaseSignal);
		}
	}
	else
	{
		return false;
	}
	return true;
}

static void release(monitor_t* mon)
{
	slockCapture(&mon->lock);
	releaseImpl(mon);
	slockRelease(&mon->lock);
}

void captureImpl(monitor_t* mon)
{
	if (mon->ownerThreadId == get_current_thread_id())
	{
		mon->enterCounter++;
	}
	else if (mon->ownerThreadId != -1)
	{
		while (mon->ownerThreadId != -1)
		{
			slockRelease(&mon->lock);
			//mon->releaseSignal._->wait(&mon->releaseSignal);
			queue_waiting_thread(&mon->threadsQueueRelease);
			slockCapture(&mon->lock);

		}

		mon->ownerThreadId = get_current_thread_id();
		mon->enterCounter++;
	}
	else
	{
		mon->ownerThreadId = get_current_thread_id();
		mon->enterCounter++;
	}
}


static bool wait(monitor_t* mon)
{
	slockCapture(&mon->lock);
	if (mon->ownerThreadId == get_current_thread_id())
	{
		do
		{
			releaseImpl(mon);
			slockRelease(&mon->lock);

			queue_waiting_thread(&mon->threadsQueuePulse);
			//mon->pulseSignal._->wait(&mon->pulseSignal);

			slockCapture(&mon->lock);
		} while (mon->isMultiPulse || (mon->isPulseHandled && !mon->isMultiPulse));

		mon->isPulseHandled = true;
		captureImpl(mon);
	}
	else
	{
		return false;
	}
	slockRelease(&mon->lock);
	return true;
}

static void capture(monitor_t* mon)
{
	slockCapture(&mon->lock);
	captureImpl(mon);
	slockRelease(&mon->lock);
}

static void notify(monitor_t* mon)
{
	slockCapture(&mon->lock);
	mon->isMultiPulse = false;
	mon->isPulseHandled = false;
	wake_queued_thread(&mon->threadsQueuePulse);
	//mon->pulseSignal._->set(&mon->pulseSignal);
	slockRelease(&mon->lock);
}

static void notifyAll(monitor_t* mon)
{
	slockCapture(&mon->lock); 
	mon->isMultiPulse = true;
	mon->isPulseHandled = false;
	wake_queued_thread(&mon->threadsQueuePulse);
	//mon->pulseSignal._->set(&mon->pulseSignal);
	slockRelease(&mon->lock);
}

monitorMethods_t monitorMethods = { capture, release, wait, notify, notifyAll };

monitor_t newMonitor()
{
	monitor_t mon;
	mon._ = &monitorMethods;
	mon.enterCounter = 0;
	mon.ownerThreadId = -1;
	mon.isMultiPulse = false;
	mon.isPulseHandled = false;
	mon.pulseSignal = newManualResetEvent(false);
	mon.releaseSignal = newManualResetEvent(false);
	mon.lock = slockInit();
	mon.threadsQueuePulse = init_threads_queue();
	mon.threadsQueueRelease = init_threads_queue();
	return mon;
}
