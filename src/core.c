#include "core.h"

bool ResolveFunctions(pfnNtQuerySystemInformation* pNtQuerySystemInformation, pfnNtQueryObject* pNtQueryObject) {
	HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
	if (!ntdll) return false;

	*pNtQuerySystemInformation =
		(pfnNtQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");

	*pNtQueryObject =
		(pfnNtQueryObject)GetProcAddress(ntdll, "NtQueryObject");

	if (!pNtQueryObject || !pNtQuerySystemInformation) return false;
	return true;
}

BOOL GetSystemHandleInformation(pfnNtQuerySystemInformation pNtQuerySystemInformation, void** out, ULONG* out_size) {
	ULONG buffer_size = 0x10000;
	void* buffer = malloc(buffer_size);

	if (!buffer) return FALSE;
	NTSTATUS status;

	ULONG retlen = 0;

	while ((status = pNtQuerySystemInformation(SystemHandleInformation, buffer, buffer_size, &retlen)) == 0xC0000004) {
		buffer_size = retlen + 0x2000;
		void* new_buffer = realloc(buffer, buffer_size);
		if (!new_buffer) {
			free(buffer);
			return FALSE;
		}
		buffer = new_buffer;
	}

	if (status != 0) {
		free(buffer);
		return FALSE;
	}

	*out = buffer;
	*out_size = buffer_size;
	return TRUE;
}

wchar_t* GetHandleName(pfnNtQueryObject NtQueryObject, HANDLE dup) {
	ULONG object_bsize = 0x1000;
	void* object_buffer = malloc(object_bsize);

	if (!object_buffer) return NULL;

	ULONG retlen;
	NTSTATUS status;

	while ((status = NtQueryObject(dup, ObjectNameInformation, object_buffer, object_bsize, &retlen)) == 0xC0000004) {
		object_bsize = retlen;
		void* new_buff = realloc(object_buffer, object_bsize);

		if (!new_buff) {
			free(object_buffer);
			return NULL;
		}

		object_buffer = new_buff;
	}

	if (status == 0) {
		POBJECT_NAME_INFORMATION nameInfo = (POBJECT_NAME_INFORMATION)object_buffer;

		if (nameInfo->Name.Buffer != NULL && nameInfo->Name.Length > 0) {
			size_t char_count = nameInfo->Name.Length / sizeof(wchar_t);
			wchar_t* heap = (wchar_t*)malloc((char_count + 1) * sizeof(wchar_t));
			if (heap) {
				wcsncpy_s(
					heap,
					char_count + 1,
					nameInfo->Name.Buffer,
					char_count
				);

				heap[char_count] = L'\0';
				free(object_buffer);
				return heap;
			}
		}
	}
	else {
		wprintf(L"[-] ntqueryobject failed with code 0x%08X.\n", status);
	}

	free(object_buffer);
	return NULL;
}

BOOL PatchHandle(HANDLE hProcess, DWORD ProcessId, const SYSTEM_HANDLE_TABLE_ENTRY_INFO* entry, pfnNtQueryObject NtQueryObject, BOOL dbg) {
	if (entry->UniqueProcessId != ProcessId) return FALSE;

	HANDLE local = NULL;
	BOOL closed = FALSE;

	if (DuplicateHandle(hProcess, (HANDLE)(ULONG_PTR)entry->HandleValue, GetCurrentProcess(), &local, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
		wchar_t* handle_name = GetHandleName(
			NtQueryObject,
			local
		);

		if (handle_name != NULL) {
			if (dbg) {
				wprintf(L"[+] handle name: %ls.\n", handle_name);
			}
			if (wcsstr(handle_name, L"ROBLOX_singletonEvent") != NULL) {
				wprintf(L"[+] handle name: %ls.\n", handle_name);
				CloseHandle(local);
				local = NULL;

				if (DuplicateHandle(hProcess, (HANDLE)(ULONG_PTR)entry->HandleValue, NULL, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE)) {
					closed = TRUE;
				}
			}

			free(handle_name);

			if (local) {
				CloseHandle(local);
			}
		}
	}

	return closed;
}
