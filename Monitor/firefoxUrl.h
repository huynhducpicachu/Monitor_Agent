#pragma once

using namespace std;

struct _URL_FIREFOX_INFO
{
	int id;
	string url;
	char title;
	string visitTime;
};

void collectFirefoxHistory();