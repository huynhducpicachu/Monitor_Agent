#include "processList.h"

#include <Windows.h>
#include <iostream>
#include <TlHelp32.h> // Include this header for process-related functions
#include <string>

void EnumerateRunningProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &processEntry)) {
            do {
                std::wcout << L"Process ID: " << processEntry.th32ProcessID << L"\t";
                std::wcout << L"Name: " << processEntry.szExeFile << std::endl;
            } while (Process32Next(hSnapshot, &processEntry));
        }
        else {
            std::wcerr << L"Error enumerating processes." << std::endl;
        }

        CloseHandle(hSnapshot);
    }
    else {
        std::wcerr << L"Error creating snapshot." << std::endl;
    }
}

