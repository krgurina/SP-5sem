#define _CRT_SECURE_NO_WARNINGS 
#include <windows.h> 
#include <iostream> 
#include <float.h> 

using namespace std;
int main()
{
	setlocale(LC_ALL, "RU");
	double a = 2.1;
	double b = 10.2;
	double res = 0;
	int cw = _controlfp(0, 0);
	// разрешить обработку исключений с плавающей точкой 
	cw &= ~(EM_OVERFLOW | EM_ZERODIVIDE);
	// установить новое управляющее слово 
	_controlfp(cw, _MCW_EM);

	while (true) {
		cout << "\nВведите делимое (или введите -1 для выхода): ";
		cin >> b;

		if (b == -1) {
			break;  
		}

		cout << "Введите делитель: ";
		cin >> a;

		__try
		{
			res = b / a;
			cout << "Результат: " << res << endl;
		}

		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DWORD ec = GetExceptionCode(); 
			if (ec == EXCEPTION_FLT_DIVIDE_BY_ZERO)
				cout << "Ошибка деления на ноль!" << endl;
			else
				cout << "Другая ошибка: " << ec << endl;
		}
		
	}
	return 0;
}
