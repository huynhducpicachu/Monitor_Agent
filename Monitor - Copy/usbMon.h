#pragma once
#include <Windows.h>
#include <thread>

#ifndef USBMON_H
#define USBMON_H

#include <string>

class DeviceNotification {
public:
    DeviceNotification();
    ~DeviceNotification();
    void run_from_thread();

private:
    HWND hWnd;
    
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



class USBThread : public std::thread {
public:
    USBThread(DeviceNotification& deviceNotification) : deviceNotification(deviceNotification) {}

    // Phương thức thực thi của luồng
    void run() {
        deviceNotification.run_from_thread();
    }

private:
    DeviceNotification& deviceNotification;
};
#endif // USBMON_H
