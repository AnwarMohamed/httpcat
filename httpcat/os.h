#ifdef _WIN32
#include <winsock2.h>
#include <mswsock.h> 
#include <Windows.h>
#endif

#ifdef _WIN32
class cMutex
{
	CRITICAL_SECTION Mutex;
public:
	cMutex(char* Name);
	~cMutex();
	void Lock();
	void Unlock();
};

class cThread
{
public:
	cThread();
	~cThread();
};

#else
class cMutex
{
public:
	cMutex(char* Name);
	~cMutex();
};

class cThread
{
public:
	cThread();
	~cThread();
};

#endif

