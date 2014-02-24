#include "cHTTPResponse.h"
#include <stdio.h>
#include "cString.h"
#include <time.h>

cHTTPResponse::cHTTPResponse()
{
	HTTPVersion = NULL;
	StatusCode = 0;
	TmpBuffer = NULL;
	DateBuffer = NULL;
	ResponseParams = new cHash();

	//tmp
	HeaderSize = 0;
	HeaderBuffer = 0;
	PayloadSize = 0;
	PayloadBuffer = 0;
}

CHAR* cHTTPResponse::GetStatusCodeStr(UINT StatusCode)
{
	switch(StatusCode)
	{
	// 1xx Informational

	// 2xx Success
	case(HTTP_OK):
		return "200 OK";

	// 3xx Redirection

	// 4xx Client Error
	case(HTTP_METHOD_NOT_ALLOWED):
		return "405 Method Not Allowed";

	// 5xx Server Error
	case(HTTP_VERSION_NOT_SUPPORTED):
		return "505 HTTP Version Not Supported";

	default:
		return "501 Not Implemented";
	}
}

VOID cHTTPResponse::GenerateHeader(CHAR** Buffer, UINT* HeaderSize)
{
	if (TmpBuffer != NULL) delete TmpBuffer;
	TmpBuffer = new cString();
	TmpBufferSize = 0;

	// http reply
	*TmpBuffer = "HTTP/";
	*TmpBuffer += HTTPVersion != NULL ? HTTPVersion: HTTP_DEFAULT_VERSION;
	*TmpBuffer += " ";
	*TmpBuffer += GetStatusCodeStr(this->StatusCode);
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

VOID cHTTPResponse::AddHashedParams(cString* Buffer)
{
	DateBuffer = new CHAR[80];
	time (&rawtime);
	gmtime_s(&timeinfo, &rawtime);

	strftime (DateBuffer, 80, "%a, %d %b %Y %X GMT", &timeinfo);

	if (ResponseParams->nItems == 0)
	{
		ResponseParams->AddItem(HTTP_RESPONSE_DATE, DateBuffer);
		ResponseParams->AddItem(HTTP_RESPONSE_CONTENT_LENGTH, cString(PayloadSize));
		ResponseParams->AddItem(HTTP_RESPONSE_CONNECTION, "close");
	}
	else
	{
		if (!ResponseParams->IsFound(HTTP_RESPONSE_DATE))
			ResponseParams->AddItem(HTTP_RESPONSE_DATE, DateBuffer);
		if (!ResponseParams->IsFound(HTTP_RESPONSE_CONTENT_LENGTH))
			ResponseParams->AddItem(HTTP_RESPONSE_CONTENT_LENGTH, cString(PayloadSize));
		if (!ResponseParams->IsFound(HTTP_RESPONSE_CONNECTION))
			ResponseParams->AddItem(HTTP_RESPONSE_CONNECTION, "close");
	}

	for (UINT i=0; i<ResponseParams->nItems; i++)
	{
		(*Buffer) += ResponseParams->GetKey(i);
		(*Buffer) += ": ";
		(*Buffer) += ResponseParams->GetValue(ResponseParams->GetKey(i));
		(*Buffer) += "\r\n";
	}
}

VOID cHTTPResponse::GetBuffer(CHAR** Buffer, UINT* BufferSize)
{
	GenerateHeader(Buffer, BufferSize);
	AddPayloadToBuffer(Buffer, BufferSize);
}

VOID cHTTPResponse::AddPayloadToBuffer(CHAR** Buffer, UINT* BufferSize)
{
	if (PayloadBuffer != NULL && PayloadSize != 0)
	{
		*Buffer = (CHAR*)realloc(*Buffer, *BufferSize + PayloadSize);
		memcpy(*Buffer + *BufferSize, PayloadBuffer, PayloadSize);
		*BufferSize += PayloadSize;
	}
}

VOID cHTTPResponse::SetStatusCode(UINT StatusCode)
{
	this->StatusCode = StatusCode;
}

VOID cHTTPResponse::SetHTTPVersion(CHAR* HTTPVersion)
{
	this->HTTPVersion = HTTPVersion;
}

VOID cHTTPResponse::SetPayload(CHAR* PayloadBuffer, UINT PayloadSize)
{
	this->PayloadBuffer = PayloadBuffer;
	this->PayloadSize = PayloadSize;
}

cHTTPResponse::~cHTTPResponse()
{
	delete ResponseParams;
	if (TmpBuffer != NULL) 
		delete TmpBuffer;
	if (DateBuffer != NULL)
		delete[] DateBuffer;
}
