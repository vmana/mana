#ifndef MANA_LIB_H
#define MANA_LIB_H

// *** include ***

#include <iostream>
#include <string>
#include <vector>

#ifndef _WIN32
	#include <future>
#else
	#include <winsock2.h>
	#include <windows.h>
	#include <winbase.h>
	#include <process.h>
	#include <tlhelp32.h>
	#include "win_dirent.h"
	#include <winerror.h>
	#include <ws2tcpip.h>
	#include <pthread.h>
	#include <sys/timeb.h>
	#include "mingw/mingw.mutex.h"
	#include "mingw/mingw.thread.h"

	//#define NOCURLHTTP

	typedef unsigned int uint;

	#define bzero(a, b) memset(a, '\0', b);
	#define getpid _getpid

	#ifndef S_IRWXU
		#define S_IRWXU 0
	#endif

	#ifndef S_IRWXG
		#define S_IRWXG 0
	#endif

	#ifndef S_IRWXO
		#define S_IRWXO 0
	#endif

#endif

/* #include "thread.h" */

using namespace std;

#endif
