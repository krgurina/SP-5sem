#pragma warning(disable : 4996)

#include <iostream>
#include <windows.h>

#include "../OS13_HTCOM_LIB/pch.h"
#include "../OS13_HTCOM_LIB/OS13_HTCOM_LIB.h"

#pragma comment(lib, "../x64/Debug/OS13_HTCOM_LIB.lib")

using namespace std;

wchar_t* getWC(const char* c);

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	try
	{
		OS13_HTCOM_HANDEL h = OS13_HTCOM::Init();

		ht::HtHandle* ht = OS13_HTCOM::HT::create(h, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), getWC(argv[5]));

		if (ht)
		{
			cout << "HT-Storage Created" << endl;
			wcout << "filename: " << ht->fileName << endl;
			cout << "secSnapshotInterval: " << ht->secSnapshotInterval << endl;
			cout << "capacity: " << ht->capacity << endl;
			cout << "maxKeyLength: " << ht->maxKeyLength << endl;
			cout << "maxPayloadLength: " << ht->maxPayloadLength << endl;

			OS13_HTCOM::HT::close(h, ht);
		}
		else
			cout << "-- create: error" << endl;

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
