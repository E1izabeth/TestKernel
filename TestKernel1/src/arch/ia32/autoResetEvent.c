#include <arch/ia32/autoResetEvent.h>

static void set(autoResetEvent_t* ev)
{
	slockRelease(&ev->signal);
	//wake_queued_thread(&ev->threadsQueue);
}

static void reset(autoResetEvent_t* ev)
{
	slockSet(&ev->signal);
}

static void wait(autoResetEvent_t* ev)
{
	slockCapture(&ev->signal);
	//queue_waiting_thread(&ev->threadsQueue);
}

autoResetEventMethods_t autoResetEventMethods = { set, reset, wait };

autoResetEvent_t newAutoResetEvent(bool signal)
{
	autoResetEvent_t ev;
	ev._ = &autoResetEventMethods;
	ev.signal = slockInit();
	ev.threadsQueue = init_threads_queue();

	if (signal)
		ev._->set(&ev);
	else
		ev._->reset(&ev);

	return ev;
}
