#pragma warning(disable : 4996)

#include <iostream>
#include <windows.h>
#include <conio.h>

#include <string>
#include <sstream>
#include <fstream>

#include "../OS13_HTCOM_LIB/pch.h"
#include "../OS13_HTCOM_LIB/OS13_HTCOM_LIB.h"

#pragma comment(lib, "../x64/Debug/OS13_HTCOM_LIB.lib")

using namespace std;

string intToString(int number);
wchar_t* getWC(const char* c);
wchar_t* getFileName(const char* c);

int main(int argc, char* argv[])
{
	//HANDLE hStopEvent = CreateEvent(NULL,
	//	TRUE, //FALSE - автоматический сброс; TRUE - ручной
	//	FALSE,
	//	L"Stop");

	setlocale(LC_ALL, "Russian");

	try
	{
		OS13_HTCOM_HANDEL h = OS13_HTCOM::Init();

		ht::HtHandle* ht = nullptr;
		//wchar_t* fileName = getWC(argv[1]);
		const char* fileName = argv[1];
		ht = OS13_HTCOM::HT::open(h, getWC(fileName), false);
		HANDLE hStopEvent = CreateEvent(NULL, TRUE, FALSE, getFileName(fileName));

		if (ht)
		{
			cout << "HT-Storage Start" << endl;
			wcout << "filename: " << ht->fileName << endl;
			cout << "secSnapshotInterval: " << ht->secSnapshotInterval << endl;
			cout << "capacity: " << ht->capacity << endl;
			cout << "maxKeyLength: " << ht->maxKeyLength << endl;
			cout << "maxPayloadLength: " << ht->maxPayloadLength << endl;

			while (!kbhit() && WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT)
				SleepEx(0, TRUE);

			OS13_HTCOM::HT::snap(h, ht);
			OS13_HTCOM::HT::close(h, ht);
		}
		else
			cout << "-- open: error" << endl;

		OS13_HTCOM::Dispose(h);
	}
	catch (const char* e) { cout << e << endl; }
	catch (int e) { cout << "HRESULT: " << e << endl; }

}

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}
wchar_t* getFileName(const char* c)
{
	const char* lastSlash = strrchr(c, '/');
	const char* filename = (lastSlash != NULL) ? lastSlash + 1 : c;
	return getWC(filename);
}

string intToString(int number)
{
	stringstream convert;
	convert << number;

	return convert.str();
}
