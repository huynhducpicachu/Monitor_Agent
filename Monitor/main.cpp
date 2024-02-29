#define _CRT_SECURE_NO_WARNINGS

#include<iostream>

#include <thread>
#include <mutex>

#include "system.h"
#include "network.h"
#include "computer.h"

#include "usbMon.h"

#include "firefoxUrl.h"

#include "chromeUrl.h"

#include "softwareInstall.h"

#include "processList.h"

using namespace std;

_CONFIG config;
_LIST_FILE_INFO listFile;
_FILE_INFO fileInfor;

_COMPUTER_INFO PC;

string hostName;
string campain;

string rootFolder;
string startDate, endDate;

int countFiles;


int main()
{

	//Computer Infor==========================================================================
	//PC = getComputerInfor();
	//cout << "Computer Name: " << PC.computerName << endl;
	//cout << "MAC Address: "   << PC.macAddress   << endl;
	//cout << "Serial Number: " << PC.serialNumber << endl;
	//cout << "OS: "            << PC.OS           << endl;
	//cout << "Manufacturer: "  << PC.manufacturer << endl;
	//cout << "Model: "         << PC.model        << endl;

	//Url Firefox==============================================================================
	//const char* firefoxProfilePath = "C:\\Users\\HuynhDuc\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles\\qn2bivag.default-release";
	//collectFirefoxHistory();
	
	//Url Chrome==============================================================================
	processChromeHistory();

	//Software Install========================================================================
	//EnumerateVideoAudioPlayers();

	//std::string fileName = "installed_programs.txt";
	//ExportInstalledProgramsToFile(fileName);
	
	//========================================================================================
	//EnumerateRunningProcesses();

	//USB detect==============================================================================
	DeviceNotification deviceNotification;
	deviceNotification.run_from_thread();


	//////////////////////////////////////////////////////////////////
	//DeviceNotification deviceNotification;

	// Tạo một đối tượng luồng và chạy phương thức run()
	//USBThread notificationThread(deviceNotification);
	//notificationThread.run();

	// Đợi luồng kết thúc trước khi thoát
	//notificationThread.join();
	//////////////////////////////////////////////////////////////////

	return 0;
}
