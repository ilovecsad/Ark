#pragma once
#include <vector>
#include <algorithm>
#include "Function.h"
#include "Common.h"
using namespace std;
class CExternTool
{
public:
	BOOL SetLoadImageNotifyRoutine();

	BOOL StatrInject(PVOID pProcessName, PVOID pDllPath, ULONG dwStats);

};

