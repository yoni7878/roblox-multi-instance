#include <Windows.h>
#include "process.h"
#include "core.h"
#include "nt.h"
#include <stdio.h>
#include <stdbool.h>

int wmain(int argc, wchar_t* argv[]) {
	BOOL dbg = FALSE;
	if (argc > 1) {
		if (wcscmp(argv[1], L"--debug") == 0) {
			dbg = TRUE;
		}
	}
	pfnNtQuerySystemInformation ntsystem = NULL;
	pfnNtQueryObject ntobject = NULL;
	DWORD ProcessId = 0;
	wprintf(L"[roblox multi instance].\n");

	if (!ResolveFunctions(&ntsystem, &ntobject)) {
		wprintf(L"[-] failed to resolve nt functions for multi-instance.\n");
		return 1;
	}
	wprintf(L"[+] successfully resolved nt functions.\n");

	ProcessId = GetRobloxPID();
	if (ProcessId == 0) {
		wprintf(L"[-] failed to find roblox process, try opening it.\n");
		return 2;
	}

	wprintf(L"[+] found roblox process.\n");

	HANDLE hProcess = OpenProcess(
		PROCESS_DUP_HANDLE,
		FALSE,
		ProcessId
	);
	if (!hProcess) {
		wprintf(L"[-] failed to attach to roblox process at pid: %lu.\n", ProcessId);
		return 3;
	}
	wprintf(L"[+] attached to roblox process safely.\n");

	void* buffer = NULL;
	ULONG buffer_size = 0;
	
	if (!GetSystemHandleInformation(ntsystem, &buffer, &buffer_size)) {
		CloseHandle(hProcess);
		wprintf(L"[-] failed to get system handle information, code: %lu,\n", GetLastError());
		return 4;
	}
	wprintf(L"[+] found system handle information.\n");

	PSYSTEM_HANDLE_INFORMATION info = (PSYSTEM_HANDLE_INFORMATION)buffer;
	BOOL closed = FALSE;

	for (ULONG i = 0; i < info->NumberOfHandles; i++) {
		SYSTEM_HANDLE_TABLE_ENTRY_INFO* entry = &(info->Handles[i]);

		if (PatchHandle(hProcess, ProcessId, entry, ntobject, dbg)) {
			closed = TRUE;
			break;
		}
	}

	if (!closed) {
		wprintf(L"[-] couldn't close handle.\n");
		free(buffer);
		CloseHandle(hProcess);

		return 5;
	}

	wprintf(L"[+] successful. try opening another roblox process.\n");

	free(buffer);
	CloseHandle(hProcess);

	return 0;
}
