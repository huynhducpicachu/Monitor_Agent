#include <iostream>

#include <string.h>
#include "computer.h"

using namespace std;


#include <iostream>
#include <Windows.h>
#include <iphlpapi.h>
#include <string>

#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")

std::string GetComputerName() {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);

    if (GetComputerNameA(computerName, &size)) {
        return computerName;
    }
    else {
        return "Error";
    }
}

std::string GetMACAddress() {
    IP_ADAPTER_INFO adapterInfo[16];
    DWORD bufferSize = sizeof(adapterInfo);

    if (GetAdaptersInfo(adapterInfo, &bufferSize) != ERROR_SUCCESS) {
        return "Error";
    }

    std::string macAddress;
    for (PIP_ADAPTER_INFO adapter = adapterInfo; adapter != nullptr; adapter = adapter->Next) {
        for (UINT i = 0; i < adapter->AddressLength; ++i) {
            char hex[3];
            sprintf_s(hex, "%02X", adapter->Address[i]);
            macAddress += hex;
            if (i < adapter->AddressLength - 1) {
                macAddress += "-";
            }
        }
        break;  // Lấy MAC của adapter đầu tiên
    }

    return macAddress;
}

std::string GetSerialNumber() {
    // Initialize COM
    if (CoInitializeEx(0, COINIT_MULTITHREADED) != S_OK) {
        return "Error";
    }

    // Initialize security
    if (CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr) != S_OK) {
        CoUninitialize();
        return "Error";
    }

    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;

    // Create the WMI locator
    if (CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        reinterpret_cast<void**>(&pLoc)) != S_OK) {
        CoUninitialize();
        return "Error";
    }

    // Connect to the root\cimv2 namespace with the current user and obtain pointer pSvc to make IWbemServices calls.
    if (pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr,
        nullptr,
        &pSvc) != S_OK) {
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Set the security levels on the proxy
    if (CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE) != S_OK) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Use the IWbemServices pointer to make requests of WMI
    IEnumWbemClassObject* pEnumerator = nullptr;
    if (pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_BIOS"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator) != S_OK) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Get the data from the query in step 6
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    std::string serialNumber;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (uReturn == 0) {
            break;
        }

        VARIANT vtProp;
        hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);

        if (vtProp.vt == VT_BSTR) {
            serialNumber = _com_util::ConvertBSTRToString(vtProp.bstrVal);
        }

        VariantClear(&vtProp);
        pclsObj->Release();
    }

    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    return serialNumber.empty() ? "NotImplemented" : serialNumber;
}

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

std::string GetOperatingSystem()
{
    HMODULE hMod = GetModuleHandle(L"ntdll.dll");
    if (hMod)
    {
        RtlGetVersionPtr pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
        if (pRtlGetVersion != nullptr)
        {
            RTL_OSVERSIONINFOW osVersion = { sizeof(RTL_OSVERSIONINFOW) };
            if (pRtlGetVersion(&osVersion) == 0)
            {
                if (osVersion.dwMajorVersion == 10)
                {
                    if (osVersion.dwMinorVersion >= 0)
                    {
                        return "Windows 10";
                    }
                }
                else if (osVersion.dwMajorVersion == 6 && osVersion.dwMinorVersion == 3)
                {
                    return "Windows 8.1";
                }
                else if (osVersion.dwMajorVersion == 6 && osVersion.dwMinorVersion == 2)
                {
                    // Check specific features of Windows 8 here
                    return "Windows 8";
                }
                else if (osVersion.dwMajorVersion == 6 && osVersion.dwMinorVersion == 1)
                {
                    return "Windows 7";
                }
                else if (osVersion.dwMajorVersion == 6 && osVersion.dwMinorVersion == 0)
                {
                    return "Windows Vista";
                }
                else if (osVersion.dwMajorVersion == 5 && osVersion.dwMinorVersion == 2)
                {
                    return "Windows XP 64-bit Edition";
                }
                else if (osVersion.dwMajorVersion == 5 && osVersion.dwMinorVersion == 1)
                {
                    return "Windows XP";
                }
                else if (osVersion.dwMajorVersion == 5 && osVersion.dwMinorVersion == 0)
                {
                    return "Windows 2000";
                }
                else
                {
                    return "Unknown Windows Version";
                }
            }
        }
    }

    return "Error getting OS information";
}

