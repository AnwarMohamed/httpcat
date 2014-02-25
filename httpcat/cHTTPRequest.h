#pragma once
#include "SRDFUtils.h"
#include "HTTPConstants.h"
#include <regex>

class cHTTPRequest
{

private:
	VOID	CommonInit(cHash* Params);
	UINT	ParseInput(CHAR* InputBuffer, UINT nInputBuffer);
	
	cString*	TmpBuffer;
	UINT	TmpBufferSize;

	VOID	AddHashedParams(cString* Buffer);
	VOID	GenerateHeader(CHAR** Buffer, UINT* HeaderSize);
	VOID	AddPayloadToBuffer(CHAR** Buffer, UINT* BufferSize);

public:
	cHTTPRequest(cHash* Params);
	cHTTPRequest(CHAR* InputBuffer, UINT nInputBuffer);
	~cHTTPRequest();

	BOOL	isValid;
	UINT	ErrorCode;
	VOID	GetBuffer(CHAR** Buffer, UINT*	BufferSize);
	
	UINT	HeaderSize;
	CHAR*	HeaderBuffer;
	UINT	PayloadSize;
	CHAR*	PayloadBuffer;

	CHAR*	RequestURI;
	DWORD	RequestURISize;
	
	CHAR*	RequestVerb;
	CHAR*	RequestHTTPVersion;
	CHAR*	RequestHost;

	cHash*	RequestParams;

	BOOL	MakeRequest(CHAR* Verb, CHAR* HTTPVersion, CHAR* URI, CHAR* Host, cHash* Params, CHAR* PayloadBuffer, UINT PayloadSize);
};
