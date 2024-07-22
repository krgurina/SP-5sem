#include <iostream>
#include <Windows.h>

using namespace std;

int main() {
    setlocale(LC_ALL, "RU");

    int a = 10;
    int* p = new int;
    delete p;
    __try {
        cout << "Адрес указателя: " << p << endl;
        *p = a;

    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        DWORD ec = GetExceptionCode(); 
        if (ec == EXCEPTION_ACCESS_VIOLATION)
        {
            cout << "Ошибка доступа к памяти!" << endl;
        }
        else
            cout << "Другая ошибка: " << ec << endl;
    }


    return 0;
}