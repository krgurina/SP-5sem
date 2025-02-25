#pragma once


namespace ht
{
	enum class State {
		Active,
		Removed
	};
	struct Element   // элемент 
	{
		Element();
		Element(const void* key, int keyLength);                                             // for get
		Element(const void* key, int keyLength, const void* payload, int  payloadLength);    // for insert
		Element(const Element* oldElement, const void* newPayload, int  newPayloadLength);   // for update
		State state;
		const void* key;                 // значение ключа 
		int         keyLength;           // рахмер ключа
		const void* payload;             // данные 
		int         payloadLength;       // размер данных
	};

	int getSizeElement(
		int   maxKeyLength,                // максимальный размер ключа
		int   maxPayloadLength);           // максимальный размер данных
}