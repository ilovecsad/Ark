#include "CallBack.h"

extern CLoadDriver g_connectDriver;
CCallBack::CCallBack()
{
}

CCallBack::~CCallBack()
{
}

BOOL CCallBack::EnumAllCallBack(PCALL_BACK * p)
{

	if (!p)return FALSE;

	BOOL bRet = FALSE;

	typedef struct _INPUT
	{
		PCALL_BACK  pAddress;
	}Input;

	Input inputs = { 0 };

	PCALL_BACK  pTemp = NULL;

	int n = sizeof(CALL_BACK);
	pTemp = (PCALL_BACK)malloc(sizeof(CALL_BACK));
	if (!pTemp)return FALSE;

	RtlZeroMemory(pTemp, sizeof(CALL_BACK));

	inputs.pAddress = pTemp;



	bRet = g_connectDriver.DeviceControl(IOCTL_ENUM_CALLBACK, &inputs, sizeof(Input),0, 0, 0);

	if (bRet) {
		*p = pTemp;
		pTemp = NULL;
	}

	return bRet;
}
