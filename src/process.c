#include "process.h"

unsigned int GetRobloxPID(DWORD* arr, unsigned int max_cnt) {
	unsigned int cnt = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snap == INVALID_HANDLE_VALUE) return 0;
	PROCESSENTRY32W proc;
	proc.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(snap, &proc)) {
		CloseHandle(snap);
		return 0;
	}

	do {
		if (wcscmp(proc.szExeFile, L"RobloxPlayerBeta.exe") == 0) {
			arr[cnt] = proc.th32ProcessID;
			cnt++;
			if (cnt >= max_cnt) break;
		}
	} while (Process32NextW(snap, &proc));

	CloseHandle(snap);
	return cnt;
}
