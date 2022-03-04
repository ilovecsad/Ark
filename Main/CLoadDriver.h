#pragma once
#include <afxcmn.h>
#include <Windows.h>
#include <winsvc.h>
#define CTL_CODE( DeviceType, Function, Method, Access ) ( ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#define FILE_DEVICE_UNKNOWN             0x00000022
#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3
#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

using namespace std;

class CLoadDriver
{

#define  ·ûºÅÃû L"\\\\.\\HZW"
private:

	HANDLE m_hDevice;

public:

	BOOLEAN WINAPI Load(CString &DriverPatch, CString &DriverName);
	BOOLEAN WINAPI UnLoad(CString &lpName);
	BOOLEAN WINAPI DeviceControl(DWORD ControlCode, PVOID64 lpInBuffer, IN DWORD InBufferSize, OUT PVOID64 lpOutBuffer, DWORD OutBufferSize, LPDWORD lpBytesReturned);

};


