#include "process.h"

DWORD GetRobloxPID() {
	DWORD pid = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snap == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;
	PROCESSENTRY32W proc;
	proc.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(snap, &proc)) {
		CloseHandle(snap);
		return FALSE;
	}

	do {
		if (wcscmp(proc.szExeFile, L"RobloxPlayerBeta.exe") == 0) {
			pid = proc.th32ProcessID;
			break;
		}
	} while (Process32NextW(snap, &proc));

	CloseHandle(snap);
	return pid;
}
