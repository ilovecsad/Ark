#pragma once
#include <vector>
#include <algorithm>
#include "Function.h"
#include "Common.h"
using namespace std;
class CListDrivers
{
public:
	CListDrivers();
	~CListDrivers();
	BOOL ListDrivers(vector<DRIVER_INFO> &vectorDrivers);
	BOOL UnLoadDriver(ULONG_PTR DriverObject);
private:
	void FixDriverPath(PDRIVER_INFO pDriverInfo);
};

/*
LoadedModuleList链表在DriverEntry里是没加锁的，直接遍历不符合规范，上生产环境碰到用户频繁加载卸载驱动的，
分分钟蓝屏给你看。这就是为什么VMP不遍历LoadedModuleList来初始化导入表非要ZwQuery一下的原因。
*/