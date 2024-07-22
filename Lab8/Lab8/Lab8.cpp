#include <windows.h> 
#include <iostream> 
#include <float.h> 
#include <excpt.h>

using namespace std;
int main()
{
	setlocale(LC_ALL, "RU");
	double a = 0;
	double b = 0;
	double res = 0;

	unsigned int cw;
	_controlfp_s(&cw, 0, 0);
	cw &= ~(EM_OVERFLOW | EM_ZERODIVIDE);
	// установить новое управляющее слово 
	_controlfp_s(&cw, cw, _MCW_EM);

	while (true) {
		cout << "\nВведите делимое (или введите -1 для выхода): ";
		cin >> b;

		if (b == -1) {
			break;  
		}

		cout << "Введите делитель: ";
		cin >> a;

		if (cin.fail()) {
			cin.clear();
			cin.ignore(INT_MAX, '\n'); 
			cout << "Попробуйте ещё раз." << endl;
		}
		else {
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

	}
	return 0;
}