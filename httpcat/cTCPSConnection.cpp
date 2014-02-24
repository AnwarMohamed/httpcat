#include "cTCPSConnection.h"
#include "cUtils.h"

cTCPSConnection::cTCPSConnection(SOCKET Listener, HANDLE IoPort) : ServerSocket(Listener)
{
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
	hEvent = 0;
	SocketState = WAIT_ACCEPT;
	RecievedData = (CHAR*)malloc(0);
	nRecievedData = 0;
	ReadyData = (CHAR*)malloc(0);
	nReadyData = 0;

	ZeroMemory(AddressBlock, ACCEPT_ADDRESS_LENGTH*2);
	ZeroMemory(ReadBuffer, DEFAULT_READ_BUFFER_SIZE);
	ZeroMemory(&TransmitBuffers, sizeof(TRANSMIT_FILE_BUFFERS));

	ClientSocket = WSASocket( PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED );
	CreateIoCompletionPort( reinterpret_cast<HANDLE>(ClientSocket), IoPort, COMPLETION_KEY_IO, 0 );
	IssueAccept();
}

// virtual method to process incoming data
VOID cTCPSConnection::ReadDataProc() { }

// virtual method to check if request is complete
BOOL cTCPSConnection::IsRequestComplete(UINT nBytesRead) { return TRUE; }

VOID cTCPSConnection::IssueAccept()
{   
	SocketState = WAIT_ACCEPT;
	DWORD ReceiveLen = 0;
	AcceptEx(ServerSocket, ClientSocket, AddressBlock, 0, ACCEPT_ADDRESS_LENGTH, ACCEPT_ADDRESS_LENGTH, &ReceiveLen, this);
}

cTCPSConnection::~cTCPSConnection(void)
{
	shutdown(ClientSocket, SD_BOTH);
	closesocket(ClientSocket);
}

VOID cTCPSConnection::CompleteAccept()
{
	setsockopt(ClientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (CHAR*)&ServerSocket, sizeof(SOCKET) );
	IssueRead();
}

VOID cTCPSConnection::IssueRead()
{
	SocketState = WAIT_REQUEST;
	ReadFile((HANDLE)ClientSocket, ReadBuffer, DEFAULT_READ_BUFFER_SIZE, 0, (OVERLAPPED*)this);
}

VOID cTCPSConnection::CompleteRead(UINT nBytesRead)
{
	RecievedData = (CHAR*)realloc(RecievedData, (nRecievedData + nBytesRead)* sizeof(CHAR));
	memcpy( &(RecievedData[nRecievedData]), ReadBuffer, nBytesRead * sizeof(CHAR));
	nRecievedData += nBytesRead;

	if (IsRequestComplete(nBytesRead))
		IssueTransmit();
	else
	{
		if (nBytesRead)
			IssueRead();
		else
			IssueReset();
	}
}

VOID cTCPSConnection::IssueTransmit()
{
	SocketState = WAIT_TRANSMIT;
	ReadDataProc();
}

VOID cTCPSConnection::CompleteTransmit()
{
	IssueReset();
}

VOID cTCPSConnection::IssueReset()
{
	SocketState = WAIT_RESET;
	TransmitFile(ClientSocket, 0, 0, 0, this, 0, TF_DISCONNECT | TF_REUSE_SOCKET );
}

VOID cTCPSConnection::CompleteReset()
{
	ClearBuffers();
	IssueAccept();
}

VOID cTCPSConnection::ClearBuffers()
{
	ZeroMemory(AddressBlock, ACCEPT_ADDRESS_LENGTH * 2);
	ZeroMemory(ReadBuffer, DEFAULT_READ_BUFFER_SIZE);
	ZeroMemory(&TransmitBuffers, sizeof(TRANSMIT_FILE_BUFFERS));

	RecievedData = (CHAR*)realloc(RecievedData, 0);
	nRecievedData = 0;
	ReadyData = (CHAR*)realloc(ReadyData, 0);
	nReadyData = 0;
}

VOID cTCPSConnection::OnIoComplete(DWORD nTransferred)
{
	switch (SocketState)
	{
	case WAIT_ACCEPT:
		CompleteAccept();
		break;

	case WAIT_REQUEST:
		CompleteRead(nTransferred);
		break;

	case WAIT_TRANSMIT:
		CompleteTransmit();
		break;

	case WAIT_RESET:
		CompleteReset();
		break;
	}
}
