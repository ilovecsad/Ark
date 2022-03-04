#ifndef _PE_H
#define _PE_H

#include <ntifs.h>
#include <ntddstor.h>
#include <mountdev.h>
#include <ntddvol.h>
#include <ntstrsafe.h>
#include <ntimage.h>


#define PE_ERROR_VALUE (ULONG)-1


PVOID GetPageBase(PVOID lpHeader, ULONG* Size, PVOID ptr);
 ULONG GetExportOffset(const unsigned char* FileData, ULONG FileSize, const char* ExportName);


#endif
