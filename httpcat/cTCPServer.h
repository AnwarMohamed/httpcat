#pragma once
#include "HttpCat.h"
#include <process.h> 
#include "cTCPSConnection.h"

#define DEFAULT_LISTEN_QUEUE_SIZE	8

#pragma comment(lib,"ws2_32") 
#pragma comment(lib,"mswsock")
#pragma comment(lib,"shlwapi") 

class cTCPServer
{
protected:
	HANDLE*	WorkerThreads;
	UINT*	WorkerThreadsIds;
	BOOL	areWorkerThreadsUp;

	HANDLE		IOCompletionPort;
	cTCPSConnection**	Connections;

	static UINT WINAPI WorkerThreadProcedure(void* IoPort);
private:
	SOCKET		ServerSocket;
	SOCKADDR_IN ServerAddr;
	WSADATA		wsadata;
	
	
	BOOL	isSocketUp;
	BOOL	isIOComPortUp;
	
	BOOL	areConnectionsUp;
	BOOL	isInitialised;

	VOID	CreateWorkerThreads(UINT nWorkerThreads);
	virtual VOID	CreateWorkerThreadsProc(UINT nWorkerThreads);
	virtual VOID	CreateConnections(SOCKET ServerSocket, HANDLE IOCompletionPort, UINT MaxConcurrentConnections);

	UINT	nWorkerThreads;
	UINT	MaxConcurrentConnections;

public:
	cTCPServer(DWORD IP, UINT Port, UINT nWorkerThreads, UINT MaxConcurrentConnections);
	virtual ~cTCPServer(void);

	BOOL	isReady;
	BOOL	StartListening();
	VOID	Shutdown();

};

