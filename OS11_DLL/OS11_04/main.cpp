#pragma comment(lib, "../x64/debug/OS11_HTAPI.lib")
#include <sstream>
#include <string>
#include "../OS11_HTAPI/pch.h"
#include "../OS11_HTAPI/HT.h"

using namespace std;

void WriteRowInLog(HANDLE inLogFileHandler, const char* inKey, const char* oldPayload, const char* newPayload, bool inSuccess);
wchar_t* getWC(const char* c);
string incrementPayload(char* str);

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


		HANDLE log_file_handler = CreateFile(L"../files/update.log", GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (log_file_handler == INVALID_HANDLE_VALUE)
		{
			cout << "can't open log file: " << endl;
			system("pause");
			return 0;
		}


		char* elementKey = new char[2];
		bool success_result;

		while (true)
		{
			elementKey[0] = '0' + rand() % 50;
			elementKey[1] = '\0';

			ht::Element* element = ht::createGetElement(elementKey, 2);
			ht::HtHandle snap();
			ht::Element* getElement = ht::get(ht, element);

			if (getElement)
			{
				cout << "-- get: success" << endl;
				string oldPayload(static_cast<const char*>(getElement->payload), getElement->payloadLength);
				string newPayload = incrementPayload((char*)getElement->payload);

				if (update(ht, getElement, newPayload.c_str(), newPayload.length() + 1))
				{
					cout << "-- update: success" << endl;
					success_result = true;
				}
				else
				{
					cout << "-- update: error" << endl;
					success_result = false;
				}

				WriteRowInLog(log_file_handler, elementKey, oldPayload.c_str(), newPayload.c_str(), success_result);
			}
			else
			{
				cout << "-- get: error" << endl;
				success_result = false;

				WriteRowInLog(log_file_handler, elementKey, "-", "-", success_result);
			}

			delete element;
			Sleep(1000);
		}

		delete[] elementKey;
	}



	catch (const char* msg)
	{
		cout << msg << endl;
	}
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

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}

string incrementPayload(char* str)
{
	// преобразование строки в число
	int num = atoi(str);
	// инкремент числа
	num++;
	// преобразование числа обратно в строку
	stringstream ss;
	ss << num;
	string result = ss.str();
	return result;
}