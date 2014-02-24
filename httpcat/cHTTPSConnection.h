#pragma once
#include "cTCPSConnection.h"
#include "cHTTPHandler.h"

class cHTTPSConnection : public cTCPSConnection
{
private:
	cHTTPHandler* HTTPHandler;

public:
	cHTTPSConnection(SOCKET Listener, HANDLE IoPort);
	~cHTTPSConnection();

	BOOL	IsRequestComplete(UINT nBytesRead);

protected:
	VOID	ReadDataProc();
	
};

