#include "BeFunc.h"

void DisassembleCode(char *start_offset, int size)
{
	DISASM Disasm_Info;
	int len;
	char *end_offset = (char*)start_offset + size;
	(void)memset(&Disasm_Info, 0, sizeof(DISASM));
	Disasm_Info.EIP = (UInt64)start_offset;
	Disasm_Info.Archi = 1;
	Disasm_Info.Options = MasmSyntax;
	while (!Disasm_Info.Error)
	{
		Disasm_Info.SecurityBlock = (UInt64)end_offset - Disasm_Info.EIP;
		if (Disasm_Info.SecurityBlock <= 0)
			break;
		len = Disasm(&Disasm_Info);
		switch (Disasm_Info.Error)
		{
		case OUT_OF_BLOCK:
			break;
		case UNKNOWN_OPCODE:
			printf("%s \n", &Disasm_Info.CompleteInstr);
			Disasm_Info.EIP += 1;
			Disasm_Info.Error = 0;
			break;
		default:
			printf("%s \n", &Disasm_Info.CompleteInstr);
			Disasm_Info.EIP += len;
		}
	}
}


// 反汇编字节数组
void DisassembleCodeByte(BYTE *ptr, int len)
{

	DISASM Disasm_Info;
	char *end_offset = (char*)ptr + 10;
	(void)memset(&Disasm_Info, 0, sizeof(DISASM));
	Disasm_Info.EIP = (UInt64)ptr;
	Disasm_Info.Archi = 1;                      // 1 = 表示反汇编32位 / 0 = 表示反汇编64位
	Disasm_Info.Options = MasmSyntax;           // 指定语法格式 MASM

	while (!Disasm_Info.Error)
	{
		Disasm_Info.SecurityBlock = (UInt64)end_offset - Disasm_Info.EIP;
		if (Disasm_Info.SecurityBlock <= 0)
			break;
		len = Disasm(&Disasm_Info);
		switch (Disasm_Info.Error)
		{
		case OUT_OF_BLOCK:
			break;
		case UNKNOWN_OPCODE:
			Disasm_Info.EIP += 1;
			Disasm_Info.Error = 0;
			break;
		default:
			printf("%s \n", &Disasm_Info.CompleteInstr);
			Disasm_Info.EIP += len;
		}
	}
}
//反汇编时，显示虚拟地址
void DisassembleCodeInstr(char *start_offset, char *end_offset, ULONG64 virtual_address)
{
	DISASM Disasm_Info;
	int len;
	(void)memset(&Disasm_Info, 0, sizeof(DISASM));
	Disasm_Info.EIP = (UINT64)start_offset;
	Disasm_Info.VirtualAddr = (UINT64)virtual_address;
	Disasm_Info.Archi = 0;
	Disasm_Info.Options = MasmSyntax;

	while (!Disasm_Info.Error)
	{
		Disasm_Info.SecurityBlock = (UInt64)end_offset - Disasm_Info.EIP;
		if (Disasm_Info.SecurityBlock <= 0)
			break;
		len = Disasm(&Disasm_Info);
		switch (Disasm_Info.Error)
		{
		case OUT_OF_BLOCK:
			break;
		case UNKNOWN_OPCODE:
			Disasm_Info.EIP += 1;
			Disasm_Info.VirtualAddr += 1;
			break;
		default:
			printf("%.16llx > %s\n", Disasm_Info.VirtualAddr, &Disasm_Info.CompleteInstr);
			Disasm_Info.EIP += len;
			Disasm_Info.VirtualAddr += len;
		}
	}
}


VOID testBeDemo1()
{
	char *buffer = "\x55\x8b\xec\x81\xec\x24\x03\x00\x00\x6a\x17";
	DisassembleCode(buffer, 11);
	BYTE bTest[] = { 0x68, 0x37, 0x31, 0x40, 0x00, 0xFF, 0x15, 0x0C, 0x20, 0x40 };
	DisassembleCode(buffer, 14);
	/*
push ebp
mov ebp, esp
sub esp, 00000324h
push 00000017h
push ebp
mov ebp, esp
sub esp, 00000324h
push 00000017h
add byte ptr [eax], al
	*/

}

VOID testBeDemo2()
{
	/*
	push ebp
mov ebp, esp
sub esp, 00000324h
*/
	BYTE bTest[] = { 0x55, 0x8b, 0xec, 0x81, 0xec, 0x24, 0x03, 0x00, 0x00, 0x6a, 0x17 };
	DisassembleCodeByte(bTest, 10);
	return VOID();
}

VOID testBeDemo3()
{
	void *pBuffer = malloc(200);
	memcpy(pBuffer, OpenProcess, 200);
	DisassembleCodeInstr((char *)pBuffer, (char *)pBuffer + 200, (ULONG64)OpenProcess);
	system("pause");
	return VOID();
}