std::string GetManufacturer()
{
    HRESULT hres;

    // Initialize COM
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        std::cerr << "Failed to initialize COM library. Error code: " << hres << std::endl;
        return "Error";
    }

    // Initialize security settings
    hres = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);

    if (FAILED(hres))
    {
        std::cerr << "Failed to initialize security. Error code: " << hres << std::endl;
        CoUninitialize();
        return "Error";
    }

    // Obtain the initial locator to WMI
    IWbemLocator* pLoc = NULL;
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        std::cerr << "Failed to create IWbemLocator object. Error code: " << hres << std::endl;
        CoUninitialize();
        return "Error";
    }

    // Connect to WMI
    IWbemServices* pSvc = NULL;
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc);

    if (FAILED(hres))
    {
        std::cerr << "Failed to connect to WMI. Error code: " << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Set security levels
    hres = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);

    if (FAILED(hres))
    {
        std::cerr << "Failed to set proxy blanket. Error code: " << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Use the IWbemServices pointer to make requests of WMI
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        _bstr_t("WQL"),
        _bstr_t("SELECT * FROM Win32_ComputerSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        std::cerr << "Query for Win32_ComputerSystem failed. Error code: " << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Retrieve the data from the query in step 6
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (uReturn == 0)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Manufacturer property
        hres = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);

        if (SUCCEEDED(hres))
        {
            std::string manufacturer = _com_util::ConvertBSTRToString(vtProp.bstrVal);
            VariantClear(&vtProp);
            pclsObj->Release();
            pEnumerator->Release();
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return manufacturer;
        }

        VariantClear(&vtProp);
        pclsObj->Release();
    }

    // Cleanup
    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return "Unknown";
}


std::string GetModel()
{
    HRESULT hres;

    // Initialize COM
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        std::cerr << "Failed to initialize COM library. Error code: " << hres << std::endl;
        return "Error";
    }

    // Initialize security settings
    hres = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);

    if (FAILED(hres))
    {
        std::cerr << "Failed to initialize security. Error code: " << hres << std::endl;
        CoUninitialize();
        return "Error";
    }

    // Obtain the initial locator to WMI
    IWbemLocator* pLoc = NULL;
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        std::cerr << "Failed to create IWbemLocator object. Error code: " << hres << std::endl;
        CoUninitialize();
        return "Error";
    }

    // Connect to WMI
    IWbemServices* pSvc = NULL;
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc);

    if (FAILED(hres))
    {
        std::cerr << "Failed to connect to WMI. Error code: " << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Set security levels
    hres = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);

    if (FAILED(hres))
    {
        std::cerr << "Failed to set proxy blanket. Error code: " << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Use the IWbemServices pointer to make requests of WMI
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        _bstr_t("WQL"),
        _bstr_t("SELECT * FROM Win32_ComputerSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        std::cerr << "Query for Win32_ComputerSystem failed. Error code: " << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return "Error";
    }

    // Retrieve the data from the query in step 6
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (uReturn == 0)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Model property
        hres = pclsObj->Get(L"Model", 0, &vtProp, 0, 0);

        if (SUCCEEDED(hres))
        {
            std::string model = _com_util::ConvertBSTRToString(vtProp.bstrVal);
            VariantClear(&vtProp);
            pclsObj->Release();
            pEnumerator->Release();
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return model;
        }

        VariantClear(&vtProp);
        pclsObj->Release();
    }

    // Cleanup
    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return "Unknown";
}

_COMPUTER_INFO getComputerInfor() {
    _COMPUTER_INFO PC;
    PC.computerName = GetComputerName();
    PC.macAddress = GetMACAddress();
    PC.serialNumber = GetSerialNumber();
    PC.OS = GetOperatingSystem();
    PC.manufacturer = GetManufacturer();
    PC.model = GetModel();
    return PC;
}