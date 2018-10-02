#ifndef __mypthreadh__
#define __mypthreadh__

#include <time.h>

void set_time_spec(struct timespec *to, int delay, int milli = 0);

#define thread_ret void*
#define thread_arg void*

#ifndef _WIN32

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <future>

#else

// WIN32

//#define mana_pthread

#include <pthread.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winbase.h>
#include <sys/timeb.h>

// to remove
#include <iostream>
using namespace std;

typedef CRITICAL_SECTION pthread_mutex_t;
typedef CRITICAL_SECTION pthread_rwlock_t;
typedef char pthread_attr_t;

typedef struct
{
	int nbsignals;
	HANDLE H;
} pthread_cond_t;

typedef HANDLE pthread_t;

#ifndef ETIMEDOUT
	#define ETIMEDOUT	110
#endif

#define thread_ret DWORD WINAPI
#define thread_arg LPVOID

struct timespec
{
	DWORD tv_sec;
	DWORD tv_nsec;
};

// thread

int pthread_create(pthread_t *thread, int *attr, LPTHREAD_START_ROUTINE lpStartAddress, threadParam arg);
int pthread_detach(pthread_t th);
int pthread_join(pthread_t th, void **value_ptr);
void pthread_exit(void *value_ptr);
int pthread_attr_init(pthread_attr_t*);
int pthread_attr_getstacksize(pthread_attr_t*, size_t*);
int pthread_attr_setstacksize(pthread_attr_t*, size_t);
int pthread_self();

// mutex

int pthread_mutex_init(pthread_mutex_t *mutex, int *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

// mutex read/write
#define pthread_rwlock_init(mutex, attr) pthread_mutex_init(mutex, attr)
#define pthread_rwlock_rdlock(mutex) pthread_mutex_lock(mutex)
#define pthread_rwlock_wrlock(mutex) pthread_mutex_lock(mutex)
#define pthread_rwlock_unlock(mutex) pthread_mutex_unlock(mutex)
#define pthread_rwlock_destroy(mutex) pthread_mutex_destroy(mutex)


// cond & signal
int pthread_cond_init(pthread_cond_t *cv, int *cattr);
int pthread_cond_signal(pthread_cond_t *cv);
int pthread_cond_broadcast(pthread_cond_t *cv);
int pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cv, pthread_mutex_t *mutex, struct timespec *t);
int pthread_cond_destroy(pthread_cond_t *cv);

#endif // _WIN32

/**********			Thread with 0 args		***********/
template <typename T, typename C>
class manastart0
{
	public:
	manastart0(T (C::*_fn)(), C *_object)
	{
		fn = _fn;
		object = _object;
	}
	void start() { pthread_t th; pthread_create(&th, NULL, &manastart0::callback_start, (void*)this); pthread_detach(th); }
	static thread_ret callback_start(thread_arg P)
	{
		manastart0 *p = (manastart0*)P;
		p->callfunction();
		delete p;
		return NULL;
	}
	void callfunction() { (object->*fn)(); }
	private: T (C::*fn)(); C *object;
};

/**********			Thread with 1 args		***********/
template <typename T, typename C, typename P1, typename A1>
class manastart1
{
	public:
	manastart1(T (C::*_fn)(P1), C *_object, A1 &_a1)
	{
		fn = _fn;
		object = _object;
		a1 = _a1;
	}
	void start() { pthread_t th; pthread_create(&th, NULL, &manastart1::callback_start, (void*)this); pthread_detach(th); }
	static thread_ret callback_start(thread_arg P)
	{
		manastart1 *p = (manastart1*)P;
		p->callfunction();
		delete p;
		return NULL;
	}
	void callfunction() { (object->*fn)(a1); }
	private: T (C::*fn)(P1); C *object; A1 a1;
};

/**********			Thread with 2 args		***********/
template <typename T, typename C, typename P1, typename A1, typename P2, typename A2>
class manastart2
{
	public:
	manastart2(T (C::*_fn)(P1, P2), C *_object, A1 &_a1, A2 &_a2)
	{
		fn = _fn;
		object = _object;
		a1 = _a1;
		a2 = _a2;
	}
	void start() { pthread_t th; pthread_create(&th, NULL, &manastart2::callback_start, (void*)this); pthread_detach(th); }
	static thread_ret callback_start(thread_arg P)
	{
		manastart2 *p = (manastart2*)P;
		p->callfunction();
		delete p;
		return NULL;
	}
	void callfunction() { (object->*fn)(a1, a2); }
	private: T (C::*fn)(P1, P2); C *object; A1 a1; A2 a2;
};

/**********			Thread with 3 args		***********/
template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3, typename A3>
class manastart3
{
	public:
	manastart3(T (C::*_fn)(P1, P2, P3), C *_object, A1 &_a1, A2 &_a2, A3 &_a3)
	{
		fn = _fn;
		object = _object;
		a1 = _a1;
		a2 = _a2;
		a3 = _a3;
	}
	void start() { pthread_t th; pthread_create(&th, NULL, &manastart3::callback_start, (void*)this); pthread_detach(th); }
	static thread_ret callback_start(thread_arg P)
	{
		manastart3 *p = (manastart3*)P;
		p->callfunction();
		delete p;
		return NULL;
	}
	void callfunction() { (object->*fn)(a1, a2, a3); }
	private: T (C::*fn)(P1, P2, P3); C *object; A1 a1; A2 a2; A3 a3;
};

/**********			Thread with 4 args		***********/
template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3, typename A3,
	typename P4, typename A4>
