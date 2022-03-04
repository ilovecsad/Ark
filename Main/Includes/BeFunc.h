#pragma once
#include <stdio.h>
#include <Windows.h>
#define BEA_ENGINE_STATIC  // 指明使用静态Lib库
#define BEA_USE_STDCALL    // 指明使用stdcall调用约定

extern "C"
{
#include "BeaEngine.h"
#pragma comment(lib, "BeaEngine.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
}

//例子 ：：
void DisassembleCode(char *start_offset, int size);
void DisassembleCodeByte(BYTE *ptr, int len);
void DisassembleCodeInstr(char *start_offset, char *end_offset, ULONG64 virtual_address);
VOID testBeDemo1();
VOID testBeDemo2();
VOID testBeDemo3();
VOID testBeDemo4();
VOID testBeDemo5();
VOID testBeDemo6();