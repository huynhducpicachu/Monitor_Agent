#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>

// Function to read registry values under a specified key
std::vector<std::wstring> ReadRegistryValues(const std::wstring& keyPath) {
    std::vector<std::wstring> values;

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t valueName[MAX_PATH];
        DWORD   valueNameSize = MAX_PATH;
        DWORD   valueType;
        BYTE    valueData[MAX_PATH];
        DWORD   valueDataSize = MAX_PATH;

        DWORD   index = 0;
        while (RegEnumValueW(hKey, index, valueName, &valueNameSize, NULL, &valueType, valueData, &valueDataSize) == ERROR_SUCCESS) {
            if (valueType == REG_SZ) {
                values.push_back(std::wstring(valueName));
            }

            // Reset sizes for next iteration
            valueNameSize = MAX_PATH;
            valueDataSize = MAX_PATH;
            index++;
        }

        RegCloseKey(hKey);
    }

    return values;
}

int main() {
    // Đường dẫn trong registry
    std::wstring keyPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs\\.mp4";

    // Đọc các giá trị từ registry
    std::vector<std::wstring> recentFiles = ReadRegistryValues(keyPath);

    // Hiển thị danh sách các tệp đã mở gần đây
    if (recentFiles.empty()) {
        std::wcout << L"No recent opened files found." << std::endl;
    }
    else {
        std::wcout << L"Recent opened files:" << std::endl;
        for (const auto& file : recentFiles) {
            std::wcout << file << std::endl;
        }
    }

    return 0;
}
