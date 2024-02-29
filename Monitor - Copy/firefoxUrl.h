#pragma once

using namespace std;

struct urlFirefoxInfo
{
	char url;
	char title;
	string accessTime;
	int visitCount;
};

void collectFirefoxHistory();