#include "PostFile.h"

#define DEFAULT_USERAGENT "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:40.0) Gecko/20100101 Firefox/40.1" 
#define METHOD_POST "POST"
#define BUFSIZE 1024

struct FileCloser
{
    typedef HANDLE pointer;

    void operator()(HANDLE h)
    {
        if (h != INVALID_HANDLE_VALUE)
            CloseHandle(h);
    }
};

struct InetCloser
{
    typedef HINTERNET pointer;

    void operator()(HINTERNET h)
    {
        if (h != NULL)
            InternetCloseHandle(h);
    }
};

char* CPostFile::WCharToChar(wchar_t* szUnicodeString) {
    char* buffer = new char[wcslen(szUnicodeString) + 1];
    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, buffer, wcslen(szUnicodeString) + 1, szUnicodeString, _TRUNCATE);
    return buffer;
};

bool CPostFile::WriteToInternet(HINTERNET hInet, const void* Data, DWORD DataSize)
{
    const BYTE* pData = (const BYTE*)Data;
    DWORD dwBytes;

    while (DataSize > 0)
    {
        if (!InternetWriteFile(hInet, pData, DataSize, &dwBytes))
            return false;
        pData += dwBytes;
        DataSize -= dwBytes;
    }

    return true;
}


int CPostFile::PostFile(LPCSTR szFile, LPCSTR szServer, INTERNET_PORT wPort, LPCSTR szRoute) {
 
    const char* szHeaders = "Content-Type: multipart/form-data; boundary=----974767299852498929531610575";
    const char* szContent = "------974767299852498929531610575\r\nContent-Disposition: form-data; name=\"file\"; filename=\"2023-04-02-07-29-50-image.jpg\"\r\nContent-Type: application/octet-stream\r\n\r\n";
    const char* szEndData = "\r\n------974767299852498929531610575--\r\n";

    std::unique_ptr<HANDLE, FileCloser> hIn(CreateFileA(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL));
    if (hIn.get() == INVALID_HANDLE_VALUE)
    {
        std::cerr << "CreateFile Error" << std::endl;
        return 1;
    }

    DWORD dwFileSize = GetFileSize(hIn.get(), NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        std::cerr << "GetFileSize Error" << std::endl;
        return 1;
    }

    std::unique_ptr<HINTERNET, InetCloser> io(InternetOpenA(DEFAULT_USERAGENT, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0));
    if (io.get() == NULL)
    {
        std::cerr << "InternetOpen Error" << std::endl;
        return 1;
    }

    std::unique_ptr<HINTERNET, InetCloser> ic(InternetConnectA(io.get(), szServer, wPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0));
    if (ic.get() == NULL)
    {
        std::cerr << "InternetConnect Error" << std::endl;
        return 1;
    }

    std::unique_ptr<HINTERNET, InetCloser> hreq(HttpOpenRequestA(ic.get(), METHOD_POST, szRoute, NULL, NULL, NULL, 0, 0));
    if (hreq.get() == NULL)
    {
        std::cerr << "HttpOpenRequest Error" << std::endl;
        return 1;
    }

    if (!HttpAddRequestHeadersA(hreq.get(), szHeaders, -1, HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD))
    {
        std::cerr << "HttpAddRequestHeaders Error" << std::endl;
        return 1;
    }

    DWORD sContentSize = static_cast<DWORD>(strlen(szContent));
    DWORD sEndDataSize = static_cast<DWORD>(strlen(szEndData));

    INTERNET_BUFFERS bufferIn = {};
    bufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
    bufferIn.dwBufferTotal = sContentSize + dwFileSize + sEndDataSize;

    if (!HttpSendRequestEx(hreq.get(), &bufferIn, NULL, HSR_INITIATE, 0))
    {
        std::cerr << "HttpSendRequestEx Error" << std::endl;
        return 1;
    }

    if (!WriteToInternet(hreq.get(), szContent, sContentSize))
    {
        std::cerr << "InternetWriteFile Error" << std::endl;
        return 1;
    }

    char szData[BUFSIZE];
    DWORD dw = 0, dwBytes;

    while (dw < dwFileSize)
    {
        if (!ReadFile(hIn.get(), szData, (((dwFileSize - dw) < (sizeof(szData))) ? (dwFileSize - dw) : (sizeof(szData))), &dwBytes, NULL))
        {
            std::cerr << "ReadFile Error" << std::endl;
            return 1;
        }

        if (!WriteToInternet(hreq.get(), szData, dwBytes))
        {
            std::cerr << "InternetWriteFile Error" << std::endl;
            return 1;
        }

        dw += dwBytes;
    }

    if (!WriteToInternet(hreq.get(), szEndData, sEndDataSize))
    {
        std::cerr << "InternetWriteFile Error" << std::endl;
        return 1;
    }

    if (!HttpEndRequest(hreq.get(), NULL, HSR_INITIATE, 0))
        std::cerr << "HttpEndRequest Error" << std::endl;

    return 0;
}