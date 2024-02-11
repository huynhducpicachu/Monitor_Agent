#include <iostream>
#include <fstream>

#include <chrono>

#include <ctime>

#include <string>
#include <filesystem>

#include <locale>
#include <codecvt>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "sqlite3.h"

#include "firefoxUrl.h"

using namespace std;
//========================================================================
wstring getLatestFirefoxProfile() {
    wstring latestProfile;

#ifdef _WIN32
    wchar_t* appDataPath = nullptr;
    size_t len;
    if (_wdupenv_s(&appDataPath, &len, L"APPDATA") == 0 && appDataPath != nullptr) {
        wstring profilesPath = wstring(appDataPath) + L"\\Mozilla\\Firefox\\Profiles";

        WIN32_FIND_DATAW findFileData;
        HANDLE hFind = FindFirstFileW((profilesPath + L"\\*").c_str(), &findFileData);

        if (hFind != INVALID_HANDLE_VALUE) {
            time_t latestTime = 0;

            do {
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    wstring profileName = findFileData.cFileName;
                    time_t lastWriteTime = findFileData.ftLastWriteTime.dwLowDateTime;

                    if (lastWriteTime > latestTime) {
                        latestTime = lastWriteTime;
                        latestProfile = profileName;
                    }
                }
            } while (FindNextFileW(hFind, &findFileData) != 0);

            FindClose(hFind);
        }

        free(static_cast<void*>(appDataPath));
    }
#endif
    
    wcout << L"PathFireFox: " << latestProfile << endl;

    return latestProfile;
}

wstring getPlacesSQLitePath(const wstring& profileName) {
    wstring placesPath;

#ifdef _WIN32
    wchar_t* appDataPath = nullptr;
    size_t len;
    if (_wdupenv_s(&appDataPath, &len, L"APPDATA") == 0 && appDataPath != nullptr) {
        placesPath = wstring(appDataPath) + L"\\Mozilla\\Firefox\\Profiles\\" + profileName + L"\\places.sqlite";
        free(appDataPath);
    }
#endif

    return placesPath;
}
//========================================================================
void handleError(sqlite3* db, const char* message) {
    cerr << message << ": " << sqlite3_errmsg(db) << endl;
}

void outputToConsoleAndFile(int64_t id, const char* url, const char* title, const string& timeString, int visitCount, int isHidden, ofstream& outFile) {
    // Output to console
    cout << "URL: " << url << ", Title: ";

    // Check if the title is not null before using it
    if (title != nullptr) {
        cout << title;
    }
    else {
        cout << "N/A";
    }

    cout << ", Visit Time: " << timeString
        << ", Visit Count: " << visitCount << ", Hidden: " << isHidden << endl;

    // Write to file
    outFile << "ID: " << id << "; URL: " << url << "; Title: ";

    // Check if the title is not null before using it
    if (title != nullptr) {
        outFile << title;
    }
    else {
        outFile << "N/A";
    }

    outFile << "; Visit Time: " << timeString
        << "; Visit Count: " << visitCount << "; Hidden: " << isHidden << endl;
}


void convertAndOutputTime(int64_t id,int64_t visitTimeInt64, const char* url, const char* title, int visitCount, int isHidden, ofstream& outFile, sqlite3* db) {
    // Convert from microseconds since an unspecified epoch
    auto epochStart = chrono::system_clock::from_time_t(0);
    auto visitTimePoint = epochStart + chrono::microseconds(visitTimeInt64);

    // Format time string
    auto timeString = chrono::system_clock::to_time_t(visitTimePoint);

    // Use localtime_s for Windows
    struct tm timeInfo;

#ifdef _WIN32
    if (localtime_s(&timeInfo, &timeString) == 0) {
#else
    if (localtime_r(&timeString, &timeInfo) != nullptr) {
#endif
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
        outputToConsoleAndFile(id, url, title, buffer, visitCount, isHidden, outFile);
    }
    else {
        handleError(db, "Error converting time.");
    }
    }

void collectFirefoxHistory() {

    wstring firefoxProfilePath = getLatestFirefoxProfile();
    wstring dbPath = getPlacesSQLitePath(firefoxProfilePath);

    wstring_convert<codecvt_utf8<wchar_t>> converter;

    string HistoryPathFirefox = converter.to_bytes(dbPath);

    sqlite3* db;

    int rc = sqlite3_open_v2(HistoryPathFirefox.c_str(), &db, SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE, nullptr);

    if (rc != SQLITE_OK) {
        handleError(db, "Cannot open Firefox database");
        return;
    }

    //const char* query = "SELECT id, url, title, last_visit_date, visit_count, hidden FROM moz_places";
    //const char* query = "SELECT id, url, title, last_visit_date, visit_count, hidden FROM moz_places WHERE url LIKE '%search?q=%' OR url LIKE '%search?query=%'";
    const char* query = "SELECT id, url, title, last_visit_date, visit_count, hidden FROM moz_places WHERE url LIKE '%results?search_query%' OR url LIKE '%search?client%'";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        handleError(db, "Cannot prepare statement");
        sqlite3_close(db);
        return;
    }

    // Open the file for writing
    ofstream outFile("firefox_output.txt");

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int64_t id             = sqlite3_column_int64(stmt, 0);
        const char* url        = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* title      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int64_t visitTimeInt64 = sqlite3_column_int64(stmt, 3);
        int visitCount         = sqlite3_column_int(stmt, 4);
        int isHidden           = sqlite3_column_int(stmt, 5);

        convertAndOutputTime(id, visitTimeInt64, url, title, visitCount, isHidden, outFile, db);
    }

    // Finalize the statement before closing the file and database
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        handleError(db, "Error reading Firefox data");
    }

    // Close the file
    outFile.close();

    // Close the database
    sqlite3_close(db);
}