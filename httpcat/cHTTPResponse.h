#pragma once
#include "HttpCat.h"
#include "cString.h"
#include "HTTPConstants.h"
#include "SRDFUtils.h"

class cHTTPResponse
{
	CHAR*	HTTPVersion;
	UINT	StatusCode;
	CHAR*	StatusCodeStr;

	VOID	AddHashedParams(cString* Buffer);
	VOID	GenerateHeader(CHAR** Buffer, UINT* HeaderSize);
	CHAR*	GetStatusCodeStr(UINT StatusCode);
	VOID	AddPayloadToBuffer(CHAR** Buffer, UINT* BufferSize);

	cString*	TmpBuffer;
	UINT	TmpBufferSize;

	CHAR*	DateBuffer;
	time_t	rawtime;
	struct tm  timeinfo;
public:
	cHTTPResponse(void);
	~cHTTPResponse(void);

	VOID	SetPayload(CHAR* PayloadBuffer, UINT PayloadSize);
	VOID	GetBuffer(CHAR** Buffer, UINT*	BufferSize);
	VOID	SetStatusCode(UINT StatusCode);
	VOID	SetHTTPVersion(CHAR* HTTPVersion);

	UINT	HeaderSize;
	CHAR*	HeaderBuffer;
	UINT	PayloadSize;
	CHAR*	PayloadBuffer;

	cHash*	ResponseParams;
};

