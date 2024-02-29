//#include <iostream>
//#include <iomanip>
//#include <Windows.h>
//#include <Dbt.h>
//#include <chrono>
//#include <ctime>
//#include "usbMon.h"
//
//using namespace std;
//using namespace std::chrono;
//
//high_resolution_clock::time_point plugInTime; // Define plugInTime and plugOutTime globally
//high_resolution_clock::time_point plugOutTime;
//
//DeviceNotification::DeviceNotification() {
//    // Register window class
//    WNDCLASS wc = {};
//    wc.lpfnWndProc = WndProc;
//    wc.hInstance = GetModuleHandle(nullptr);
//    wc.lpszClassName = L"USBEventWindowClass";
//    RegisterClass(&wc);
//
//    // Create a window
//    hWnd = CreateWindow(wc.lpszClassName, nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
//
//    if (!hWnd) {
//        cerr << "Failed to create window. Error: " << GetLastError() << endl;
//    }
//    else {
//        cout << "Window created successfully" << endl;
//    }
//
//    // Register for USB device change notifications
//    DEV_BROADCAST_DEVICEINTERFACE dbdi = {};
//    dbdi.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
//    dbdi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
//
//    RegisterDeviceNotification(hWnd, &dbdi, DEVICE_NOTIFY_WINDOW_HANDLE);
//}
//
//DeviceNotification::~DeviceNotification() {
//    // Clean up
//    UnregisterDeviceNotification(hWnd);
//}
//
//void DeviceNotification::run_from_thread() {
//    MSG msg;
//    while (GetMessage(&msg, nullptr, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//}
//
//void printTime(const string& label, const high_resolution_clock::time_point& timePoint) {
//    auto system_now = system_clock::now();
//    auto duration_since_epoch = timePoint.time_since_epoch() - high_resolution_clock::now().time_since_epoch() + system_now.time_since_epoch();
//    auto millis = duration_cast<milliseconds>(duration_since_epoch).count() % 1000;
//
//    time_t tt = system_clock::to_time_t(system_clock::time_point(duration_cast<system_clock::duration>(duration_since_epoch)));
//    tm tm_time;
//    localtime_s(&tm_time, &tt);
//
//    cout << label << ": " << put_time(&tm_time, "%Y-%m-%d %H:%M:%S") << "." << setfill('0') << setw(3) << millis << " milliseconds" << endl;
//}
//
//
//void convertAndOutputTime(const steady_clock::time_point& timePoint) {
//    // Convert from microseconds since an unspecified epoch
//    auto epochStart = system_clock::from_time_t(0);
//    auto visitTimePoint = epochStart + duration_cast<system_clock::duration>(timePoint.time_since_epoch());
//
//    // Format time string
//    auto timeString = system_clock::to_time_t(visitTimePoint);
//    struct tm timeInfo;
//    localtime_s(&timeInfo, &timeString);
//
//    char buffer[80];
//    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
//    std::cout << "Time: " << buffer << std::endl;
//}
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//    PDEV_BROADCAST_HDR lpdb = nullptr;
//
//    switch (message) {
//    case WM_CREATE:
//        cout << "WM_CREATE received" << endl;
//        break;
//
//    case WM_DEVICECHANGE:
//        cout << "WM_DEVICECHANGE received. wParam: " << wParam << endl;
//        lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
//
//        switch (wParam) {
//        case DBT_DEVICEARRIVAL:
//            cout << "DBT_DEVICEARRIVAL received" << endl;
//
//            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
//                plugInTime = high_resolution_clock::now(); // Record plug in time
//
//                PDEV_BROADCAST_VOLUME lpdbv = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lpdb);
//                DWORD unitMask = lpdbv->dbcv_unitmask;
//
//                for (int drive = 0; drive < 26; ++drive) {
//                    if (unitMask & (1 << drive)) {
//                        cout << "USB Device Inserted. Drive Letter: " << char('A' + drive) << endl;
//                    }
//                }
//            }
//            break;
//
//        case DBT_DEVICEREMOVECOMPLETE:
//            cout << "DBT_DEVICEREMOVECOMPLETE received" << endl;
//
//            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
//                plugOutTime = high_resolution_clock::now(); // Record plug out time
//
//                PDEV_BROADCAST_VOLUME lpdbv = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lpdb);
//                cout << "USB Device Removed. Drive Letter: " << char('A' + lpdbv->dbcv_unitmask) << endl;
//
//                // Print plug-in and plug-out times
//                printTime("Plug-in Time", plugInTime);
//                printTime("Plug-out Time", plugOutTime);
//
//                // Calculate and print duration
//                auto duration = duration_cast<milliseconds>(plugOutTime - plugInTime).count();
//                cout << "Duration: " << duration << " milliseconds" << endl;
//            }
//            break;
//        }
//
//        break;
//
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//
//    return 0;
//}






