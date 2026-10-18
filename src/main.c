#include <Windows.h>
#include "process.h"
#include "core.h"
#include "nt.h"
#include <stdio.h>
#include <stdbool.h>

void green() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(
		console,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY
	);
}

void red() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(
		console,
		FOREGROUND_RED | FOREGROUND_INTENSITY
	);
}

void blue() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(
		console,
		FOREGROUND_BLUE | FOREGROUND_INTENSITY
	);
}

int wmain(int argc, wchar_t* argv[]) {
	BOOL dbg = TRUE;
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
		red();
		wprintf(L"[-] failed to resolve nt functions for multi-instance.\n");

		wprintf(L"\n[+] press any key to leave...\n");
		getwchar();
		return 1;
	}
	green();
	wprintf(L"[+] successfully resolved nt functions.\n");

	ProcessId = GetRobloxPID();
	if (ProcessId == 0) {
		red();
		wprintf(L"[-] failed to find roblox process, try opening it.\n");

		wprintf(L"\n[+] press any key to leave...\n");
		getwchar();
		return 2;
	}

	green();
	wprintf(L"[+] found roblox process.\n");

	HANDLE hProcess = OpenProcess(
		PROCESS_DUP_HANDLE,
		FALSE,
		ProcessId
	);
	if (!hProcess) {
		red();
		wprintf(L"[-] failed to attach to roblox process at pid: %lu.\n", ProcessId);

		wprintf(L"\n[+] press any key to leave...\n");
		getwchar();
		return 3;
	}
	green();
	wprintf(L"[+] attached to roblox process safely.\n");

	void* buffer = NULL;
	ULONG buffer_size = 0;
	
	if (!GetSystemHandleInformation(ntsystem, &buffer, &buffer_size)) {
		CloseHandle(hProcess);
		red();
		wprintf(L"[-] failed to get system handle information, code: %lu,\n", GetLastError());

		wprintf(L"\n[+] press any key to leave...\n");
		getwchar();
		return 4;
	}
	green();
	wprintf(L"[+] found system handle information.\n");

	PSYSTEM_HANDLE_INFORMATION info = (PSYSTEM_HANDLE_INFORMATION)buffer;
	BOOL closed = FALSE;

	for (ULONG i = 0; i < info->NumberOfHandles; i++) {
		SYSTEM_HANDLE_TABLE_ENTRY_INFO* entry = &(info->Handles[i]);
		blue();
		if (PatchHandle(hProcess, ProcessId, entry, ntobject, dbg)) {
			closed = TRUE;
			break;
		}
	}

	if (!closed) {
		red();
		wprintf(L"[-] couldn't close handle.\n");
		free(buffer);
		CloseHandle(hProcess);

		wprintf(L"\n[+] press any key to leave...\n");
		getwchar();

		return 5;
	}

	green();
	wprintf(L"[+] successful. try opening another roblox process.\n");

	wprintf(L"\n[+] press any key to leave...\n");
	getwchar();

	free(buffer);
	CloseHandle(hProcess);

	return 0;
}
