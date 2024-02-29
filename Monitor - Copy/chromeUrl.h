#pragma once
#include <string>

using namespace std;

struct urlChromeInfo
{
	char url;
	char title;
	string accessTime;
	int visitCount;
};

void processChromeHistory();