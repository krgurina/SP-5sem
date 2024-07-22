// по лекции смелова

//#include <iostream>
//#include <Windows.h>
//#include <eh.h>
//#include <tchar.h>
//
//using namespace std;
//
//void myFunc(unsigned int c, EXCEPTION_POINTERS* ep)
//{
//    throw "error";
//}
//
//void (*f)(unsigned int, EXCEPTION_POINTERS*);
//
//
//
//int _tmain(int argc, _TCHAR* argv[]) {
//    setlocale(LC_ALL, "RU");
//    int a = 10;
//    int* p = new int;
//    delete p;
//    // устанавливаем функцию-транслятор 
//    f=_set_se_translator(myFunc);
//
//    try {
//        // Попытка доступа к памяти
//        cout << "Адрес указателя: " << p << endl;
//        *p = a;
//    }
//    catch (char*e) {
//        cout << "Caught error!\n";
//    }
//  
//    catch (...) {
//        cout << "Caught an exception!\n";
//    }
//
//    _se_translator_function(f);
//    system("pause");
//    return 0;
//}





#include <windows.h> 
#include <iostream> 
#include <eh.h> 
using namespace std;

void se_trans_func(unsigned code, EXCEPTION_POINTERS*)
{
	throw code;
}

int main()
{
	setlocale(LC_ALL, "RU");

	int a = 10;
	int* p = new int;
	delete p;
	// устанавливаем функцию-транслятор 
	_set_se_translator(se_trans_func);

	try
	{
		cout << "Адрес указателя: " << p << endl;
		*p = a;
	}
	catch (unsigned code)
	{
		if (code == EXCEPTION_ACCESS_VIOLATION)
        {
            cout << "Ошибка доступа к памяти!" << endl;
            p = &a;
        }
        else
            cout << "Другая ошибка: " << code << endl;
    
		cout << "Exception code = " << hex << code << endl;
	}
	return 0;
}







// это раьотает
//#include <iostream>
//#include <Windows.h>
//#include <eh.h>
//
//using namespace std;
//
//int main() {
//    setlocale(LC_ALL, "RU");
//
//    int* p = nullptr;
//    
//    try {
//
//        cout << "Адрес указателя: " << p << endl;
//        cout << "Разыменование указателя: " << *p << endl;
//        //*p = 10;
//    }
//
//    catch (...) {
//        cout << "Caught an exception!\n";
//    }
//
//    return 0;
//}