class manastart4
{
	public:
	manastart4(T (C::*_fn)(P1, P2, P3, P4), C *_object, A1 &_a1, A2 &_a2, A3 &_a3, A4 &_a4)
	{
		fn = _fn;
		object = _object;
		a1 = _a1;
		a2 = _a2;
		a3 = _a3;
		a4 = _a4;
	}
	void start() { pthread_t th; pthread_create(&th, NULL, &manastart4::callback_start, (void*)this); pthread_detach(th); }
	static thread_ret callback_start(thread_arg P)
	{
		manastart4 *p = (manastart4*)P;
		p->callfunction();
		delete p;
		return NULL;
	}
	void callfunction() { (object->*fn)(a1, a2, a3, a4); }
	private: T (C::*fn)(P1, P2, P3, P4); C *object; A1 a1; A2 a2; A3 a3; A4 a4;
};

/**********			Thread with 5 args		***********/
template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3, typename A3,
	typename P4, typename A4, typename P5, typename A5>
class manastart5
{
	public:
	manastart5(T (C::*_fn)(P1, P2, P3, P4, P5), C *_object, A1 &_a1, A2 &_a2, A3 &_a3, A4 &_a4, A5 &_a5)
	{
		fn = _fn;
		object = _object;
		a1 = _a1;
		a2 = _a2;
		a3 = _a3;
		a4 = _a4;
		a5 = _a5;
	}
	void start() { pthread_t th; pthread_create(&th, NULL, &manastart5::callback_start, (void*)this); pthread_detach(th); }
	static thread_ret callback_start(thread_arg P)
	{
		manastart5 *p = (manastart5*)P;
		p->callfunction();
		delete p;
		return NULL;
	}
	void callfunction() { (object->*fn)(a1, a2, a3, a4, a5); }
	private: T (C::*fn)(P1, P2, P3, P4, P5); C *object; A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;
};

/**********			Thread with 6 args		***********/
template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3, typename A3,
	typename P4, typename A4, typename P5, typename A5, typename P6, typename A6>
class manastart6
{
	public:
	manastart6(T (C::*_fn)(P1, P2, P3, P4, P5, P6), C *_object, A1 &_a1, A2 &_a2, A3 &_a3, A4 &_a4, A5 &_a5, A6 &_a6)
	{
		fn = _fn;
		object = _object;
		a1 = _a1;
		a2 = _a2;
		a3 = _a3;
		a4 = _a4;
		a5 = _a5;
		a6 = _a6;
	}
	void start() { pthread_t th; pthread_create(&th, NULL, &manastart6::callback_start, (void*)this); pthread_detach(th); }
	static thread_ret callback_start(thread_arg P)
	{
		manastart6 *p = (manastart6*)P;
		p->callfunction();
		delete p;
		return NULL;
	}
	void callfunction() { (object->*fn)(a1, a2, a3, a4, a5, a6); }
	private: T (C::*fn)(P1, P2, P3, P4, P5, P6); C *object; A1 a1; A2 a2; A3 a3; A4 a4; A5 a5; A6 a6;
};

/**********			Run		***********/

namespace mana
{

#ifndef _WIN32
template <typename F, typename... T>
inline std::future<typename std::result_of<F(T...)>::type> async_function(F&& f, T&&... params)
{
	return std::async(std::launch::async, std::forward<F>(f), std::forward<T>(params)...);
}
#endif

class mthread
{
	public :
	mthread() {};

	template <typename T, typename C>
	static void run(C *object, T (C::*fn)())
	{
		( new manastart0<T, C>(fn, object) )->start();
	}

	template <typename T, typename C, typename P1, typename A1>
	static void run(C *object, T (C::*fn)(P1), A1 &a1)
	{
		( new manastart1<T, C, P1, A1>(fn, object, a1) )->start();

	}

	template <typename T, typename C, typename P1, typename A1, typename P2, typename A2>
	static void run(C *object, T (C::*fn)(P1, P2), A1 &a1, A2 &a2)
	{
		( new manastart2<T, C, P1, A1, P2, A2>(fn, object, a1, a2) )->start();

	}

	template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3, typename A3>
	static void run(C *object, T (C::*fn)(P1, P2, P3), A1 &a1, A2 &a2, A3 &a3)
	{
		( new manastart3<T, C, P1, A1, P2, A2, P3, A3>(fn, object, a1, a2, a3) )->start();

	}

	template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3, typename A3,
		typename P4, typename A4>
	static void run(C *object, T (C::*fn)(P1, P2, P3, P4), A1 &a1, A2 &a2, A3 &a3, A4 &a4)
	{
		( new manastart4<T, C, P1, A1, P2, A2, P3, A3, P4, A4>(fn, object, a1, a2, a3, a4) )->start();

	}

	template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3,typename A3,
		typename P4, typename A4, typename P5, typename A5>
	static void run(C *object, T (C::*fn)(P1, P2, P3, P4, P5), A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5)
	{
		( new manastart5<T, C, P1, A1, P2, A2, P3, A3, P4, A4, P5, A5>(fn, object, a1, a2, a3, a4, a5) )->start();

	}

	template <typename T, typename C, typename P1, typename A1, typename P2, typename A2, typename P3, typename A3,
		typename P4, typename A4, typename P5, typename A5, typename P6, typename A6>
	static void run(C *object, T (C::*fn)(P1, P2, P3, P4, P5, P6), A1 &a1, A2 &a2, A3 &a3, A4 &a4, A5 &a5, A6 &a6)
	{
		( new manastart6<T, C, P1, A1, P2, A2, P3, A3, P4, A4, P5, A5, P6, A6>(fn, object, a1, a2, a3, a4, a5, a6) )->start();

	}
}; // thread

} // namespace
#endif
