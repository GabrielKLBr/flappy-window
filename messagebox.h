#include "resource.h"
#include <windows.h>

void ResetGame(HINSTANCE hInstance);

LRESULT CALLBACK MessageBoxWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            exit(0);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // O ID 1 corresponde ao botão "OK"
                // Ação executada quando o botão "OK" é pressionado
                ResetGame(NULL);
                DestroyWindow(hwnd);
            } else if (LOWORD(wParam) == 2) {
                exit(0);
            }
            break;
        case WM_KEYDOWN:
            if (wParam == VK_SPACE || wParam == VK_ACCEPT)
                {
                    ResetGame(NULL);
                    DestroyWindow(hwnd);
                }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MessageBoxCheatWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            exit(0);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // O ID 1 corresponde ao botão "OK"
                exit(0);
                PostQuitMessage(0);
                break;
            }
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void MessageBoxCustom()
{
    int scrWidth = GetSystemMetrics(SM_CXSCREEN);
    int scrHeight = GetSystemMetrics(SM_CYSCREEN);

    // Registrar a classe de janela
    const char* className = "MessageBoxCustomClass";
    WNDCLASS wc = { };
    wc.lpfnWndProc = MessageBoxWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    RegisterClass(&wc);

    // Criar a janela sem botões de minimizar/maximizar
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST, className, "Flappy Window", WS_OVERLAPPED | WS_SYSMENU, // Título da janela e sem botões de minimizar/maximizar
        (scrWidth - 248) / 2, (scrHeight - 149) / 2, 248, 149,
        NULL, NULL, wc.hInstance, NULL
    );

    MessageBeep(MB_ICONERROR);

    // Carregar o ícone de informação
    HICON hIcon = LoadIcon(NULL, IDI_INFORMATION);
    HICON hIconWindow = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconWindow);  // Configura o ícone pequeno
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconWindow);  // Configura o ícone grande

    // Texto da mensagem
    const char* message = "Game Over, click ok to restart";

    HFONT hFontText, hFontButton;

    hFontText = CreateFont(15, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
    hFontButton = CreateFont(12, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");

    // Criar o controle de ícone (STATIC) à esquerda
    HWND hwndIcon = CreateWindow(
        "STATIC", "", WS_VISIBLE | WS_CHILD | SS_ICON,
        28, 25, 32, 32, hwnd, NULL, wc.hInstance, NULL
    );
    SendMessage(hwndIcon, STM_SETICON, (WPARAM)hIcon, 0);

    // Criar o controle de texto à direita do ícone
    HWND hwndStatic = CreateWindow(
        "STATIC", message, WS_VISIBLE | WS_CHILD | SS_LEFT,
        70, 35, 180, 50, hwnd, NULL, wc.hInstance, NULL
    );
    SendMessage(hwndStatic, WM_SETFONT, (WPARAM)hFontText, TRUE);

    // Exibir o botão OK (simulando a MessageBox) mais à direita
    HWND hwndButton = CreateWindow(
        "BUTTON", "OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        80, 90, 70, 22, hwnd, (HMENU)1, wc.hInstance, NULL
    );
    SendMessage(hwndButton, WM_SETFONT, (WPARAM)hFontButton, TRUE);

    HWND hwndQuitButton = CreateWindow(
        "BUTTON", "Quit", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        158, 90, 70, 22, hwnd, (HMENU)2, wc.hInstance, NULL
    );
    SendMessage(hwndQuitButton, WM_SETFONT, (WPARAM)hFontButton, TRUE);

    // Exibir a janela
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    // Loop de mensagens
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Quando o botão OK for pressionado, fechamos a janela
        if (msg.message == WM_COMMAND && LOWORD(msg.wParam) == 1) {
            DestroyWindow(hwnd);
        }
    }
}

void MessageBoxCheatCustom()
{
    int scrWidth = GetSystemMetrics(SM_CXSCREEN);
    int scrHeight = GetSystemMetrics(SM_CYSCREEN);

    // Registrar a classe de janela
    const char* className = "MessageBoxCustomClass";
    WNDCLASS wc = { };
    wc.lpfnWndProc = MessageBoxWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    RegisterClass(&wc);

    // Criar a janela sem botões de minimizar/maximizar
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST, className, "CHEATER!!!", WS_OVERLAPPED | WS_SYSMENU, // Título da janela e sem botões de minimizar/maximizar
        (scrWidth - 248) / 2, (scrHeight - 149) / 2, 248, 149,
        NULL, NULL, wc.hInstance, NULL
    );

    MessageBeep(MB_ICONERROR);

    // Carregar o ícone de informação
    HICON hIcon = LoadIcon(NULL, IDI_ERROR);
    HICON hIconWindow = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconWindow);  // Configura o ícone pequeno
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconWindow);  // Configura o ícone grande

    // Texto da mensagem
    const char* message = "CHEATERS AREN'T ALLOWED!!";

    HFONT hFontText, hFontButton;

    hFontText = CreateFont(15, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
    hFontButton = CreateFont(12, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");

    // Criar o controle de ícone (STATIC) à esquerda
    HWND hwndIcon = CreateWindow(
        "STATIC", "", WS_VISIBLE | WS_CHILD | SS_ICON,
        28, 35, 32, 32, hwnd, NULL, wc.hInstance, NULL
    );
    SendMessage(hwndIcon, STM_SETICON, (WPARAM)hIcon, 0);

    // Criar o controle de texto à direita do ícone
    HWND hwndStatic = CreateWindow(
        "STATIC", message, WS_VISIBLE | WS_CHILD | SS_LEFT,
        70, 45, 180, 50, hwnd, NULL, wc.hInstance, NULL
    );
    SendMessage(hwndStatic, WM_SETFONT, (WPARAM)hFontText, TRUE);

    // Exibir a janela
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    // Loop de mensagens
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Quando o botão OK for pressionado, fechamos a janela
        if (msg.message == WM_COMMAND && LOWORD(msg.wParam) == 1) {
            DestroyWindow(hwnd);
        }
    }
}