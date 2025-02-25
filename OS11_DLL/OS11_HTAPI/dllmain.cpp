#include "pch.h"

// dllmain.cpp : Определяет точку входа для приложения DLL.
BOOL APIENTRY DllMain( HMODULE hModule, //дескриптор модуля DLL
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:    // Вызывается при подключении DLL к процессу
    case DLL_THREAD_ATTACH:     // Вызывается, когда поток создается в процессе
    case DLL_THREAD_DETACH:     // Вызывается при завершении потока в процессе
    case DLL_PROCESS_DETACH:    // Вызывается при отключении DLL от процесса
        break;
    }
    return TRUE;
}

