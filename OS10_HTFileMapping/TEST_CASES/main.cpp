#include "tests.h"

using namespace std;

int main()

{
	ht::HtHandle* ht = ht::create(6, 4, 12, 256, L"./files/HTspace.ht");
	if (ht)
		cout << "-- create: success" << endl;
	else
		throw "-- create: error";




	if (tests::test1(ht))
		cout << "-- test1: success" << endl;
	else
		cout << "-- test1: error" << endl;

	if (tests::test2(ht))
		cout << "-- test2: success" << endl;
	else
		cout << "-- test2: error" << endl;

	if (tests::test3(ht))
		cout << "-- test3: success" << endl;
	else
	{
		cout << "-- test3: error" << endl;
	}

	if (tests::test4(ht))
		cout << "-- test4: success" << endl;
	else
	{
		cout << "-- test4: error" << endl;
	}




	cout << "==========================================================" << endl;




	if (tests::test5(ht))
		cout << "-- test5: success" << endl;
	else
	{
		cout << "-- test5: error" << endl;
		printf("Error: %s\n", ht::getLastError(ht));
	}

	if (tests::test6(ht))
		cout << "-- test6: success" << endl;
	else
	{
		cout << "-- test6: error" << endl;
		printf("Error: %s\n", ht::getLastError(ht));
	}

	if (tests::test7(ht))
		cout << "-- test7: success" << endl;
	else
	{
		cout << "-- test7: error" << endl;
		printf("Error: %s\n", ht::getLastError(ht));
	}


	if (ht != nullptr)
		if (ht::close(ht))
			cout << "-- close: success" << endl;
		else
			throw "-- close: error";
}