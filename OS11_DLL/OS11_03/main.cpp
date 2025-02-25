#pragma comment(lib, "../x64/debug/OS11_HTAPI.lib")
#include <string>
#include "../OS11_HTAPI/pch.h"
#include "../OS11_HTAPI/HT.h"

using namespace std;

void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* inValue, bool inSuccess);

wchar_t* getWC(const char* c);

int main(int argc, char* argv[])
{
	try
	{
		wchar_t* fileName = getWC(argv[1]);

		ht::HtHandle* ht = ht::open(fileName, true);
		if (ht)
			cout << "-- open: success" << endl;
		else
			throw "-- open: error";


		HANDLE log_file_handler = CreateFile(L"../files/delete.log", GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (log_file_handler == INVALID_HANDLE_VALUE)
		{
			cout << "can't open log file: " << endl;
			system("pause");
			return 0;
		}


		char* elementKey = new char[2];
		char elementValue[] = "0";

		bool success_result;

		while (true)
		{
			elementKey[0] = '0' + rand() % 50;
			elementKey[1] = '\0';

			ht::Element* element = ht::createGetElement(elementKey, 2);

			if (removeOne(ht, element))
			{
				cout << "-- remove: success" << endl;
				cout << elementKey << endl;
				success_result = true;
			}
			else
			{
				cout << "-- remove: error" << endl;
				success_result = false;
			}


			WriteRowInLog(log_file_handler, elementKey, elementValue, success_result);


			delete element;
			Sleep(1000);
		}

		delete[] elementKey;
		if (!CloseHandle(log_file_handler)) throw " log_file_handler failed";
	}



	catch (const char* msg)
	{
		cout << msg << endl;
	}
}



void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* inValue, bool inSuccess)
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
		message += "   [Delete]   Key = ";
		message += inKey;
		message += "   Payload = ";
		message += inValue;
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

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}