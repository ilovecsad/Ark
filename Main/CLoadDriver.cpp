#pragma once
#include "pch.h"
#include "CLoadDriver.h"



BOOLEAN WINAPI CLoadDriver::Load(CString &DriverPatch, CString &DriverName)
{


	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hService = CreateService(hSCManager, DriverName,
		DriverName, SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE, DriverPatch, NULL, NULL, NULL, NULL, NULL);



	if (hService == 0)
	{
		hService = OpenService(hSCManager, DriverName, SERVICE_ALL_ACCESS);
	}
	bool boole = StartService(hService, 0, NULL);


	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	if (!boole)
	{
		if (GetLastError() == 1056)//驱动已经运行
		{

			return FALSE;
		}
	}

	m_hDevice = CreateFile(符号名,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		UnLoad(DriverName);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
BOOLEAN WINAPI CLoadDriver::UnLoad(CString &lpName)
{
	CloseHandle(m_hDevice);
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hService = OpenService(hSCManager, lpName, SERVICE_ALL_ACCESS);

	SERVICE_STATUS ss;
	ControlService(hService, SERVICE_CONTROL_STOP, &ss);
	BOOLEAN boole = DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return boole;
}

//CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
BOOLEAN WINAPI CLoadDriver::DeviceControl(DWORD ControlCode, PVOID64 lpInBuffer, IN DWORD InBufferSize, OUT PVOID64 lpOutBuffer, DWORD OutBufferSize, LPDWORD lpBytesReturned)
{
	return	DeviceIoControl(m_hDevice, ControlCode, lpInBuffer, InBufferSize, lpOutBuffer, OutBufferSize, lpBytesReturned, 0);
}