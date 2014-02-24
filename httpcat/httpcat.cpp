#include <Ws2tcpip.h>
#include "HttpCat.h"
#include "cHTTPServer.h"

using namespace std;

HANDLE StopEvent = 0;

// Handler for console control events.
BOOL WINAPI ConsoleCtrlHandler(DWORD Ctrl)
{
    switch (Ctrl)
    {
    case CTRL_C_EVENT:      // Falls through..
    case CTRL_CLOSE_EVENT:
        SetEvent(StopEvent);
        return TRUE;
    default:
        return FALSE;
    }
}

INT main(INT argc, CHAR** argv)
{
	// Create an event to use to synchronize the shutdown process.
    StopEvent = CreateEvent( 0, FALSE, FALSE, 0 );

    // Setup a console control handler: We stop the server on CTRL-C
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

	DWORD IPAddress;
	InetPton(AF_INET, "0.0.0.0", &IPAddress);
	cHTTPServer *server = new cHTTPServer(IPAddress, 8080, 1, 10);
	server->StartListening();

	// Wait for the user to press CTRL-C...
    WaitForSingleObject(StopEvent, INFINITE);

	// Deregister console control handler: We stop the server on CTRL-C
	SetConsoleCtrlHandler(NULL, FALSE);

	server->Shutdown();
	delete server;

	return 0;
}

VOID exitApp(VOID)
{
	printf("[*] Shutting down\n");
}