#include "cHTTPSConnection.h"
#include "cHTTPHandler.h"

cHTTPSConnection::cHTTPSConnection(SOCKET Listener, HANDLE IoPort) : cTCPSConnection(Listener, IoPort)
{ 
	HTTPHandler = new cHTTPHandler(HTTP_HANDLER_SERVER);
}

cHTTPSConnection::~cHTTPSConnection()
{
	delete HTTPHandler;
}

VOID cHTTPSConnection::ReadDataProc()
{
	HTTPHandler->ParseRequest(RecievedData, nRecievedData);
	HTTPHandler->PutResponse(&ReadyData, &nReadyData);

	TransmitBuffers.Head = ReadyData;
	TransmitBuffers.HeadLength = nReadyData;
	TransmitFile(ClientSocket, 0, 0, 0, this, &TransmitBuffers, 0 );
}

BOOL cHTTPSConnection::IsRequestComplete(UINT nBytesRead)
{
	return HTTPHandler->IsRequestComplete(RecievedData, nRecievedData, nBytesRead);
}