#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <Dbt.h>
#include <chrono>
#include <ctime>
#include "usbMon.h"

using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point plugInTime;
high_resolution_clock::time_point plugOutTime;


DeviceNotification::DeviceNotification() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"USBEventWindowClass";
    RegisterClass(&wc);

    hWnd = CreateWindow(wc.lpszClassName, nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

    if (!hWnd) {
        cerr << "Failed to create window. Error: " << GetLastError() << endl;
    }
    else {
        cout << "Window created successfully" << endl;
    }

    DEV_BROADCAST_DEVICEINTERFACE dbdi = {};
    dbdi.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    dbdi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    RegisterDeviceNotification(hWnd, &dbdi, DEVICE_NOTIFY_WINDOW_HANDLE);
}

DeviceNotification::~DeviceNotification() {
    UnregisterDeviceNotification(hWnd);
}

void DeviceNotification::run_from_thread() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void printTime(const string& label, const high_resolution_clock::time_point& timePoint) {
    auto system_now = system_clock::now();
    auto duration_since_epoch = timePoint.time_since_epoch() - high_resolution_clock::now().time_since_epoch() + system_now.time_since_epoch();
    auto millis = duration_cast<milliseconds>(duration_since_epoch).count() % 1000;

    time_t tt = system_clock::to_time_t(system_clock::time_point(duration_cast<system_clock::duration>(duration_since_epoch)));
    tm tm_time;
    localtime_s(&tm_time, &tt);

    cout << label << ": " << put_time(&tm_time, "%Y-%m-%d %H:%M:%S") << "." << setfill('0') << setw(3) << millis << " milliseconds" << endl;
}

void convertAndOutputTime(const steady_clock::time_point& timePoint) {
    auto epochStart = system_clock::from_time_t(0);
    auto visitTimePoint = epochStart + duration_cast<system_clock::duration>(timePoint.time_since_epoch());

    auto timeString = system_clock::to_time_t(visitTimePoint);
    struct tm timeInfo;
    localtime_s(&timeInfo, &timeString);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    std::cout << "Time: " << buffer << std::endl;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PDEV_BROADCAST_HDR lpdb = nullptr;

    switch (message) {
    case WM_CREATE:
        cout << "WM_CREATE received" << endl;
        break;

    case WM_DEVICECHANGE:
        cout << "WM_DEVICECHANGE received. wParam: " << wParam << endl;
        lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);

        switch (wParam) {
        case DBT_DEVICEARRIVAL:
            cout << "DBT_DEVICEARRIVAL received" << endl;

            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                plugInTime = high_resolution_clock::now();

                PDEV_BROADCAST_VOLUME lpdbv = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lpdb);
                DWORD unitMask = lpdbv->dbcv_unitmask;

                for (int drive = 0; drive < 26; ++drive) {
                    if (unitMask & (1 << drive)) {
                        cout << "USB Device Inserted. Drive Letter: " << char('A' + drive) << endl;
                    }
                }
            }
            break;

        case DBT_DEVICEREMOVECOMPLETE:
            cout << "DBT_DEVICEREMOVECOMPLETE received" << endl;

            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                plugOutTime = high_resolution_clock::now();

                PDEV_BROADCAST_VOLUME lpdbv = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lpdb);
                cout << "USB Device Removed. Drive Letter: " << char('A' + lpdbv->dbcv_unitmask) << endl;

                printTime("Plug-in Time", plugInTime);
                printTime("Plug-out Time", plugOutTime);

                auto duration = duration_cast<milliseconds>(plugOutTime - plugInTime).count();
                cout << "Duration: " << duration << " milliseconds" << endl;
            }
            break;
        }

        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}