#pragma once
#include "HttpCat.h"
#include "cHTTPRequest.h"
#include "cHTTPResponse.h"

#define HTTP_HANDLER_SERVER	0x0000
#define	HTTP_HANDLER_CLIENT	0x0001

class cHTTPHandler
{
private:
	CHAR*	RootDirectory; /*[_MAX_PATH]*/
	cHTTPRequest*	TempRequest;
	cHTTPResponse*	TempResponse;

	VOID	ProcessRequest();

public:
	cHTTPHandler(USHORT Mode);
	~cHTTPHandler();

	VOID	ParseRequest(CHAR* RecievedData, UINT nRecievedData);
	VOID	PutResponse(CHAR** ReadyData, UINT* nReadyData);	
	BOOL	IsRequestComplete(CHAR* RecievedData, UINT nRecievedData, UINT nBytesRead);

};

