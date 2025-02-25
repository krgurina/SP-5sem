#include "pch.h"
#include "HT.h"

namespace ht
{
	HtHandle::HtHandle()
	{
		this->capacity = 0;
		this->secSnapshotInterval = 0;
		this->maxKeyLength = 0;
		this->maxPayloadLength = 0;
		ZeroMemory(this->fileName, sizeof(this->fileName));
		this->file = NULL;
		this->fileMapping = NULL;
		this->addr = NULL;
		ZeroMemory(this->lastErrorMessage, sizeof(this->lastErrorMessage));
		this->lastSnaptime = 0;

		this->count = 0;
	}

	HtHandle::HtHandle(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const wchar_t* htUsersGroup, const wchar_t* fileName) : HtHandle()
	{
		this->capacity = capacity;
		this->secSnapshotInterval = secSnapshotInterval;
		this->maxKeyLength = maxKeyLength;
		this->maxPayloadLength = maxPayloadLength;
		memcpy(this->fileName, fileName, sizeof(this->fileName));
		memcpy(this->htUsersGroup, htUsersGroup, sizeof(this->htUsersGroup));
	}

	HtHandle* create(
		int	  capacity,					// емкость хранилища
		int   secSnapshotInterval,		// переодичность сохранения в сек.
		int   maxKeyLength,             // максимальный размер ключа
		int   maxPayloadLength,			// максимальный размер данных
		const wchar_t* htUsersGroup,
		const wchar_t* fileName)		// имя файла 
	{
		if (chekingDataForCreateHT(htUsersGroup)) {
			HtHandle* htHandle = createHt(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, htUsersGroup, fileName);
			runSnapshotTimer(htHandle);
			return htHandle;
		}
		else
			return NULL;
	}

#pragma region work_with_groups

	bool checkCurrentUser(const wchar_t* usersGroup)
	{
		bool rc = false;
		DWORD lenUn = 512;
		wchar_t un[512];

		if (!GetUserName(un, &lenUn))
			return rc;

		if (isUserBelongToUsersGroup(un, usersGroup))
			rc = true;

		return rc;
	}

	bool chekingDataForOpenHT(HtHandle* htHandle)
	{
		return checkCurrentUser(htHandle->htUsersGroup);
	}
	bool chekingDataForOpenHT(HtHandle* htHandle, const wchar_t* htUser, const wchar_t* htPassword)
	{
		return isUserBelongToUsersGroup(htUser, htHandle->htUsersGroup) && verifyUser(htUser, htPassword);
	}

	bool isUserBelongToUsersGroup(const wchar_t* user, const wchar_t* usersGroup)
	{
		bool userInGroup = false;

		GROUP_USERS_INFO_0* buf3;
		DWORD uc3 = 0, tc3 = 0;

		NET_API_STATUS ns3 = NetUserGetLocalGroups(NULL, user, 0, LG_INCLUDE_INDIRECT, (LPBYTE*)&buf3, MAX_PREFERRED_LENGTH, &uc3, &tc3);

		if (ns3 == NERR_Success)
			for (int i = 0; i < uc3; i++)
				if (wcscmp(buf3[i].grui0_name, usersGroup) == 0)
				{
					userInGroup = true;
					break;
				}

		NetApiBufferFree(buf3);

		return userInGroup;
	}

	bool verifyUser(const wchar_t* user, const wchar_t* password)
	{
		bool rc = false;
		HANDLE pHandle;

		if (LogonUserW(user, NULL, password, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &pHandle) != 0)
			rc = true;

		return rc;
	}

	bool findMatchByGroupName(const wchar_t* usersGroup)
	{
		bool isMatch = false;

		NET_API_STATUS netStatus;
		PLOCALGROUP_INFO_0 pBuffer;
		DWORD entriesread, totalentries;

		netStatus = NetLocalGroupEnum(NULL, 0, (LPBYTE*)&pBuffer, MAX_PREFERRED_LENGTH, &entriesread, &totalentries, NULL);

		for (int i = 0; i < entriesread; i++)
			if (wcscmp(pBuffer[i].lgrpi0_name, usersGroup) == 0)
			{
				isMatch = true;
				break;
			}

		NetApiBufferFree(pBuffer);

		return isMatch;
	}

	bool chekingDataForCreateHT(const wchar_t* htUsersGroup)
	{
		if (findMatchByGroupName(htUsersGroup))
		{
			if (checkCurrentUser(htUsersGroup) || checkCurrentUser(L"Администраторы"))
				return true;
		}

		return false;
	}
#pragma endregion



