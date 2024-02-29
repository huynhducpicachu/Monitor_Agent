#pragma once
#include <string>

using namespace std;

struct _URL_CHROME_INFO 
{
	int id;
	string url;
	char title;
	string visitTime;
};

struct _LIST_URL_CHROME_INFO
{
	int countUrlChrome;
	_URL_CHROME_INFO arrUrlChrome[1000];
};

//==========================================================
struct _URL_SEARCH_CHROME_INFO
{
	int id;
	string url;
	char title;
	string visitTime;
};

struct _LIST_URL_SEARCH_CHROME_INFO
{
	int countUrlSearchChrome;
	_URL_SEARCH_CHROME_INFO arrUrlSearchChrome[1000];
};

//==========================================================
struct _URL_YOUTUBE_CHROME_INFO
{
	int id;
	string url;
	char title;
	string visitTime;
};

struct _LIST_URL_YOUTUBE_CHROME_INFO
{
	int countUrlYoutubeChrome;
	_URL_YOUTUBE_CHROME_INFO arrUrlYoutubeChrome[1000];
};

//==========================================================
struct _URL_SEARCH_YOUTUBE_CHROME_INFO
{
	int id;
	string url;
	char title;
	string visitTime;
};

struct _LIST_URL_SEARCH_YOUTUBE_CHROME_INFO
{
	int countUrlSearchYoutubeChrome;
	_URL_SEARCH_YOUTUBE_CHROME_INFO arrUrlSearchYoutubeChrome[1000];
};


void processChromeHistory();