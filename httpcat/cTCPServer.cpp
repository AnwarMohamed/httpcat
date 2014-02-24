#include "cTCPServer.h"

cTCPServer::cTCPServer(DWORD IP, UINT Port, UINT nWorkerThreads, UINT MaxConcurrentConnections)
{
    isInitialised = FALSE;
    isSocketUp = FALSE;
    areWorkerThreadsUp = FALSE;
    isIOComPortUp = FALSE;
    areConnectionsUp = FALSE;
	isReady = FALSE;

    this->nWorkerThreads = nWorkerThreads;
    this->MaxConcurrentConnections = MaxConcurrentConnections;

    INT error = WSAStartup (0x0202, &wsadata);
    if (error)	return;

    printf(	"WSAStartup\n" "  Version: %d\n" "  Description: %s\n" "  Status: %s\n",
            wsadata.wVersion, wsadata.szDescription, wsadata.szSystemStatus);

    isInitialised = TRUE;

    if (wsadata.wVersion != 0x0202)
    {
        printf(	"Invalid winsock version\n");
        Shutdown();
        return;
    }

    IOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, nWorkerThreads);

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_addr.s_addr = htonl(IP);	

    ServerSocket = WSASocket( PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
    if (ServerSocket == INVALID_SOCKET)
    {
		printf("Unable to create socket\n");
        Shutdown();
        return;		
    }

	isReady = TRUE;
}

UINT WINAPI cTCPServer::WorkerThreadProcedure(void* IoPort)
{
    printf("from inside tcp workerproc\n");

    while(TRUE)
    {
        BOOL Status = 0;
        DWORD nTransferred = 0;
        ULONG_PTR CompKey = COMPLETION_KEY_NONE;
        LPOVERLAPPED pOver = 0;

        Status = GetQueuedCompletionStatus(static_cast<HANDLE>(IoPort), &nTransferred, &CompKey, &pOver,INFINITE);
        cTCPSConnection* pConn = static_cast<cTCPSConnection*>(pOver);

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

VOID cTCPServer::CreateWorkerThreads(UINT WorkerThreadsCount)
{
    WorkerThreads = new HANDLE[WorkerThreadsCount];
    ZeroMemory(WorkerThreads, sizeof(HANDLE)* WorkerThreadsCount);

    WorkerThreadsIds = new UINT[WorkerThreadsCount];
    ZeroMemory(WorkerThreadsIds, sizeof(UINT)* WorkerThreadsCount);

	CreateWorkerThreadsProc(WorkerThreadsCount);
    areWorkerThreadsUp = TRUE;
}

VOID cTCPServer::CreateWorkerThreadsProc(UINT WorkerThreadsCount)
{
	for (UINT i=0; i<WorkerThreadsCount; i++)
        WorkerThreads[i] = (HANDLE)_beginthreadex(0, 0, WorkerThreadProcedure, IOCompletionPort, 0, &WorkerThreadsIds[i]);
}

BOOL cTCPServer::StartListening()
{
    if (!isReady) return FALSE;
    if (bind(ServerSocket, (LPSOCKADDR)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR ||
        listen(ServerSocket, DEFAULT_LISTEN_QUEUE_SIZE) == SOCKET_ERROR)
    {
        Shutdown();
        return FALSE;
    }
    isSocketUp = TRUE;

    CreateWorkerThreads(nWorkerThreads);
    CreateIoCompletionPort( (HANDLE)ServerSocket, IOCompletionPort, COMPLETION_KEY_IO, 0 );
    isIOComPortUp = TRUE;

    Connections = (cTCPSConnection**) malloc(sizeof(cTCPSConnection*) * MaxConcurrentConnections);
    ZeroMemory(Connections, sizeof(cTCPSConnection*) * MaxConcurrentConnections);

	CreateConnections(ServerSocket, IOCompletionPort, MaxConcurrentConnections);
    areConnectionsUp = TRUE;
  
    return TRUE;
}

VOID cTCPServer::CreateConnections(SOCKET ServerSocket, HANDLE IOCompletionPort, UINT MaxConcurrentConnections)
{
	for (UINT i=0; i<MaxConcurrentConnections; i++)
        Connections[i] = new cTCPSConnection(ServerSocket, IOCompletionPort);
}

VOID cTCPServer::Shutdown()
{
    if (isReady) 
    {
        UINT i;

        if (isIOComPortUp)
            for (i=0; i<nWorkerThreads; i++)
                PostQueuedCompletionStatus(IOCompletionPort, 0, COMPLETION_KEY_SHUTDOWN, 0);

        if (areWorkerThreadsUp)
        {
            WaitForMultipleObjects(nWorkerThreads, WorkerThreads, TRUE, INFINITE);
            for (i=0; i<nWorkerThreads; i++)
                CloseHandle(WorkerThreads[i]);
            delete[] WorkerThreads;
            delete[] WorkerThreadsIds;
            areWorkerThreadsUp = FALSE;
        }

        if (isSocketUp)
        {
            shutdown(ServerSocket, SD_BOTH);
            closesocket(ServerSocket);
            isSocketUp = FALSE;
        }

        if (isIOComPortUp)
        {
            CloseHandle(IOCompletionPort);
            isIOComPortUp = FALSE;
        }

        if (areConnectionsUp)
        {
            for ( i=0; i<MaxConcurrentConnections; i++)
                delete(Connections[i]);
            free(Connections);
            areConnectionsUp = FALSE;
        }

        isInitialised = FALSE;
		isReady = FALSE;
    }

	if (isInitialised) {
		WSACleanup();
		isInitialised = FALSE;
	}
}

cTCPServer::~cTCPServer()
{
    if (isReady)
        Shutdown();
}
