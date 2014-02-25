#include "cHTTPRequest.h"
#include <string>
#include <shlwapi.h>

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
	RequestURI = (CHAR*)malloc(0);
	RequestHTTPVersion = NULL;
	RequestHost = NULL;

	TmpBuffer = NULL;
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

	if (strcmp(RequestHTTPVersion, HTTP_VERSION_1_1) != 0 && strcmp(RequestHTTPVersion, HTTP_VERSION_1_1) != 0)
		return HTTP_VERSION_NOT_SUPPORTED;

	RequestURI = (CHAR*)realloc(RequestURI, RegxResult[2].str().size() + 1);
	RequestURI[RegxResult[2].str().size()] = 0;
	memcpy(RequestURI, RegxResult[2].str().c_str(), RegxResult[2].str().size());

	while(TRUE)
	{
		if (!regex_search(InputBuffer + InputBufferOffset, RegxResult, 
			regex("^(.*?):\\s(.*?)\\r\\n"))) break;

		InputBufferOffset += RegxResult[0].str().size();
		RequestParams->AddItem(RegxResult[1].str().c_str(), RegxResult[2].str().c_str());
	}

	if (!RequestParams->IsFound(HTTP_HEADER_HOST))
		return HTTP_EXPECTATION_FAILED;
	else 
		RequestHost = RequestParams->GetValue(HTTP_HEADER_HOST).GetChar();

	printf("%s\n", RequestParams->GetValue(HTTP_HEADER_HOST).GetChar());
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
	GenerateHeader(Buffer, BufferSize);
	AddPayloadToBuffer(Buffer, BufferSize);
}

cHTTPRequest::~cHTTPRequest()
{
	if (strcmp(RequestURI, HTTP_DEFAULT_URI) != 0) free(RequestURI);
	if (RequestHTTPVersion != NULL) delete[] RequestHTTPVersion;
	if (TmpBuffer != NULL) delete TmpBuffer;
}

BOOL cHTTPRequest::MakeRequest(CHAR* Verb, CHAR* HTTPVersion, 
					CHAR* URI, CHAR* Host, cHash* Params, CHAR* PayloadBuffer, UINT PayloadSize)
{
	isValid = FALSE;

	if (Verb == NULL)
		RequestVerb = HTTP_VERB_GET;
	else 
		RequestVerb = Verb;

	if (strcmp(HTTPVersion, HTTP_VERSION_1_1) != 0 && strcmp(HTTPVersion, HTTP_VERSION_1_1) != 0)
		RequestHTTPVersion = HTTP_DEFAULT_VERSION;
	else
		RequestHTTPVersion = HTTPVersion;

	if (URI == NULL || strlen(URI) > HTTP_DEFAULT_URI_MAX_LENGTH)
		RequestURI = HTTP_DEFAULT_URI;
	else 
	{
		RequestURI = (CHAR*)realloc(RequestURI, HTTP_DEFAULT_URI_MAX_LENGTH);
		ZeroMemory(RequestURI, HTTP_DEFAULT_URI_MAX_LENGTH);
        RequestURISize = HTTP_DEFAULT_URI_MAX_LENGTH - 1;
        UrlUnescape(RequestURI, URI, &RequestURISize, 0);
		RequestURISize = strlen(RequestURI);
		RequestURI = (CHAR*)realloc(RequestURI, RequestURISize + 1);
	}

	if (RequestParams != NULL)
		delete RequestParams;
	if (Params == NULL)
		RequestParams = new cHash();
	else
		RequestParams = Params;


	if (RequestParams->IsFound(HTTP_HEADER_HOST))
		RequestHost = RequestParams->GetValue(HTTP_HEADER_HOST).GetChar();
	else if (Host != NULL && strlen(RequestHost) < HTTP_DEFAULT_HOST_MAX_LENGTH)
		RequestHost = Host;
	else
	{
		ErrorCode = HTTP_EXPECTATION_FAILED;
		return FALSE;
	}


	if (strcmp(Verb, HTTP_VERB_POST) != 0)
	{
		this->PayloadBuffer = NULL;
		this->PayloadSize = 0;
	}
	else
	{
		this->PayloadBuffer = PayloadBuffer;
		this->PayloadSize = PayloadSize;
	}

	isValid = TRUE;
	return TRUE;
}

VOID cHTTPRequest::GenerateHeader(CHAR** Buffer, UINT* HeaderSize)
{
	if (TmpBuffer != NULL) delete TmpBuffer;
	TmpBuffer = new cString();
	TmpBufferSize = 0;

	// http reply
	*TmpBuffer = RequestVerb;
	*TmpBuffer += " ";
	*TmpBuffer += RequestURI;
	*TmpBuffer += " HTTP/";
	*TmpBuffer += RequestHTTPVersion;
	*TmpBuffer += "\r\n";

	// generate from params
	AddHashedParams(TmpBuffer);
	*TmpBuffer += "\r\n";

	TmpBufferSize = (*TmpBuffer).GetLength();
	*Buffer = (CHAR*)realloc(*Buffer, TmpBufferSize + 1);
	sprintf_s(*Buffer, TmpBufferSize + 1, "%s", (*TmpBuffer).GetChar());
	*HeaderSize = TmpBufferSize;
	HeaderBuffer = *Buffer;

	delete TmpBuffer;
	TmpBuffer = NULL;
	TmpBufferSize = 0;
}

VOID cHTTPRequest::AddHashedParams(cString* Buffer)
{
	if (RequestParams->nItems == 0)
	{
		RequestParams->AddItem(HTTP_HEADER_HOST, RequestHost);

		if (strcmp(RequestVerb, HTTP_VERB_POST) == 0)
			RequestParams->AddItem(HTTP_HEADER_CONTENT_LENGTH, cString(PayloadSize));

		RequestParams->AddItem(HTTP_HEADER_CONNECTION, HTTP_DEFAULT_CONNECTION);
	}
	else
	{
		if (!RequestParams->IsFound(HTTP_HEADER_HOST))
			RequestParams->AddItem(HTTP_HEADER_HOST, RequestHost);

		if (strcmp(RequestVerb, HTTP_VERB_POST) == 0)
			if (!RequestParams->IsFound(HTTP_HEADER_CONTENT_LENGTH))
				RequestParams->AddItem(HTTP_HEADER_CONTENT_LENGTH, cString(PayloadSize));

		if (!RequestParams->IsFound(HTTP_HEADER_CONNECTION))
			RequestParams->AddItem(HTTP_HEADER_CONNECTION, HTTP_DEFAULT_CONNECTION);
	}

	for (UINT i=0; i<RequestParams->nItems; i++)
	{
		(*Buffer) += RequestParams->GetKey(i);
		(*Buffer) += ": ";
		(*Buffer) += RequestParams->GetValue(RequestParams->GetKey(i));
		(*Buffer) += "\r\n";
	}
}

VOID cHTTPRequest::AddPayloadToBuffer(CHAR** Buffer, UINT* BufferSize)
{
	if (PayloadBuffer != NULL && PayloadSize != 0)
	{
		*Buffer = (CHAR*)realloc(*Buffer, *BufferSize + PayloadSize);
		memcpy(*Buffer + *BufferSize, PayloadBuffer, PayloadSize);
		*BufferSize += PayloadSize;
	}
}