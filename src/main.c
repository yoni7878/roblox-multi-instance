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
	BOOL dbg = FALSE;
	if (argc > 1) {
		if (wcscmp(argv[1], L"--debug") == 0) {
			dbg = TRUE;
		}
	}
	pfnNtQuerySystemInformation ntsystem = NULL;
	pfnNtQueryObject ntobject = NULL;

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

	DWORD proc[100];
	UINT count = GetRobloxPID(
		proc,
		100
	);

	if (count == 0) {
		red();
		wprintf(L"[-] failed to find any roblox processes.\n");
		return 6;
	}

	green();
	wprintf(L"[+] found %u roblox processes.\n", count);

	void* buffer = NULL;
	ULONG buffer_size = 0;
	
	if (!GetSystemHandleInformation(ntsystem, &buffer, &buffer_size)) {
		red();
		wprintf(L"[-] failed to get system handle information, code: %lu,\n", GetLastError());

		wprintf(L"\n[+] press any key to leave...\n");
		getwchar();
		return 4;
	}
	green();
	wprintf(L"[+] found system handle information.\n");

	PSYSTEM_HANDLE_INFORMATION info = (PSYSTEM_HANDLE_INFORMATION)buffer;
	int total = 0;

	for (int c = 0; c < count; c++) {
		DWORD current = proc[c];

		HANDLE hProcess = OpenProcess(
			PROCESS_DUP_HANDLE,
			FALSE,
			current
		);
		if (!hProcess) {
			red();
			wprintf(L"[-] failed to attach to roblox process id: %lu.\n", current);
			blue();
			continue;
		}

		for (ULONG i = 0; i < info->NumberOfHandles; i++) {
			SYSTEM_HANDLE_TABLE_ENTRY_INFO* entry = &(info->Handles[i]);
			blue();
			if (PatchHandle(hProcess, current, entry, ntobject, dbg)) {
				total++;
			}
		}
		CloseHandle(hProcess);
	}

	if (total == 0) {
		red();
		wprintf(L"[-] failed to patch any roblox processes. make sure you didn't already run!\n");
		return 10;
	}

	green();
	wprintf(L"[+] successfully patched %d processes.\n", total);

	wprintf(L"\n[+] press any key to leave...\n");
	getwchar();

	free(buffer);
	return 0;
}
