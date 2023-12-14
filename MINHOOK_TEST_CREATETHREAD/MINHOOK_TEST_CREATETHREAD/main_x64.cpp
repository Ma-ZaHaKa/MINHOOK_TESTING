#include <Windows.h>
#include <iostream>
#include "MinHook.h"
#pragma comment(lib, "minhook.x64.lib")

HANDLE InitConsole() // with proto
{
    AllocConsole();

    //SetConsoleOutputCP(866);
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);


    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    return hConsole;
}


// Определение типа оригинальной функции
typedef int(WINAPI* MessageBoxAType)(HWND, LPCSTR, LPCSTR, UINT);

// Оригинальная функция
MessageBoxAType MessageBoxA_orig;

// Наша функция-перехватчик
int WINAPI MessageBoxA_hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    std::cout << "MessageBoxA вызван!" << std::endl;

    return 0;
    return MessageBoxA_orig(hWnd, "Hooked!", lpCaption, uType);
}



int main()
{
    InitConsole();

    // Инициализация MinHook
    if (MH_Initialize() != MH_OK) {
        std::cerr << "Не удалось инициализировать MinHook" << std::endl;
        return 1;
    }

    // Получение указателя на оригинальную функцию
    MessageBoxA_orig = MessageBoxA;

    // Создание перехвата
    if (MH_CreateHook(&MessageBoxA, &MessageBoxA_hook, reinterpret_cast<LPVOID*>(&MessageBoxA_orig)) != MH_OK) {
        std::cerr << "Не удалось создать перехват для MessageBoxA" << std::endl;
        return 1;
    }

    // Применение перехвата
    if (MH_EnableHook(&MessageBoxA) != MH_OK) {
        std::cerr << "Не удалось применить перехват для MessageBoxA" << std::endl;
        return 1;
    }

    // Тестируем перехват
    //MessageBoxA(nullptr, "Hello, World!", "Original", MB_OK);

    // Убираем перехват перед завершением программы
    //MH_DisableHook(&MessageBoxA);

    // Деинициализация MinHook
    //MH_Uninitialize();

    return 0;
}


DWORD CALLBACK Entry(LPVOID)
{
    main();
    return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        //DisableThreadLibraryCalls(hinstDLL);
        CreateThread(NULL, 0, Entry, NULL, 0, NULL); // указатель dllPathStr на каталог с dll
        //main();
    }
    return TRUE; // crash if no return;
}
