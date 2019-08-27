#pragma once
#include <Windows.h>
#include <TlHelp32.h>

class Memory
{
public:
	HANDLE hProcess;

	template <typename datatype>
	datatype ReadMemory(DWORD address)
	{
		datatype buffer;
		ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(datatype), NULL);
		return buffer;
	}

	template <typename datatype>
	void WriteMemory(DWORD address, datatype value)
	{
		WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(value), NULL);
	}

	HANDLE GetHandleByProcessName(char* processName, DWORD dwAccessRights)
	{
		DWORD pID = NULL;
		HANDLE hProcess = INVALID_HANDLE_VALUE;
		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (ss != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			do
			{
				if (!strcmp(pe.szExeFile, processName))
				{
					pID = pe.th32ProcessID;
					hProcess = OpenProcess(dwAccessRights, false, pID);
				}
			} while (Process32Next(ss, &pe));
			CloseHandle(ss);
		}
		return hProcess;
	}

	HANDLE GetHandleByWindowName(char* windowName, DWORD dwAccessRights)
	{
		DWORD pID = NULL;
		HANDLE hProcess = INVALID_HANDLE_VALUE;
		HWND hW = FindWindowA(NULL, windowName);
		GetWindowThreadProcessId(hW, &pID);
		CloseHandle(hW);
		if (pID != NULL)
		{
			hProcess = OpenProcess(dwAccessRights, false, pID);
		}
		return hProcess;
	}

	DWORD GetProcessIdByProcessName(char* processName)
	{
		DWORD pID = NULL;
		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (ss != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			do
			{
				if (!strcmp(pe.szExeFile, processName))
				{
					pID = pe.th32ProcessID;
				}
			} while (Process32Next(ss, &pe));
			CloseHandle(ss);
		}
		return pID;
	}

	DWORD GetProcessIdByWindowName(char* windowName)
	{
		DWORD pID = NULL;
		HWND hW = FindWindowA(NULL, windowName);
		GetWindowThreadProcessId(hW, &pID);
		CloseHandle(hW);
		return pID;
	}

	DWORD GetModuleBaseAddress(DWORD pID, char* moduleName)
	{
		DWORD ModuleBaseAddress = NULL;
		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
		if (ss != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 me;
			me.dwSize = sizeof(MODULEENTRY32);

			if (Module32First(ss, &me))
			{
				do
				{
					if (!strcmp(me.szModule, moduleName))
					{
						ModuleBaseAddress = (DWORD)me.modBaseAddr;
						break;
					}
				} while (Module32Next(ss, &me));
			}
			CloseHandle(ss);
		}
		return ModuleBaseAddress;
	}

};

Memory mem;