#pragma once
#include <vector>
#include "CLoadDriver.h"
#include "Common.h"
using namespace std;

class CCallBack
{
public:
	CCallBack();
	~CCallBack();

	BOOL EnumAllCallBack(PCALL_BACK *p);

};

