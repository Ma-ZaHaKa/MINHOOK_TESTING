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


// ����������� ���� ������������ �������
typedef int(WINAPI* MessageBoxAType)(HWND, LPCSTR, LPCSTR, UINT);

// ������������ �������
MessageBoxAType MessageBoxA_orig;

// ���� �������-�����������
int WINAPI MessageBoxA_hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    std::cout << "MessageBoxA ������!" << std::endl;

    return 0;
    return MessageBoxA_orig(hWnd, "Hooked!", lpCaption, uType);
}



int main()
{
    InitConsole();

    // ������������� MinHook
    if (MH_Initialize() != MH_OK) {
        std::cerr << "�� ������� ���������������� MinHook" << std::endl;
        return 1;
    }

    // ��������� ��������� �� ������������ �������
    MessageBoxA_orig = MessageBoxA;

    // �������� ���������
    if (MH_CreateHook(&MessageBoxA, &MessageBoxA_hook, reinterpret_cast<LPVOID*>(&MessageBoxA_orig)) != MH_OK) {
        std::cerr << "�� ������� ������� �������� ��� MessageBoxA" << std::endl;
        return 1;
    }

    // ���������� ���������
    if (MH_EnableHook(&MessageBoxA) != MH_OK) {
        std::cerr << "�� ������� ��������� �������� ��� MessageBoxA" << std::endl;
        return 1;
    }

    // ��������� ��������
    //MessageBoxA(nullptr, "Hello, World!", "Original", MB_OK);

    // ������� �������� ����� ����������� ���������
    //MH_DisableHook(&MessageBoxA);

    // ��������������� MinHook
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
        CreateThread(NULL, 0, Entry, NULL, 0, NULL); // ��������� dllPathStr �� ������� � dll
        //main();
    }
    return TRUE; // crash if no return;
}
