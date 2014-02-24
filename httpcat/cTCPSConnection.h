#pragma once
#include <winsock2.h>
#include <mswsock.h>

#define ACCEPT_ADDRESS_LENGTH ((sizeof( struct sockaddr_in) + 16))
#define	DEFAULT_LISTEN_QUEUE_SIZE	8
#define	DEFAULT_READ_BUFFER_SIZE	1024

enum
{
	COMPLETION_KEY_NONE         =   0,
	COMPLETION_KEY_SHUTDOWN     =   1,
	COMPLETION_KEY_IO           =   2,
};

enum STATE
{
	WAIT_ACCEPT   =   0,
	WAIT_REQUEST  =   1,
	WAIT_TRANSMIT =   2,
	WAIT_RESET    =   3,
};

class cTCPSConnection :public OVERLAPPED
{
	cTCPSConnection(const cTCPSConnection&);
	cTCPSConnection& operator=(const cTCPSConnection&);

public:

	cTCPSConnection(SOCKET Listener, HANDLE IoPort);
	virtual ~cTCPSConnection();

	VOID	IssueAccept();
	VOID	IssueReset();
	VOID	IssueTransmit();
	VOID	IssueRead();

	
	virtual BOOL	IsRequestComplete(UINT nBytesRead);

	VOID	ClearBuffers();
	VOID	OnIoComplete(DWORD nTransferred);

	VOID	CompleteReset();
	VOID	CompleteTransmit();
	VOID	CompleteRead(UINT nBytesRead);
	VOID	CompleteAccept();	

private:
	INT		SocketState;
	SOCKET	ServerSocket;

	BYTE	AddressBlock[ACCEPT_ADDRESS_LENGTH * 2];
	CHAR	ReadBuffer[DEFAULT_READ_BUFFER_SIZE];

protected:
	TRANSMIT_FILE_BUFFERS	TransmitBuffers;
	
	SOCKET	ClientSocket;

	CHAR*	RecievedData;
	UINT	nRecievedData;
	CHAR*	ReadyData;
	UINT	nReadyData;

	virtual VOID	ReadDataProc();
};