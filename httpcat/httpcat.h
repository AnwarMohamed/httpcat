#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "SRDFUtils.h"

using namespace std;

struct COMMAND;
class cSession;
class cMutex;
class cThread;

typedef cSession* ExecuteCommand(COMMAND* command, cHash* params);
typedef ExecuteCommand *PExecuteCommand;

struct BINARY
{
	char* buffer;
	char* size;
};

struct COMMAND
{
	char* cmd;
	char* help;
	DWORD nParams;
	PExecuteCommand CreateFunction;
	char* LastError;
};

class cCommChannel		//To be continued
{
public:
	cCommChannel();
	~cCommChannel();
};

class cSession
{
	BINARY Output;
	BINARY Input;
	bool is_active;
	cMutex* InputMutex;
	cMutex* OutputMutex;
	cCommChannel* Channel;
public:
	cSession(cCommChannel* Channel);
	~cSession();
	virtual void Run();
	void SetInput();
	BINARY* GetOutput();
	bool IsActive();
};

class cVictim
{
public:
	cString Info;
	cCommChannel* CommChannel;
	array<cSession*> Sessions;
	cVictim(cString Info,cCommChannel CommChannel);
	~cVictim();
	void AddSession(cSession* Session);
};

class cSessionManager
{
	array<cVictim*> Victims;
	cCommChannel* MainChannel;
	cThread* ControlThread;
	cThread* SessionsThread;
public:
	array<COMMAND*> commands;
	cSessionManager();
	~cSessionManager();
	void RunSessions();				//For 2nd Thread
	void ControlSession();			//For 1st Thread
};

