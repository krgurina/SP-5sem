#include <Windows.h>
#include <iostream>

#define SERVICENAME L"OS15HTServ"

#ifdef _WIN64
#define SERVICEPATH L"D:/15/lab6-ht-service-user-com-dll/x64/Debug/OS15_HTService.exe"
#else
#define SERVICEPATH L"D:/15/lab6-ht-service-user-com-dll/Debug/OS15_HTService.exe"
#endif

char* errortxt(const char* msg, int code)
{
	char* buf = new char[512];

	sprintf_s(buf, 512, "%s: error code = %d\n", msg, code);

	return buf;
}

int main()
{
	SC_HANDLE schService = NULL, schSCManager = NULL;
	try
	{
		//Открытие менеджера служб 
		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		//1. имя удаленной машины
		//2.
		//3.  права доступа к менеджеру служб
		if (!schSCManager)
		{
			throw errortxt("OpenSCManager", GetLastError());
		}
		else
		{
			schService = CreateService(
				schSCManager,	//дескриптор менеджера служб
				SERVICENAME,	//имя, по которому служба будет известна системе
				SERVICENAME,	// используется для отображения службы в пользовательском интерфейсе
				SERVICE_ALL_ACCESS,	//требуемые права доступа к службе
				SERVICE_WIN32_SHARE_PROCESS,	//тип службы. служба может разделять свой процесс с другими службами
				SERVICE_AUTO_START,		//автоматический запуск службы при старте системы
				SERVICE_ERROR_NORMAL,	// определяет, как система реагирует на ошибки службы
				SERVICEPATH,	//путь к исполняемому файлу службы
				NULL,	//Имя группы загрузки, к которой принадлежит служба
				NULL,	//Уникальный идентификатор, присвоенный системой службе
				NULL,	//Массив имен служб, от которых зависит создаваемая служба
				NULL,	// Имя учетной записи, от имени которой будет запущена служба
				NULL	//Пароль учетной записи, от имени которой будет запущена служба
			);	

			if (!schService)
			{
				throw errortxt("CreateService", GetLastError());
			}
		}
	}
	catch (char* txt)
	{
		std::cout << txt << std::endl;
	}


	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
	}

	if (schService)
	{
		CloseServiceHandle(schService);
	}
	system("pause");

	return 0;
}