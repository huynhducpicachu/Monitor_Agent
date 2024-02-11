#include <iostream>
#include <Windows.h>

void PrintRecentFiles() {
    const wchar_t* regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs\\.mp4";
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, regPath, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        LPWSTR errorMessage = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessage, 0, NULL);
        if (errorMessage) 
        {
            wprintf(L"Failed to open registry key: %s\n", errorMessage);
            LocalFree(errorMessage);
        }
        return;
    }

    wchar_t valueName[256];
    DWORD   valueNameSize = 256;
    BYTE    data[1024];
    DWORD   dataSize = 1024;
    DWORD   valueType;
    DWORD   index = 0;

    while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, &valueType, data, &dataSize) == ERROR_SUCCESS) 
    {
        wprintf(L"%s\n", (wchar_t*)data);
        valueNameSize = 256;
        dataSize = 1024;
    }

    RegCloseKey(hKey);
}

int main() {
    
    PrintRecentFiles();
    return 0;
}
