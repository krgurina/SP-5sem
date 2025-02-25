#pragma warning(disable : 4996)
#include <windows.h>
#include <iostream>

wchar_t* getWC(const char* c)
{
    wchar_t* wc = new wchar_t[strlen(c) + 1];
    mbstowcs(wc, c, strlen(c) + 1);

    return wc;
}


int main()
{
	char path[256];

	HANDLE hStopEvent = CreateEvent(NULL,
		TRUE, //FALSE - автоматический сброс; TRUE - ручной
		FALSE,
		L"Stop");
    int answer;
    bool flagExit = true;
    while (flagExit) {
        std::cout << "1 - Close Start\n0 - Close programm" << std::endl;
        std::cin >> answer;
        switch (answer)
        {
        case 1:
            std::cout << "cin name of file" << std::endl;
            std::cin >> path;
            hStopEvent = CreateEvent(NULL, TRUE, FALSE, getWC(path));
            SetEvent(hStopEvent);
            break;
        case 0:
            //SetEvent(hStopEvent);
            flagExit = false;
            break;
        default:
            break;
        }
    }
}
