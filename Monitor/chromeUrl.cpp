#include <iostream>

#include <fstream>

#include <cstdlib>

#include <Windows.h>
#include <Knownfolders.h>
#include <Shlobj.h>

// Link against the Shell32.lib library
#pragma comment(lib, "Shell32.lib")

#include <locale>
#include <codecvt>

#include "sqlite3.h"
#include <ctime>

#include "chromeUrl.h"

// Function to get the path to the Chrome history folder
wstring getChromeHistoryPath() {
    PWSTR localAppData;

    if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &localAppData) != S_OK) {
        wcerr << L"Error getting Local AppData folder." << endl;
        return L"";
    }

    wstring chromeProfilePath = wstring(localAppData) + L"\\Google\\Chrome\\User Data\\Default";

    CoTaskMemFree(localAppData);

    return chromeProfilePath;
}
// Function to create a new folder and get its path

bool createNewFolder(const wstring& parentFolderPath, const wstring& newFolderName, wstring& newPath) {
    newPath = parentFolderPath + L"\\" + newFolderName;

    if (CreateDirectoryW(newPath.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS) {
        wcout << L"New folder is created\n";
        return true;
    }
    else {
        wcerr << L"Error creating folder: " << GetLastError() << L"\n";
        return false;
    }
}

// Function to copy the Chrome history file
bool copyHistoryFile(const wstring& sourceFolderPath, const wstring& destinationFolderPath) {
    wstring sourceFilePath = sourceFolderPath + L"\\History";
    wstring destinationFilePath = destinationFolderPath + L"\\History";

    if (CopyFileW(sourceFilePath.c_str(), destinationFilePath.c_str(), FALSE)) {
        wcout << L"History file is copied.\n";
        return true;
    }
    else {
        wcerr << L"Error copying History file: " << GetLastError() << L"\n";
        return false;
    }
}

// Function to check if a URL corresponds to an email link
bool isEmailLink(const char* url) 
{
    const char* emailDomains[] = { "gmail.com", "yahoo.com", "outlook.com" }; 

    for (const char* domain : emailDomains) {
        if (strstr(url, domain) != nullptr) {
            return true;
        }
    }

    return false;
}
bool isYoutubeLink(const char* url)
{
    const char* youtubeDomains[] = { "youtube.com" }; // 

    for (const char* domain : youtubeDomains) {
        if (strstr(url, domain) != nullptr) {
            return true;
        }
    }

    return false;
}
bool isYoutubeSearchLink(const char* url)
{
    const char* googleSearchDomains[] = { "youtube.com/results?search_quer"}; // 

    for (const char* domain : googleSearchDomains) {
        if (strstr(url, domain) != nullptr) {
            return true;
        }
    }
    return false;
}

bool isGoogleSearchLink(const char* url)
{
    const char* youtubeSearchDomains[] = { "https://www.google.com/search?q" }; // 

    for (const char* domain : youtubeSearchDomains) {
        if (strstr(url, domain) != nullptr) {
            return true;
        }
    }

    return false;
}

bool isYahooSearchLink(const char* url)
{
    const char* youtubeSearchDomains[] = { "https://www.google.com/search?q" }; // 

    for (const char* domain : youtubeSearchDomains) {
        if (strstr(url, domain) != nullptr) {
            return true;
        }
    }

    return false;
}

// Function to process Chrome history
void processChromeHistory() {
    wstring chromeHistoryPath = getChromeHistoryPath();

    wstring historyPathChromeNew;
    createNewFolder(chromeHistoryPath, L"ChromeHistory", historyPathChromeNew);
    wcout << "Path Of New Folder Contain History File: " << historyPathChromeNew << endl;

    copyHistoryFile(chromeHistoryPath, historyPathChromeNew);

    wstring_convert<codecvt_utf8<wchar_t>> converter;
    string HistoryPathChrome = converter.to_bytes(historyPathChromeNew+ L"\\History");

    const char* outputFilePath = "outputChrome.txt";

    sqlite3* db;

    int rc = sqlite3_open_v2(HistoryPathChrome.c_str(), &db, SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char* query = "SELECT id, url, title, visit_count, typed_count, last_visit_time, hidden FROM urls ORDER BY last_visit_time DESC";

    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    std::ofstream outFile(outputFilePath);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(stmt, 0);
        const char* url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* title = reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        int visitCount = sqlite3_column_int(stmt, 3);
        int typeCount = sqlite3_column_int(stmt, 4);
        int64_t visitTimeInt64 = sqlite3_column_int64(stmt, 5);
        int isHidden = sqlite3_column_int(stmt, 6);
        //cout << "RawTime: " << visitTimeInt64;
        time_t visitTime = static_cast<time_t>(visitTimeInt64/1000000 - 11644473600LL);
        //cout << "RawTime2: " << visitTimeInt64;
        char buffer[80];
        struct tm timeInfo;

        //=================================================
        bool isEmail = isEmailLink(title);
        bool isYoutube = isYoutubeLink(url);
        bool isYoutubeSearch = isYoutubeSearchLink(url);
        bool isGoogleSearch  = isGoogleSearchLink(url);

        if (localtime_s(&timeInfo, &visitTime) == 0) {
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);

            if (isEmail) {
                cout << "Email Link - ID: " << id << "; URL: " << url << "; TITLE: " << title << "; Visit Time : " << buffer << std::endl;
                outFile << "Email Link - ID: " << id << "; URL: " << url << "; TITLE: " << title << "; Visit Time : " << buffer << std::endl;
                outFile << "//==============================================================================" << endl;
            }
            else
            {
                if (isYoutube) {
                    if (isYoutubeSearch) {
                        cout << "  SEARCH YOUTUBE TITLE: " << title << std::endl;
                        outFile << "SEARCH YOUTUBE TITLE: " << title << std::endl;
                        outFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
                    }
                    else
                    {
                        cout << "Youtube Link - ID: " << id << "; URL: " << url << "; TITLE: " << title << "; Visit Time : " << buffer << std::endl;
                        outFile << "Youtube  Link - ID: " << id << "; URL: " << url << "; TITLE: " << title << "; Visit Time : " << buffer << std::endl;
                        outFile << "//////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
                    }                   
                }
                else
                {
                    if (isGoogleSearch) {
                        cout   << "ID: " << id << "SEARCH GOOGLE TITLE: " << title << "URL: " << url << "; Visit Time : " << buffer << std::endl;
                        outFile << "ID: " << id << "SEARCH GOOGLE TITLE: " << title << "URL: " << url << "; Visit Time : " << buffer << std::endl;
                        outFile << "-----------------------------------------------------------------------------------------------" << endl;
                    }
                    else {
                        cout << "ID: " << id << "; URL: " << url << "; TITLE: " << title << "; Visit Time : " << buffer << std::endl;
                        outFile << "ID: " << id << "; URL: " << url << "; TITLE: " << title << "; Visit Time : " << buffer << std::endl;
                    }
                }
            }
        }
        else {
            std::cerr << "Error converting time." << std::endl;
        }
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error reading data: " << sqlite3_errmsg(db) << std::endl;
    }

    outFile.close();

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}