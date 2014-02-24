#pragma once
#include "SRDFUtils.h"
#include "cHTTPResponse.h"
#include <regex>

#define HTTP_VERB_GET		"GET"
#define	HTTP_VERB_POST		"POST"
#define	HTTP_VERB_HEAD		"HEAD"
#define	HTTP_VERB_CONNECT	"CONNECT"		//for proxy

class cHTTPRequest
{

private:
	VOID	CommonInit(cHash* Params);
	UINT	ParseInput(CHAR* InputBuffer, UINT nInputBuffer);
	
	CHAR*	TempBuffer;

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
	CHAR*	RequestVerb;
	CHAR*	RequestHTTPVersion;

	cHash*	RequestParams;
};
