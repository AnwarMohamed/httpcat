#include "cHTTPRequest.h"
#include <string>

using namespace std;



cHTTPRequest::cHTTPRequest(cHash* Params)
{
	CommonInit(Params);
}

cHTTPRequest::cHTTPRequest(CHAR* InputBuffer, UINT nInputBuffer)
{
	CommonInit(NULL);
	ErrorCode = ParseInput(InputBuffer, nInputBuffer);
}

VOID cHTTPRequest::CommonInit(cHash* Params)
{
	isValid = FALSE;
	if (Params == NULL) 
		this->RequestParams = new cHash();
	else this->RequestParams = Params;

	RequestVerb = NULL;
	RequestURI = NULL;
	RequestHTTPVersion = NULL;

	TempBuffer = (CHAR*)malloc(0);
	HeaderBuffer = NULL;
	HeaderSize = 0;
	PayloadBuffer = NULL;
	PayloadSize = 0;
	ErrorCode = 0;
}

UINT cHTTPRequest::ParseInput(CHAR* InputBuffer, UINT nInputBuffer)
{
	cmatch RegxResult; 
	UINT InputBufferOffset = 0;

	if (regex_search(InputBuffer, RegxResult, regex("^(.*?)\\s(.*?)\\sHTTP/(.*?)\\r\\n")))
	{
		InputBufferOffset += RegxResult[0].str().size();
		if (strcmp(RegxResult[1].str().c_str(), HTTP_VERB_GET) == 0)
		{
			RequestVerb = HTTP_VERB_GET;
		}
		else if (strcmp(RegxResult[1].str().c_str(), HTTP_VERB_POST) == 0)
		{
			RequestVerb = HTTP_VERB_POST;
		}
		else if (strcmp(RegxResult[1].str().c_str(), HTTP_VERB_HEAD) == 0)
		{
			RequestVerb = HTTP_VERB_HEAD;
		}

		else if (strcmp(RegxResult[1].str().c_str(), HTTP_VERB_CONNECT) == 0)
		{
			RequestVerb = HTTP_VERB_CONNECT;
		}
		else return HTTP_METHOD_NOT_ALLOWED;
	}

	RequestHTTPVersion = new CHAR[RegxResult[3].str().size() + 1];
	RequestHTTPVersion[RegxResult[3].str().size()] = 0;
	memcpy(RequestHTTPVersion, RegxResult[3].str().c_str(), RegxResult[3].str().size());

	if (strcmp(RequestHTTPVersion, "1.1") != 0 && strcmp(RequestHTTPVersion, "1.0") != 0)
		return HTTP_VERSION_NOT_SUPPORTED;

	RequestURI = new CHAR[RegxResult[2].str().size() + 1];
	RequestURI[RegxResult[2].str().size()] = 0;
	memcpy(RequestURI, RegxResult[2].str().c_str(), RegxResult[2].str().size());

	while(TRUE)
	{
		if (!regex_search(InputBuffer + InputBufferOffset, RegxResult, 
			regex("^(.*?):\\s(.*?)\\r\\n"))) break;

		InputBufferOffset += RegxResult[0].str().size();
		RequestParams->AddItem(RegxResult[1].str().c_str(), RegxResult[2].str().c_str());
	}

	InputBufferOffset+=2;

	HeaderBuffer = InputBuffer;
	HeaderSize = nInputBuffer >= InputBufferOffset ? InputBufferOffset: nInputBuffer;

	PayloadSize = nInputBuffer - HeaderSize;
	PayloadBuffer = PayloadSize > 0 ? HeaderBuffer + HeaderSize: NULL;
	
	isValid = TRUE;
	return 0;
}

VOID cHTTPRequest::GetBuffer(CHAR** Buffer, UINT* BufferSize)
{

}

cHTTPRequest::~cHTTPRequest()
{
	if (RequestURI != NULL) delete[] RequestURI;
	if (RequestHTTPVersion != NULL) delete[] RequestHTTPVersion;
	free(TempBuffer);
}
