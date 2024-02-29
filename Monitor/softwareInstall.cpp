#include <Windows.h>
#include <iostream>
#include <string>

void EnumerateInstalledSoftware(HKEY hKey, const std::wstring& subkey) {
    HKEY hSubKey;
    if (RegOpenKeyExW(hKey, subkey.c_str(), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
        DWORD index = 0;
        WCHAR achKey[255];
        DWORD cbName = sizeof(achKey);

        while (RegEnumKeyExW(hSubKey, index++, achKey, &cbName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            HKEY hSoftwareKey;
            if (RegOpenKeyExW(hSubKey, achKey, 0, KEY_READ, &hSoftwareKey) == ERROR_SUCCESS) {
                WCHAR achValue[1024];
                DWORD cbValue = sizeof(achValue);

                if (RegQueryValueExW(hSoftwareKey, L"DisplayName", NULL, NULL, (LPBYTE)achValue, &cbValue) == ERROR_SUCCESS) {
                    std::wcout << achValue << std::endl;
                }
                RegCloseKey(hSoftwareKey);
            }
            cbName = sizeof(achKey);
        }
        RegCloseKey(hSubKey);
    }
}

int main() {
    std::wcout << L"Installed software:" << std::endl;
    EnumerateInstalledSoftware(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    EnumerateInstalledSoftware(HKEY_LOCAL_MACHINE, L"Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"); // For 32-bit software on 64-bit Windows
    return 0;
}
