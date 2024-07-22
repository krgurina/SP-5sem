#include "tests.h"

namespace tests
{
	// вставка элемента
	BOOL test1(ht::HtHandle* htHandle)
	{
		ht::Element* insertEl = new ht::Element("test1", 6, "test", 5);

		ht::insert(htHandle, insertEl);
		ht::Element* getEl = ht::get(htHandle, new ht::Element("test1", 6));

		if (
			getEl == NULL ||
			insertEl->keyLength != getEl->keyLength ||
			memcmp(insertEl->key, getEl->key, insertEl->keyLength) != NULL ||
			insertEl->payloadLength != getEl->payloadLength ||
			memcmp(insertEl->payload, getEl->payload, insertEl->payloadLength) != NULL
			)
			return false;

		return true;
	}


	// удаление элемента
	BOOL test2(ht::HtHandle* htHandle)
	{
		ht::Element* element = new ht::Element("test2", 6, "test2", 6);

		ht::insert(htHandle, element);
		ht::removeOne(htHandle, element);
		if (ht::get(htHandle, element) != NULL)
			return false;

		return true;
	}

	// обновление элемента
	BOOL test3(ht::HtHandle* htHandle)
	{
		// Создаем элемент с ключом "testKey" и значением "testValue1"
		ht::Element* insertEl = new ht::Element("testKey", 8, "testValue1", 11);

		ht::insert(htHandle, insertEl);


		// Обновляем элемент с ключом "testKey"
		ht::update(htHandle, insertEl, "testValue2", 11);


		// Получаем элемент с ключом "testKey"
		ht::Element* getEl = ht::get(htHandle, new ht::Element("testKey", 8));


		// Проверяем, что полученное значение соответствует новому значению элемента
		if (getEl == NULL || getEl->payloadLength != 11 || memcmp(getEl->payload, "testValue2", getEl->payloadLength) != 0)
		{
			return false;
		}

		return true;
	}
	
	// получение элемента
	BOOL test4(ht::HtHandle* htHandle)
	{
		// Создаем элемент и вставляем его в хеш-таблицу
		ht::Element* insertEl = new ht::Element("t4", 3, "test", 5);
		ht::insert(htHandle, insertEl);
		// Получаем элемент по существующему ключу
		ht::Element* getEl = ht::get(htHandle, new ht::Element("t4", 3));

		// Проверяем, что полученный элемент соответствует ожидаемому
		if (getEl == NULL || getEl->payloadLength != 5 || memcmp(getEl->payload, "test", getEl->payloadLength) != 0)
		{
			return false;
		}

		return true;

	}


	// -------------------------------------------------------------------


	// ключ больше максимального значения ключа в хранилище
	BOOL test5(ht::HtHandle* htHandle)
	{
		const char* key = "test5test5test5test5test5";
		const char* value = "test";
		ht::Element* insertEl = new ht::Element(key, strlen(key), value, strlen(value));

		if (!ht::insert(htHandle, insertEl)) {
			const char* expectedError = "-- key size exceeds the maximum allowed";
			const char* actualError = ht::getLastError(htHandle);
			if (strcmp(expectedError, actualError) == 0) {
				return true;
			}
			else {
				return false;
			}
		}
	}

	// получение несуществующего элемента
	BOOL test6(ht::HtHandle* htHandle)
	{
		ht::Element* element = new ht::Element("test7", 6);

		if (!(ht::get(htHandle, element)))
		{
			const char* expectedError = "-- not found element (GET)";
			const char* actualError = ht::getLastError(htHandle);
			if (strcmp(expectedError, actualError) == 0) {
				return true;
			}
			else {
				return false;
			}
		}
	}

	// удаление несуществующего элемента
	BOOL test7(ht::HtHandle* htHandle)
	{
		ht::Element* element = new ht::Element("test8", 6, "test8", 6);

		ht::insert(htHandle, element);
		ht::removeOne(htHandle, element);
		if (!(ht::removeOne(htHandle, element)))
		{
			const char* expectedError = "-- not found element (DELETE)";
			const char* actualError = ht::getLastError(htHandle);
			if (strcmp(expectedError, actualError) == 0) {
				return true;
			}
			else {
				return false;
			}
		}
	}

}