	//я должен создать файл для будущей работы с ним,
	//для этого надо создать сам файл, создать сопоставление, и адрестное пространство отображения
	HtHandle* createHt(
		int	  capacity,					// емкость хранилища
		int   secSnapshotInterval,		// переодичность сохранения в сек.
		int   maxKeyLength,             // максимальный размер ключа
		int   maxPayloadLength,			// максимальный размер данных
		const wchar_t* htUsersGroup,
		const wchar_t* fileName)		// имя файла 
	{
		HANDLE hf = CreateFile( //создаём или открываем файл
			fileName,
			GENERIC_WRITE | GENERIC_READ,
			NULL,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hf == INVALID_HANDLE_VALUE)
			throw "create or open file failed";

		int sizeMap = sizeof(HtHandle) + getSizeElement(maxKeyLength, maxPayloadLength) * capacity;
		HANDLE hm = CreateFileMapping(
			hf,
			NULL,
			PAGE_READWRITE,
			0, sizeMap,
			fileName);
		if (!hm)
			throw "create File Mapping failed";

		LPVOID lp = MapViewOfFile(// Отображаем отображенный участок файла в виртуальное адресное пространство процесса
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		ZeroMemory(lp, sizeMap);

		HtHandle* htHandle = new(lp) HtHandle(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, htUsersGroup, fileName);//чтобы htHandle был в нужной области памяти
		htHandle->file = hf;
		htHandle->fileMapping = hm;
		htHandle->addr = lp;
		htHandle->lastSnaptime = time(NULL);
		htHandle->mutex = CreateMutex(
			NULL,
			FALSE,
			fileName);

		return htHandle;
	}

	void CALLBACK snapAsync(LPVOID prm, DWORD, DWORD)
	{
		HtHandle* htHandle = (HtHandle*)prm;
		if (snap(htHandle))
			std::cout << "-- spanshotAsync success" << std::endl;
	}

	HtHandle* openWithoutAuth
	(
		const wchar_t* fileName,         // имя файла
		bool isMapFile)					// true если открыть fileMapping; false если открыть файл; по умолчанию false
	{
		HtHandle* htHandle = NULL;

		if (isMapFile)
		{
			htHandle = openHtFromMapName(fileName);
		}
		else
		{
			htHandle = openHtFromFile(fileName);
			if (htHandle)
				runSnapshotTimer(htHandle);
		}

		return htHandle;
	}

	HtHandle* open
	(
		const wchar_t* fileName,        // имя файла
		const wchar_t* htUser,			// HT-пользователь
		const wchar_t* htPassword,		// пароль
		bool isMapFile)					// true если открыть fileMapping; false если открыть файл; по умолчанию false
	{
		HtHandle* htHandle = openWithoutAuth(fileName, isMapFile);

		if (htHandle)
		{
			if (!chekingDataForOpenHT(htHandle, htUser, htPassword))
			{
				close(htHandle);
				return NULL;
			}
		}

		return htHandle;
	}

	HtHandle* open
	(
		const wchar_t* fileName, bool isMapFile)
	{
		HtHandle* htHandle = openWithoutAuth(fileName, isMapFile);
		if (htHandle)
		{
			if (!chekingDataForOpenHT(htHandle))
			{
				close(htHandle);
				return NULL;
			}
		}

		runSnapshotTimer(htHandle);

		return htHandle;
	}

	HtHandle* openHtFromFile(
		const wchar_t* fileName)
	{
		HANDLE hf = CreateFile(
			fileName,
			GENERIC_WRITE | GENERIC_READ,
			NULL,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hf == INVALID_HANDLE_VALUE)
			return NULL;

		HANDLE hm = CreateFileMapping(
			hf,
			NULL,
			PAGE_READWRITE,
			0, 0,
			fileName);
		if (!hm)
			return NULL;

		LPVOID lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		HtHandle* htHandle = (HtHandle*)lp;
		htHandle->file = hf;
		htHandle->fileMapping = hm;
		htHandle->addr = lp;
		htHandle->lastSnaptime = time(NULL);

		return htHandle;
	}

	HtHandle* openHtFromMapName(
		const wchar_t* fileName)
	{
		HANDLE hm = CreateFileMapping(
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0, sizeof(HtHandle),
			fileName);
		if (!hm)
			return NULL;

		LPVOID lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		HtHandle* htHandle = (HtHandle*)lp;

		int sizeMapping = sizeof(HtHandle) + getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength) * htHandle->capacity;

		if (!UnmapViewOfFile(lp))
			return NULL;
		if (!CloseHandle(hm))
			return NULL;

		hm = CreateFileMapping(//чтобы в отображении получить уже все данные
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0, sizeMapping,
			fileName);
		if (!hm)
			return NULL;

		lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		htHandle = new HtHandle();
		memcpy(htHandle, lp, sizeof(HtHandle));
		htHandle->file = NULL;
		htHandle->fileMapping = hm;
		htHandle->addr = lp;
		htHandle->lastSnaptime = time(NULL);

		return htHandle;
	}
	BOOL changeElementState(const HtHandle* const htHandle, int index) {
		int removedState = static_cast<int>(State::Removed);
		LPVOID lp = htHandle->addr;

		lp = (HtHandle*)lp + 1;
		lp = (byte*)lp + getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength) * index;
		lp = (byte*)lp + htHandle->maxKeyLength;
		lp = (int*)lp + 1;
		lp = (byte*)lp + htHandle->maxPayloadLength;
		lp = (int*)lp + 1;
		memcpy(lp, &removedState, sizeof(int));
		return true;
	}
	BOOL runSnapshotTimer(HtHandle* htHandle)
	{
		htHandle->snapshotTimer = CreateWaitableTimer(0, FALSE, 0);
		LARGE_INTEGER Li{};
		Li.QuadPart = -(SECOND * htHandle->secSnapshotInterval);
		SetWaitableTimer(htHandle->snapshotTimer, &Li, 1, snapAsync, htHandle, FALSE);

		return true;
	}

	Element* get     //  читать элемент из хранилища
	(
		HtHandle* htHandle,            // управление HT
		const Element* element)              // элемент 
	{
		int index = findIndex(htHandle, element);
		if (index < 0)
		{
			writeLastError(htHandle, "-- not found element (GET)");
			return NULL;
		}

		Element* foundElement = new Element();
		readFromMemory(htHandle, foundElement, index);

		return foundElement;
	}

	BOOL insert		// добавить элемент в хранилище
	(
		HtHandle* htHandle,            // управление HT
		const Element* element)              // элемент
	{
		if (htHandle->count >= htHandle->capacity)
		{
			writeLastError(htHandle, "-- not found free memory");
			return false;
		}

		WaitForSingleObject(htHandle->mutex, INFINITE);
		int copyIndex = findIndex(htHandle, element);
		if (copyIndex > 0)
		{
			writeLastError(htHandle, "-- found element(insert)");
			return NULL;
		}
		int freeIndex = findFreeIndex(htHandle, element);
		writeToMemory(htHandle, element, freeIndex);
		incrementCount(htHandle);
		ReleaseMutex(htHandle->mutex);

		return true;
	}

	BOOL update     //  именить элемент в хранилище
	(
		HtHandle* htHandle,            // управление HT
		const Element* oldElement,          // старый элемент (ключ, размер ключа)
		const void* newPayload,          // новые данные  
		int             newPayloadLength)     // размер новых данных
	{
		WaitForSingleObject(htHandle->mutex, INFINITE);
		int index = findIndex(htHandle, oldElement);
		if (index < 0)
		{
			writeLastError(htHandle, "-- not found element (UPDATE)");
			ReleaseMutex(htHandle->mutex);
			return false;
		}

		Element* updateElement = new Element(oldElement, newPayload, newPayloadLength);
		writeToMemory(htHandle, updateElement, index);
		ReleaseMutex(htHandle->mutex);

		return true;
	}

	BOOL remove      // удалить элемент в хранилище
	(
		HtHandle* htHandle,            // управление HT (ключ)
		const Element* element)				 // элемент 
	{
		WaitForSingleObject(htHandle->mutex, INFINITE);
		int index = findIndex(htHandle, element);
		if (index < 0)
		{
			writeLastError(htHandle, "-- not found element (DELETE)");
			ReleaseMutex(htHandle->mutex);
			return false;
		}
		changeElementState(htHandle, index);
		//clearMemoryByIndex(htHandle, index);
		decrementCount(htHandle);
		ReleaseMutex(htHandle->mutex);

		return true;
	}

	BOOL snap         // выполнить Snapshot
	(
		HtHandle* htHandle)           // управление HT (File, FileMapping)
	{
		WaitForSingleObject(htHandle->mutex, INFINITE);
		if (!FlushViewOfFile(htHandle->addr, NULL)) {
			writeLastError(htHandle, "-- snapshot error");
			ReleaseMutex(htHandle->mutex);
			return false;
		}
		htHandle->lastSnaptime = time(NULL);
		ReleaseMutex(htHandle->mutex);
		return true;
	}

	void print                               // распечатать элемент 
	(
		const Element* element)              // элемент 
	{
		std::cout << "Element:" << std::endl;
		std::cout << "{" << std::endl;
		std::cout << "\t\"key\": \"" << (char*)element->key << "\"," << std::endl;
		std::cout << "\t\"keyLength\": " << element->keyLength << "," << std::endl;
		std::cout << "\t\"payload\": \"" << (char*)element->payload << "\"," << std::endl;
		std::cout << "\t\"payloadLength\": " << element->payloadLength << std::endl;
		std::cout << "\t\"state\": " << (int)element->state << std::endl;
	}

	BOOL close
	(
		const HtHandle* htHandle)
	{
		HANDLE hf = htHandle->file;
		HANDLE hfm = htHandle->fileMapping;

		if (!CancelWaitableTimer(htHandle->snapshotTimer))
			throw "cancel snapshotTimer failed";
		if (!UnmapViewOfFile(htHandle->addr))
			throw "unmapping view failed";
		if (!CloseHandle(hfm))
			throw "close File Mapping failed";
		if (hf != NULL)
			if (!CloseHandle(hf))
				throw "close File failed";

		return true;
	}

	int hashFunction(const char* key, int capacity)
	{
		unsigned long i = 0;
		for (int j = 0; key[j]; j++)
			i += key[j];
		return i % capacity;
	}

	int nextHash(int currentHash, const char* key, int capacity)
	{
		return ++currentHash;
	}

	int findFreeIndex(
		const HtHandle* htHandle,           // управление HT
		const Element* element)				// элемент
	{
		int index = hashFunction((char*)element->key, htHandle->capacity);

		Element* foundElement = new Element();
		do
		{
			if (index >= htHandle->capacity)
			{
				index = -1;
				break;
			}

			readFromMemory(htHandle, foundElement, index);
			index = nextHash(index, (char*)element->key, htHandle->capacity);
		} while (foundElement->state == State::Active ?
			(foundElement->keyLength != NULL &&
				foundElement->payloadLength != NULL) : false);

		delete foundElement;
		return index - 1;
	}

	int findIndex(
		const HtHandle* htHandle,           // управление HT
		const Element* element)				// элемент
	{
		int index = hashFunction((char*)element->key, htHandle->capacity);

		Element* foundElement = new Element();
		foundElement->keyLength = -1;
		do
		{
			if (index >= htHandle->capacity || (foundElement->keyLength == NULL && foundElement->payloadLength == NULL))
			{
				index = -1;
				break;
			}

			readFromMemory(htHandle, foundElement, index);
			index = nextHash(index, (char*)element->key, htHandle->capacity);


		} while (foundElement->state == State::Active ? (
			memcmp((char*)foundElement->key, element->key, element->keyLength) != NULL) : true);

		delete foundElement;
		return index - 1;
	}

	BOOL writeToMemory(const HtHandle* const htHandle, const Element* const element, int index)
	{
		LPVOID lp = htHandle->addr;

		lp = (HtHandle*)lp + 1;
		lp = (byte*)lp + getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength) * index;

		memcpy(lp, element->key, element->keyLength);
		lp = (byte*)lp + htHandle->maxKeyLength;
		memcpy(lp, &element->keyLength, sizeof(int));
		lp = (int*)lp + 1;
		memcpy(lp, element->payload, element->payloadLength);
		lp = (byte*)lp + htHandle->maxPayloadLength;
		memcpy(lp, &element->payloadLength, sizeof(int));
		lp = (int*)lp + 1;
		memcpy(lp, &element->state, sizeof(int));

		return true;
	}

	int incrementCount(HtHandle* const htHandle)
	{
		return ++htHandle->count;
	}

	Element* readFromMemory(const HtHandle* const htHandle, Element* const element, int index)
	{
		LPVOID lp = htHandle->addr;

		lp = (HtHandle*)lp + 1;//перемещаемся на размер одного HtHandle
		lp = (byte*)lp + getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength) * index;

		element->key = lp;
		lp = (byte*)lp + htHandle->maxKeyLength;
		element->keyLength = *(int*)lp;
		lp = (int*)lp + 1;
		element->payload = lp;
		lp = (byte*)lp + htHandle->maxPayloadLength;
		element->payloadLength = *(int*)lp;
		lp = (int*)lp + 1;
		element->state = static_cast<State>(*(int*)lp);


		return element;
	}


	BOOL clearMemoryByIndex(const HtHandle* const htHandle, int index)
	{
		LPVOID lp = htHandle->addr;
		int sizeElement = getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength);

		lp = (HtHandle*)lp + 1;
		lp = (byte*)lp + sizeElement * index;

		ZeroMemory(lp, sizeElement);

		return true;
	}

	int decrementCount(HtHandle* const htHandle)
	{
		return --htHandle->count;
	}

	const char* writeLastError(HtHandle* const htHandle, const char* msg)
	{
		memcpy(htHandle->lastErrorMessage, msg, sizeof(htHandle->lastErrorMessage));
		return htHandle->lastErrorMessage;
	}

	const char* getLastError  // получить сообщение о последней ошибке
	(
		const HtHandle* htHandle)				// управление HT
	{
		return htHandle->lastErrorMessage;
	}
}
