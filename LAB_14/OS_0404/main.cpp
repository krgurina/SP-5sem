#pragma warning(disable : 4996)

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>

#include "../OS14_HTCOM_LIB/pch.h"
#include "../OS14_HTCOM_LIB/OS14_HTCOM_LIB.h"

#ifdef _WIN64
#pragma comment(lib, "../x64/Debug/OS14_HTCOM_LIB.lib")
#else
#pragma comment(lib, "../Debug/OS14_HTCOM_LIB.lib")
#endif

using namespace std;

wchar_t* getWC(const char* c);
wchar_t* getFileName(const char* c);
string intToString(int number);
int charToInt(char* str);
string incrementPayload(char* str);

int main(int argc, char* argv[])
{

	setlocale(LC_ALL, "Russian");

	try
	{
		OS14_HTCOM_HANDEL h = OS14_HTCOM::Init();
		ht::HtHandle* ht;
		const char* path;
		if (argc > 1) {
			path = argv[1];
			ht = OS14_HTCOM::HT::open(h, getWC(argv[1]), true);
			//ht = OS14_HTCOM::HT::open(h, getWC(argv[1]), L"HTUser01", L"1111", true);
		}
		else {
			path = "E:/уник/СП/LAB_14/storage/test.ht";
			ht = OS14_HTCOM::HT::open(h, L"E:/уник/СП/LAB_14/storage/test.ht", L"HTUser01", L"1111", true);
		}
		//ht::HtHandle* ht = OS14_HTCOM::HT::open(h, L"../storage/HTspace.ht", true);
		//ht::HtHandle* ht = OS14_HTCOM::HT::open(h, L"test.ht", L"user", L"user", true);
		if (ht)
			cout << "-- open: success" << endl;
		else
			throw "-- open: error";

		HANDLE event = CreateEvent(NULL, TRUE, FALSE, getFileName(path));

		while (WaitForSingleObject(event, 0) == WAIT_TIMEOUT) {
			int numberKey = rand() % 50;
			string key = intToString(numberKey);
			cout << key << endl;

			ht::Element* element = OS14_HTCOM::HT::get(h, ht, OS14_HTCOM::Element::createGetElement(h, key.c_str(), key.length() + 1));
			if (element)
			{
				cout << "-- get: success" << endl;
				string newPayload = incrementPayload((char*)element->payload);

				if (OS14_HTCOM::HT::update(h, ht, element, newPayload.c_str(), newPayload.length() + 1))
					cout << "-- update: success" << endl;
				else
					cout << "-- update: error" << endl;
			}
			else
				cout << "-- get: error" << endl;

			Sleep(1000);
		}

		OS14_HTCOM::HT::close(h, ht);

		OS14_HTCOM::Dispose(h);
	}
	catch (const char* e) { cout << e << endl; }
	catch (int e) { cout << "HRESULT: " << e << endl; }

}

string intToString(int number)
{
	stringstream convert;
	convert << number;

	return convert.str();
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