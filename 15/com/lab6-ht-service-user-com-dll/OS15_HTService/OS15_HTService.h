#pragma once
#include <windows.h>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <string>
#include "sddl.h"

#include "D:/15/lab6-ht-service-user-com-dll/OS15_HTCOM_LIB/pch.h"
#include "D:/15/lab6-ht-service-user-com-dll/OS15_HTCOM_LIB/OS15_HTCOM_LIB.h"

#define SERVICENAME L"OS15HTServ"
#define HTPATH L"D:/15/lab6-ht-service-user-com-dll/storage/HTspace.ht"

#define USERNAME L"HTUser01"
#define PASSWORD L"1111"

#define TRACEPATH L"D:/15/lab6-ht-service-user-com-dll/storage/servise.trace"

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);

SECURITY_ATTRIBUTES getSecurityAttributes();
HANDLE createStopEvent(const wchar_t* stopEventName);
void startService();
void trace(const char* msg, int r = std::ofstream::app);
