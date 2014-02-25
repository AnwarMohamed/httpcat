#include "cHTTPHandler.h"
#include "cUtils.h"

cHTTPHandler::cHTTPHandler(USHORT Mode)
{
	TempRequest = NULL;
	TempResponse = NULL;
}

cHTTPHandler::~cHTTPHandler()
{
	if (TempResponse != NULL) 
		delete TempResponse;

	if (TempRequest != NULL) 
		delete TempRequest;
}

VOID cHTTPHandler::ParseRequest(CHAR* RecievedData, UINT nRecievedData)
{
	if (TempRequest != NULL) 
		delete TempRequest;

	TempRequest = new cHTTPRequest(RecievedData, nRecievedData);

	ProcessRequest();
}

VOID cHTTPHandler::ProcessRequest()
{
	if (TempResponse != NULL) 
		delete TempResponse;

	TempResponse = new cHTTPResponse();

	if (TempRequest->isValid)
	{
		TempResponse->SetHTTPVersion(TempRequest->RequestHTTPVersion);
	}
	else 
		TempResponse->SetStatusCode(TempRequest->ErrorCode);
}

VOID cHTTPHandler::PutResponse(CHAR** ReadyData, UINT* nReadyData)
{
	TempResponse->GetBuffer(ReadyData, nReadyData);
}	

BOOL cHTTPHandler::IsRequestComplete(CHAR* RecievedData, UINT nRecievedData, UINT nBytesRead)
{
	// Valid HTTP requests are supposed to end with \r\n\r\n
	const unsigned char Term[] = { '\r', '\n', '\r', '\n' };

	UINT LastSize = nRecievedData - nBytesRead;
	UINT Start = ( LastSize > 3 ) ? ( LastSize - 3 ) : 0;
	return ( 0 != memscan((UCHAR*)RecievedData + Start, nRecievedData - Start, Term, sizeof(Term) ));
}