#include "OS15_HTService.h"

#ifdef _WIN64
#pragma comment(lib, "D:/15/lab6-ht-service-user-com-dll/x64/Debug/OS15_HTCOM_LIB.lib")
#else
#pragma comment(lib, "D:/15/lab6-ht-service-user-com-dll/Debug/OS15_HTCOM_LIB.lib")
#endif

using namespace std;	

WCHAR ServiceName[] = SERVICENAME;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
SERVICE_STATUS serviceStatus;

HANDLE hStopEvent = createStopEvent(HTPATH);

// функция будет вызвана при запуске службы
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	char temp[121];

	serviceStatus.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;	//служба является процессом
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;	//указывает, что служба находится в процессе запуска.
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;	// указывают, какие операции управления службой принимаются
	serviceStatus.dwWin32ExitCode = 0;	//коды завершения службы
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;	//для сообщения контроллеру служб о статусе запуска.
	serviceStatus.dwWaitHint = 0;	//для сообщения контроллеру служб о статусе запуска.

	//Регистрация обработчика управления службой 
	if (!(hServiceStatusHandle = RegisterServiceCtrlHandler(ServiceName, ServiceHandler)))
	{
		sprintf_s(temp, "RegisterServiceCtrlHandler failed, error code = %d\n", GetLastError());	//для формирования строки с сообщением об ошибке
		trace(temp);
	}
	else
	{

		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		serviceStatus.dwCheckPoint = 0;
		serviceStatus.dwWaitHint = 0;

		//для уведомления контроллера служб о текущем статусе службы
		if (!SetServiceStatus(hServiceStatusHandle, &serviceStatus))
		{
			sprintf_s(temp, "SetServiceStatus failed, error code = %d\n", GetLastError());
			trace(temp);
		}
		else
		{
			trace("Start service", std::ofstream::out);
			startService();
		}
	}
}

VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	char temp[121];

	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
	{
		// Обработка кодов остановки и выключения компьютера
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		serviceStatus.dwCheckPoint = 0;
		serviceStatus.dwWaitHint = 0;

		SetEvent(hStopEvent);	 // Установка события остановки службы
		trace("Stop service");

		break;
	}
	case SERVICE_CONTROL_PAUSE:
		// Обработка кода приостановки службы
		serviceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		// Обработка кода возобновления работы службы
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		// Обработка кода запроса информации о состоянии службы
		break;
	default:
		sprintf_s(temp, "Unrecognized opcode %d\n", fdwControl);
		trace(temp);
	};

	if (!SetServiceStatus(hServiceStatusHandle, &serviceStatus))
	{
		sprintf_s(temp, "SetServiceStatus failed, error code = %d\n", GetLastError());
		trace(temp);
	}
}

void startService()
{
	setlocale(LC_ALL, "Russian");

	try
	{
		cout << "Инициализация компонента:" << endl;
		OS15_HTCOM_HANDEL h = OS15_HTCOM::Init();

		ht::HtHandle* ht = OS15_HTCOM::HT::open(h, HTPATH, USERNAME, PASSWORD);

		if (ht)
		{
			trace("HT-Storage Start\n");
			trace("secSnapshotInterval: ");
			trace(std::to_string(ht->secSnapshotInterval).c_str());
			trace("\ncapacity: ");
			trace(std::to_string(ht->capacity).c_str());

			while (WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT)
				SleepEx(0, TRUE);

			OS15_HTCOM::HT::snap(h, ht);
			OS15_HTCOM::HT::close(h, ht);

			trace("Close ht");
		}
		else
			trace("-- open: error\n");

		cout << endl << "Удалить компонент и выгрузить dll, если можно:" << endl;
		OS15_HTCOM::Dispose(h);
	}
	catch (const char* e) { cout << e << endl; }
	catch (int e) { cout << "HRESULT: " << e << endl; }
}

SECURITY_ATTRIBUTES getSecurityAttributes()
{
	const wchar_t* sdd = L"D:"
		L"(D;OICI;GA;;;BG)" //Deny guests
		L"(D;OICI;GA;;;AN)" //Deny anonymous
		L"(A;OICI;GA;;;AU)" //Allow read, write and execute for Users
		L"(A;OICI;GA;;;BA)"; //Allow all for Administrators
	SECURITY_ATTRIBUTES SA;
	ZeroMemory(&SA, sizeof(SA));
	SA.nLength = sizeof(SA);
	// для преобразования строки SDDL в структуру безопасности
	ConvertStringSecurityDescriptorToSecurityDescriptor(
		sdd,				// Строка SDDL
		SDDL_REVISION_1,	// Версия формата SDDL
		&SA.lpSecurityDescriptor,	// Указатель на SECURITY_DESCRIPTOR
		NULL);

	return SA;
}

HANDLE createStopEvent(const wchar_t* stopEventName)
{
	std::wstring closeEventName = L"Global\\"; closeEventName += stopEventName; closeEventName += L"-stopEvent";
	SECURITY_ATTRIBUTES SA = getSecurityAttributes();

	HANDLE hStopEvent = CreateEvent(
		&SA,
		TRUE, //FALSE - автоматический сброс; TRUE - ручной
		FALSE,
		closeEventName.c_str());	// Имя события

	return hStopEvent;
}

void trace(const char* msg, int r)
{
	std::ofstream out;

	out.open(TRACEPATH, r);
	out << msg << std::endl;

	out.close();
}