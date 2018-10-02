#ifndef __mypthreadcpp__
#define __mypthreadcpp__

#include "thread.h"


#ifdef WIN32
// thread

int pthread_create(pthread_t *thread, int *attr, LPTHREAD_START_ROUTINE lpStartAddress, threadParam arg)
{
	//SetLastError(0);
	*thread = CreateThread(NULL, 0, lpStartAddress, arg, 0, NULL);
	//cout << GetLastError() << endl;
	return 1;
}

int pthread_detach(pthread_t th)
{
	return  0;
}

int pthread_join(pthread_t th, void **value_ptr)
{
	WaitForSingleObject(th, INFINITE);
	CloseHandle(th);
	return 0;
}

void pthread_exit(void *value_ptr)
{
	ExitThread(0);
}

int pthread_attr_init(pthread_attr_t *value_ptr)
{
	return 0;
}

int pthread_attr_getstacksize(pthread_attr_t *value_ptr, size_t *a)
{
	return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *value_ptr, size_t a)
{
	return 0;
}

int pthread_self()
{
	return (unsigned int)GetCurrentThreadId();
}


// mutex

int pthread_mutex_init(pthread_mutex_t *mutex, int *attr)
{
	InitializeCriticalSection(mutex);
	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	EnterCriticalSection(mutex);
	return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	LeaveCriticalSection(mutex);
	return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	DeleteCriticalSection(mutex);
	return 0;
}

// cond & signal

int pthread_cond_init(pthread_cond_t *cv, int *cattr)
{
	cv->H = CreateEvent(NULL, true, false, NULL);
	cv->nbsignals = 0;
	return 0;
}

int pthread_cond_signal(pthread_cond_t *cv)
{
	//WakeConditionVariable(cv);
	cv->nbsignals++; // it should have external protections
	SetEvent(cv->H);
	return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cv)
{
	//WakeAllConditionVariable(cv);
	cv->nbsignals += 1000; // it should have external protections
	SetEvent(cv->H);
	return 0;
}

int pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex)
{
	while(1)
	{
		pthread_mutex_unlock(mutex); // release the mutex and wait
		WaitForSingleObject(cv->H, INFINITE); // waiting for a signal
		pthread_mutex_lock(mutex); // get the mutex back
		if (cv->nbsignals > 0) // do someone have consumed the signal since the mutex ?
		{
			cv->nbsignals--;
			if (cv->nbsignals <= 0) ResetEvent(cv->H);
			return 0; // return with the mutex
		}
		else // no more signal, unsetting the signal
		{
			ResetEvent(cv->H);
		}
	} // relase mutex, and wait again
	return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cv, pthread_mutex_t *mutex, struct timespec *t)
{
	while(1)
	{
		pthread_mutex_unlock(mutex); // release the mutex and wait
		// waiting for a signal or timeout
		if (WaitForSingleObject(cv->H, 1000*t->tv_sec) == WAIT_TIMEOUT)
		{
			pthread_mutex_lock(mutex);
			return ETIMEDOUT; // return, with the mutex
		}

		pthread_mutex_lock(mutex); // get the mutex back
		if (cv->nbsignals > 0) // do someone have consumed the signal since the mutex ?
		{
			cv->nbsignals--;
			return 0; // return with the mutex
		}
		else // no more signal, unsetting the signal
		{
			ResetEvent(cv->H);
		}
	} // relase mutex, and wait again

	//if (!SleepConditionVariableCS(cv, mutex, 1000*t->tv_sec)) return ETIMEDOUT;
	return 0;
}

int pthread_cond_destroy(pthread_cond_t *cv)
{
	CloseHandle(cv->H);
	return 0;
}

#endif // WIN32

void set_time_spec(struct timespec *to, int delay, int milli)
{
	to->tv_sec = delay;
	to->tv_nsec = milli*1000;
	#ifndef WIN32
	to->tv_sec += time(NULL);
	#endif
}

#endif
