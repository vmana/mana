#ifndef MANA_LIB_H
#define MANA_LIB_H

#define mana_useragent	"Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:21.0) Gecko/20100101 Firefox/21.0"

// Errors
#define mana_error_http_get			"mana.error : unable to perform an url get"
#define mana_error_http_upload		"mana.error : unable to perform an url upload"
#define mana_error_file_open		"mana.error : unable to open file "
#define mana_error_imap				"mana.error : unable to perform an imap"

// *** include ***

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <cstdio>
#include <stdlib.h>
#include <sys/stat.h>
#include <memory>

#ifndef _WIN32

	#include <sys/types.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/param.h>
	#include <dirent.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <pthread.h>
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
