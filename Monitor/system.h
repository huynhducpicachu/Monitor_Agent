#pragma once
#include <ctime>
using namespace std;

struct _CONFIG {
	string Git_Url;
	string Campain;
	string LastModifyFile;
};

struct _FILE_INFO {
	string FileName;
	string LastModify;
};

struct _LIST_FILE_INFO {
	int count = 0;
	_FILE_INFO arr[1000];
};

_LIST_FILE_INFO GetListFile(string RootFolder, string LastModify, string StartDate, string EndDate);