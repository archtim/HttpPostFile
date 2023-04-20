#pragma once
#include <Windows.h>
#include <Wininet.h>
#include <iostream>
#pragma comment(lib,"wininet.lib")

class CPostFile {
private:
    HINTERNET hSession;
    HINTERNET hConnect;

    bool WriteToInternet(HINTERNET hInet, const void* Data, DWORD DataSize);
    char* WCharToChar(wchar_t* szUnicodeString);

public:

    int PostFile(LPCSTR szFile, LPCSTR szServer, INTERNET_PORT dwPort, LPCSTR szRoute);
};
