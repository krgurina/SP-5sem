#pragma warning(disable : 4996)

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>

#include "../OS13_HTCOM_LIB/pch.h"
#include "../OS13_HTCOM_LIB/OS13_HTCOM_LIB.h"

#pragma comment(lib, "../x64/Debug/OS13_HTCOM_LIB.lib")

using namespace std;

void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* oldPayload, const char* newPayload, bool inSuccess);
string intToString(int number);
int charToInt(char* str);
string incrementPayload(char* str);
wchar_t* getWC(const char* c);
wchar_t* getFileName(const char* c);

int main(int argc, char* argv[])
{

	setlocale(LC_ALL, "Russian");

	try
	{
		const char* path;
		ht::HtHandle* ht;
		OS13_HTCOM_HANDEL h = OS13_HTCOM::Init();

		if (argc > 1) {
			path = argv[1];
			ht = OS13_HTCOM::HT::open(h, getWC(argv[1]), true);
		}
		else {
			path = "E:/уник/СП/OS_Lab13/lab4-ht-com-dll/x64/files/test.ht";
			ht = OS13_HTCOM::HT::open(h, L"E:/уник/СП/OS_Lab13/lab4-ht-com-dll/x64/files/test.ht", true);
		}


		if (ht)
			cout << "-- open: success" << endl;
		else
			throw "-- open: error";

		/*HANDLE log_file_handler = CreateFile(L"E:/уник/СП/OS_Lab13/lab4-ht-com-dll/x64/files/update.log", GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (log_file_handler == INVALID_HANDLE_VALUE)
		{
			cout << "can't open log file: " << endl;
			system("pause");
			return 0;
		}*/

		HANDLE hStopEvent = CreateEvent(NULL, TRUE, FALSE, getFileName(path));

		while (WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT) {
			srand(time(NULL));
			int numberKey = rand() % 50;
			string key = intToString(numberKey);
			cout << key << endl;

			ht::Element* element = OS13_HTCOM::HT::get(h, ht, OS13_HTCOM::Element::createGetElement(h, key.c_str(), key.length() + 1));
			if (element)
			{
				cout << "-- get: success" << endl;
				string newPayload = incrementPayload((char*)element->payload);

				if (OS13_HTCOM::HT::update(h, ht, element, newPayload.c_str(), newPayload.length() + 1))
				{
					cout << "-- update: success" << endl;
					//WriteRowInLog(log_file_handler, key.c_str(), "0", newPayload.c_str(), true );
				}
					
				else
				{
					cout << "-- update: error" << endl;
					//WriteRowInLog(log_file_handler, key.c_str(), "0", newPayload.c_str(), false);

				}
			}	
			else
				cout << "-- get: error" << endl;

			Sleep(1000);
		}

		//if (!CloseHandle(log_file_handler)) throw " log_file_handler failed";
		OS13_HTCOM::HT::close(h, ht);

		OS13_HTCOM::Dispose(h);
	}
	catch (const char* e) { cout << e << endl; }
	catch (int e) { cout << "HRESULT: " << e << endl; }

}



void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* oldPayload, const char* newPayload, bool inSuccess)
{
	try
	{
		char time_buffer[80]{ 0 };

		time_t seconds = time(NULL);
		tm* time_info = localtime(&seconds);
		const char* format = "%e.%m.%Y %H:%M:%S";
		strftime(time_buffer, 80, format, time_info);

		string message("");

		message += time_buffer;
		message += "   [Update]   Key = ";
		message += inKey;
		message += "   oldPayload = ";
		message += oldPayload;
		message += "   newPayload = ";
		message += newPayload;
		inSuccess ? message += "   success\n" : message += "   error\n";


		LARGE_INTEGER file_pointer;
		file_pointer.QuadPart = 0;
		if (!SetFilePointerEx(inLogFileHandler, file_pointer, NULL, FILE_END))
		{
			throw "SetFilePointerEx return False";
		}

		if (!SetEndOfFile(inLogFileHandler))
		{
			throw "SetEndOfFile return False";
		}

		DWORD bytesWritten;
		WriteFile(inLogFileHandler, message.c_str(), message.length(), &bytesWritten, NULL);
	}

	catch (const char* msg)
	{
		cout << msg << endl;
	}
}



int charToInt(char* str)
{
	stringstream convert;
	convert << str;
	int num;
	convert >> num;

	return num;
}

string incrementPayload(char* str)
{
	int oldNumberPayload = charToInt(str);
	int newNumberPayload = oldNumberPayload + 1;
	string newPayload = intToString(newNumberPayload);

	return newPayload;
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