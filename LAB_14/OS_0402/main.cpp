#pragma warning(disable : 4996)

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include "sddl.h"

#include "../OS14_HTCOM_LIB/pch.h"
#include "../OS14_HTCOM_LIB/OS14_HTCOM_LIB.h"

#ifdef _WIN64
#pragma comment(lib, "../x64/Debug/OS14_HTCOM_LIB.lib")
#else
#pragma comment(lib, "../Debug/OS14_HTCOM_LIB.lib")
#endif

using namespace std;

string intToString(int number);
wchar_t* getWC(const char* c);
wchar_t* getFileName(const char* c);

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
			path = "E:/уник/СП/LAB_14/storage/test3.ht";
			ht = OS14_HTCOM::HT::open(h, L"E:/уник/СП/LAB_14/storage/test3.ht", L"HTUser01", L"1111", true);
		}
		//ht::HtHandle* ht = OS14_HTCOM::HT::open(h, L"test.ht", true);
		//ht::HtHandle* ht = OS14_HTCOM::HT::open(h, L"test.ht", L"HTUser02", L"1111", true);
		if (ht)
			cout << "-- open: success" << endl;
		else
			throw "-- open: error";

		HANDLE event = CreateEvent(NULL, TRUE, FALSE, getFileName(path));

		while (WaitForSingleObject(event, 0) == WAIT_TIMEOUT) {
			int numberKey = rand() % 50;
			string key = intToString(numberKey);
			cout << key << endl;

			ht::Element* element = OS14_HTCOM::Element::createInsertElement(h, key.c_str(), key.length() + 1, "0", 2);
			if (OS14_HTCOM::HT::insert(h, ht, element))
				cout << "-- insert: success" << endl;
			else
				cout << "-- insert: error" << endl;

			delete element;

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