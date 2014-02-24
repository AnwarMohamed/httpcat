#pragma once
#include "HttpCat.h"
#include "cString.h"
#include "SRDFUtils.h"

#define HTTP_OK						200
#define HTTP_METHOD_NOT_ALLOWED		405
#define HTTP_VERSION_NOT_SUPPORTED	505

#define HTTP_DEFAULT_VERSION	"1.0"

#define HTTP_RESPONSE_CONNECTION		"Connection"
#define HTTP_RESPONSE_DATE				"Date"
#define HTTP_RESPONSE_CONTENT_LENGTH	"Content-Length"

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

