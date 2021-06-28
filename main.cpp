#undef UNICODE
#include <iostream>
#include <Windows.h>
using namespace std;

bool inject(int pid, const char* dllFile);

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "Usage: console.exe 'Process ID' 'DLL Path'" << endl;
		return 1;
	}
	inject(atoi(argv[1]), argv[2]);
	return EXIT_SUCCESS;
}

bool inject(int pid, const char* dllFile)
{
	if (!dllFile)
		return false;

	size_t dllNameLen = strlen(dllFile) + 1;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hProc == NULL)
	{
		return false;
	}
	LPVOID param = VirtualAllocEx(hProc, NULL, dllNameLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!param)
	{
		return false;
	}
	int writeSucceeded = WriteProcessMemory(hProc, param, dllFile, dllNameLen, 0);
	if (!writeSucceeded)
	{
		return false;
	}
	DWORD threadId = 0;
	LPTHREAD_START_ROUTINE loadLibFunc = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary("kernel32"), "LoadLibraryA");
	if (!loadLibFunc)
		return false;
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, loadLibFunc, param, 0, &threadId);
	return hThread!=NULL;
}
