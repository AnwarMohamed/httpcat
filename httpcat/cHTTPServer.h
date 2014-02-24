#pragma once
#include "HttpCat.h"
#include "cTCPServer.h"

class cHTTPServer : public cTCPServer
{
public:
	cHTTPServer(DWORD IP, UINT Port, UINT nWorkerThreads, UINT MaxConcurrentConnections);
	~cHTTPServer();

private:
	VOID	CreateWorkerThreadsProc(UINT nWorkerThreads);
	VOID	CreateConnections(SOCKET ServerSocket, HANDLE IOCompletionPort, UINT MaxConcurrentConnections);

private:
	static UINT WINAPI WorkerThreadProcedure(void* IoPort);
};

