#pragma once
#include "nt.h"
#include <Windows.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdbool.h>

bool ResolveFunctions(pfnNtQuerySystemInformation* NtQuerySystemInformation, pfnNtQueryObject* NtQueryObject);

BOOL GetSystemHandleInformation(pfnNtQuerySystemInformation NtQuerySystemInformation, void** buffer, ULONG* out_size);

wchar_t* GetHandleName(pfnNtQueryObject NtQueryObject, HANDLE dup);

BOOL PatchHandle(
	HANDLE hProcess,
	DWORD ProcessId,
	const SYSTEM_HANDLE_TABLE_ENTRY_INFO* entry,
	pfnNtQueryObject NtQueryObject,
	BOOL dbg
);
