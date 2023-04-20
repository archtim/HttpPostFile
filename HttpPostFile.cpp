// HttpPostFile.cpp : This simple Visual C++ console program sends a binary file (e.g. jpg) to a REST endpoint.
// The code defines two structures, FileCloser and InetCloser, which are used to close file handles and internet 
// handles respectively when they are no longer needed. The code also defines two functions, WCharToChar and WriteToInternet, 
// which are used to convert wide-character strings to multibyte strings and write data to an internet handle respectively.
// Finally, the code defines a function called PostFile which takes four parameters: a pointer to a null-terminated string that 
// specifies the file name of the file to be uploaded, a pointer to a null-terminated string that specifies the 
// server name or IP address of the server that will receive the file, an integer that specifies the port number of the server, 
// and a pointer to a null-terminated string that specifies the route on the server where the file will be uploaded.
//

#include <iostream>
#include "PostFile.h"

int main()
{
    CPostFile FileSender;
    const char* szServer = "127.0.0.1";
    INTERNET_PORT wPort = 5000;
    const char* szRoute = "/upload";
    const char* szFile = "Images/famous-ace-pilot.jpg";
    bool bError = FileSender.PostFile(szFile, szServer, wPort, szRoute);
    if (!bError)
        std::cout << "File sent successfully!\n";
    else
        std::cout << "Failed to send file!\n";
}

