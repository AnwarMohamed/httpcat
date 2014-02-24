#include "cHTTPServer.h"
#include "cHTTPSConnection.h"

cHTTPServer::cHTTPServer(DWORD IP, UINT Port, UINT nWorkerThreads, UINT MaxConcurrentConnections): 
	cTCPServer(IP, Port, nWorkerThreads, MaxConcurrentConnections) { }
cHTTPServer::~cHTTPServer() { }

VOID cHTTPServer::CreateWorkerThreadsProc(UINT WorkerThreadsCount)
{
	for (UINT i=0; i<WorkerThreadsCount; i++)
        WorkerThreads[i] = (HANDLE)_beginthreadex(0, 0, WorkerThreadProcedure, IOCompletionPort, 0, &WorkerThreadsIds[i]);
}

VOID cHTTPServer::CreateConnections(SOCKET ServerSocket, HANDLE IOCompletionPort, UINT MaxConcurrent)
{
	for (UINT i=0; i<MaxConcurrent; i++)
        Connections[i] = new cHTTPSConnection(ServerSocket, IOCompletionPort);
}

UINT WINAPI cHTTPServer::WorkerThreadProcedure(void* IoPort)
{
    while(TRUE)
    {
        BOOL Status = 0;
        DWORD nTransferred = 0;
        ULONG_PTR CompKey = COMPLETION_KEY_NONE;
        LPOVERLAPPED pOver = 0;

        Status = GetQueuedCompletionStatus(static_cast<HANDLE>(IoPort), &nTransferred, &CompKey, &pOver,INFINITE);
        cHTTPSConnection* pConn = static_cast<cHTTPSConnection*>(pOver);

        if (!Status)
        {
            // An error occurred; reset to a known state.
            if (pConn)
                pConn->IssueReset(); 
        }
        else if (COMPLETION_KEY_IO == CompKey)
             pConn->OnIoComplete(nTransferred);
        else if (COMPLETION_KEY_SHUTDOWN == CompKey)
            break;
    }
    return 0;
}
