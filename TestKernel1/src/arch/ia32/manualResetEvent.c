#include <arch/ia32/manualResetEvent.h>

static void set(manualResetEvent_t* ev)
{
	slockCapture(&ev->lock);

	ev->isSet = true;
	slockRelease(&ev->signal);

	slockRelease(&ev->lock);
}

static void reset(manualResetEvent_t* ev)
{
	slockCapture(&ev->lock);

	slockSet(&ev->signal);
	ev->isSet = false;

	slockRelease(&ev->lock);
}

static void wait(manualResetEvent_t* ev)
{
	slockCapture(&ev->signal);

	slockCapture(&ev->lock);

	if (&ev->isSet)
		slockRelease(&ev->signal);

	slockRelease(&ev->lock);
}

manualResetEventMethods_t manualResetEventMethods = { set, reset, wait };

manualResetEvent_t newManualResetEvent(bool signal)
{
	manualResetEvent_t ev;
	ev._ = &manualResetEventMethods;
	ev.signal = slockInit();
	ev.lock = slockInit();
	ev.isSet = signal;

	if (signal)
		ev._->set(&ev);
	else
		ev._->reset(&ev);

	return ev;
}
