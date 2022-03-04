#pragma once
#include <ntifs.h>

NTSTATUS NtTerminateThreadBySystemThread(IN HANDLE dwThreadId, OUT PULONG PreviousSuspendCount OPTIONAL);
NTSTATUS NtSuspendThreadBySystemThread(IN HANDLE dwThreadId, OUT PULONG PreviousSuspendCount OPTIONAL);
NTSTATUS NtResumeThreadBySystemThread(IN HANDLE dwThreadId, OUT PULONG PreviousSuspendCount OPTIONAL);
