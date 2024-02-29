#include <iostream>
#include <string>

#include <windows.h>
#include <ctime>
#include <algorithm>

#include "system.h"

using namespace std;

string filetimeToString(const FILETIME& filetime)
{
	SYSTEMTIME systemtime;
	FileTimeToSystemTime(&filetime, &systemtime);
	char buffer[24];
	sprintf_s(buffer, "%04d_%02d_%02d %02d_%02d_%02d.%03d", systemtime.wYear, systemtime.wMonth, systemtime.wDay,
		systemtime.wHour, systemtime.wMinute, systemtime.wSecond, systemtime.wMilliseconds);
	return string(buffer);
}

bool compareFileByTime(const _FILE_INFO& a, const _FILE_INFO& b) {
	return a.LastModify < b.LastModify;
}

void ScanFolder(string folder, _LIST_FILE_INFO* ListFileInfor, int* numberfiles, string LastModify, string StartDate, string EndDate) {

	WIN32_FIND_DATAA findData;

	HANDLE hFind = FindFirstFileA((folder + "/*").c_str(), &findData);
	FindNextFileA(hFind, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		cout << "Error: " << GetLastError() << endl;
		return;
	}
	do
	{
		string currentFileTime = filetimeToString(findData.ftLastWriteTime);
		if (currentFileTime >= LastModify)
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
				{
					ScanFolder(folder + "/" + findData.cFileName, ListFileInfor, numberfiles, LastModify, StartDate, EndDate);
				}
			}
			else
				if (currentFileTime >= StartDate && currentFileTime <= EndDate)
				{
					*numberfiles = *numberfiles + 1;
					int a = *numberfiles;

					ListFileInfor->arr[*numberfiles].FileName = folder + "/" + findData.cFileName;
					ListFileInfor->arr[*numberfiles].LastModify = currentFileTime;
				}
		}
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);
}

_LIST_FILE_INFO GetListFile(string RootFolder, string LastModify, string StartDate, string EndDate) {
	_LIST_FILE_INFO ListFileInfor;
	int numberfiles = 0;
	ScanFolder(RootFolder, &ListFileInfor, &numberfiles, LastModify, StartDate, EndDate);
	sort(ListFileInfor.arr, ListFileInfor.arr + numberfiles, compareFileByTime);

	ListFileInfor.count = numberfiles;

	return ListFileInfor;
};