#pragma once

#include <list>
#include <vector>
#include <afxcmn.h>
#include "Common.h"

using namespace std;

class CListProcess
{
public:
	CListProcess();
	~CListProcess();
	BOOL EnumProcess(vector<PROCESS_INFO> &vectorProcess);
	BOOL EnumProcessByR3(list<ULONG> &vectorProcess);
	CString GetProcessCommandLine(HANDLE dwPid);

	void SortByProcessCreateTime(vector<PROCESS_INFO> &ProcListEx);
	BOOL KillProcess(HANDLE nPid);
	BOOL SuspendProcess(HANDLE nPid);
	BOOL ResumeProcess(HANDLE nPid);

};

