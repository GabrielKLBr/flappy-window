#include "messagebox.h"
#include "resource.h"
#include <windows.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>

// Constantes globais
const int MONITOR_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const int MONITOR_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
const int BIRD_SIZE = 20;
const int PIPE_WIDTH = 80;
const int PIPE_GAP = 125;
const int PIPE_SPEED = 4;

struct PipePair
{
    HWND topPipe, bottomPipe;
    bool scored;
};

// Variáveis globais
HWND hwndBird, hwndScore;
std::vector<PipePair> pipePairs;
RECT bird;
int birdSpeed = 0;
int gravity = 1;
int score = 0;
bool isDead = false;
//WS_EX_LAYERED | WS_VISIBLE
// Função para criar uma janela básica
HWND CreateGameWindow(HINSTANCE hInstance, LPCSTR className, int x, int y, int width, int height, DWORD style, LPCSTR title) {
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        className,
        title,                 // Sem título
        style,   // Estilo de janela
        x, y, width, height,
        nullptr, nullptr, hInstance, nullptr
    );
    return hwnd;
}

void HandleFlap(UINT msg, WPARAM wparam)
{
        if (msg == WM_KEYDOWN)
    {
        if (wparam == VK_SPACE || !isDead)
        {
            birdSpeed = -9.5f;
        }
    }
}

// Função para reiniciar o jogo
void ResetGame(HINSTANCE hInstance) {
    
    bird = { MONITOR_WIDTH / 4, MONITOR_HEIGHT / 2, MONITOR_WIDTH / 4 + BIRD_SIZE, MONITOR_HEIGHT / 2 + BIRD_SIZE };
    birdSpeed = 0;
    score = 0;
    isDead = false;

    // Destruir canos antigos e criar novos
    for (auto& pair : pipePairs) {
        DestroyWindow(pair.topPipe);
        DestroyWindow(pair.bottomPipe);
    }
    pipePairs.clear();

    DestroyWindow(hwndBird);
    DestroyWindow(hwndScore);
    hwndBird = CreateGameWindow(hInstance, "BirdWindow", MONITOR_WIDTH / 4, MONITOR_HEIGHT / 2, BIRD_SIZE, BIRD_SIZE, WS_POPUP | WS_VISIBLE, "Bird");
    hwndScore = CreateGameWindow(hInstance, "ScoreWindow", 10, 10, 100, 60, WS_EX_LAYERED | WS_VISIBLE, "Score");

    // Criar o primeiro par de canos
    int gapStart = 100 + rand() % (MONITOR_HEIGHT - PIPE_GAP - 200);
    HWND topPipe = CreateGameWindow(hInstance, "PipeWindow", MONITOR_WIDTH, 0, PIPE_WIDTH, gapStart, WS_EX_LAYERED | WS_VISIBLE, "Top Pipe");
    HWND bottomPipe = CreateGameWindow(hInstance, "PipeWindow", MONITOR_WIDTH, gapStart + PIPE_GAP, PIPE_WIDTH, MONITOR_HEIGHT - gapStart - PIPE_GAP, WS_EX_LAYERED | WS_VISIBLE, "Bottom Pipe");
    pipePairs.push_back({topPipe, bottomPipe, false});
}

// Função auxiliar para verificar se um novo par de canos é necessário
bool IsNewPipeNeeded() {
    if (pipePairs.empty()) return true;

    RECT lastPipeRect;
    GetWindowRect(pipePairs.back().topPipe, &lastPipeRect);
    return (lastPipeRect.right < MONITOR_WIDTH - 300);
}

bool CheckCollision() {
    for (const auto& pair : pipePairs) {
        RECT pipeRectTop, pipeRectBottom;
        GetWindowRect(pair.topPipe, &pipeRectTop);
        GetWindowRect(pair.bottomPipe, &pipeRectBottom);

        RECT intersection;
        if (IntersectRect(&intersection, &bird, &pipeRectTop) || IntersectRect(&intersection, &bird, &pipeRectBottom)) {
            return true; // Colisão detectada
        }
    }
    return false;
}

void UpdateScore() {
    for (auto& pair : pipePairs) {
        if (!pair.scored) { // Se a pontuação ainda não foi contada
            RECT pipeRectTop;
            GetWindowRect(pair.topPipe, &pipeRectTop);
            if (bird.left > pipeRectTop.right) { // Se o pássaro passou o par de canos
                pair.scored = true;
                score++; // Incrementar a pontuação
            }
        }
    }
}
int pipeX, pipeRectLeft;
// Função para atualizar o jogo
void UpdateGame(HINSTANCE hInstance) {
    if (isDead) return;
    SetForegroundWindow(hwndBird);

    // Atualizar a posição do pássaro
    bird.top += birdSpeed;
    bird.bottom += birdSpeed;
    birdSpeed += gravity;
    MoveWindow(hwndBird, bird.left, bird.top, BIRD_SIZE, BIRD_SIZE, TRUE);

    // Atualizar os canos
    for (size_t i = 0; i < pipePairs.size(); i++) {
        RECT pipeRectTop, pipeRectBottom;
        GetWindowRect(pipePairs[i].topPipe, &pipeRectTop);
        GetWindowRect(pipePairs[i].bottomPipe, &pipeRectBottom);
        int x = pipeRectTop.left - PIPE_SPEED;
        pipeRectLeft = pipeRectTop.left;
        pipeX = x;

        // Remover canos fora da tela
        if (x + PIPE_WIDTH < 0) {
            DestroyWindow(pipePairs[i].topPipe);
            DestroyWindow(pipePairs[i].bottomPipe);
            pipePairs.erase(pipePairs.begin() + i);
            i--; // Ajustar o índice devido à remoção
        } else {
            MoveWindow(pipePairs[i].topPipe, x, pipeRectTop.top, PIPE_WIDTH, pipeRectTop.bottom - pipeRectTop.top, TRUE);
            MoveWindow(pipePairs[i].bottomPipe, x, pipeRectBottom.top, PIPE_WIDTH, pipeRectBottom.bottom - pipeRectBottom.top, TRUE);
        }
    }

    // Adicionar novos canos
    if (IsNewPipeNeeded()) {
        int gapStart = 100 + rand() % (MONITOR_HEIGHT - PIPE_GAP - 200);
        HWND topPipe = CreateGameWindow(hInstance, "PipeWindow", MONITOR_WIDTH, 0, PIPE_WIDTH, gapStart, WS_EX_LAYERED | WS_VISIBLE, "Top Pipe");
        HWND bottomPipe = CreateGameWindow(hInstance, "PipeWindow", MONITOR_WIDTH, gapStart + PIPE_GAP, PIPE_WIDTH, MONITOR_HEIGHT - gapStart - PIPE_GAP, WS_EX_LAYERED | WS_VISIBLE, "Bottom Pipe");
        pipePairs.push_back({ topPipe, bottomPipe, false });
    }

    // Verificar colisões e atualizar pontuação
    if (bird.top < 0 || bird.bottom > MONITOR_HEIGHT || CheckCollision()) {
        isDead = true;
        MessageBoxCustom();
    } else {
        UpdateScore();
    }

    // Forçar repintura da janela de pontuação
    InvalidateRect(hwndScore, nullptr, TRUE);
}
  
// Callback para a janela do pássaro
LRESULT CALLBACK BirdWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HBRUSH brush = CreateSolidBrush(RGB(255, 255, 0)); // Cor amarela
        FillRect(hdc, &ps.rcPaint, brush);
        DeleteObject(brush);
        EndPaint(hwnd, &ps);
    }
    HandleFlap(uMsg, wParam);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Callback para a janela dos canos
LRESULT CALLBACK PipeWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0)); // Cor verde
        FillRect(hdc, &ps.rcPaint, brush);
        DeleteObject(brush);
        EndPaint(hwnd, &ps);
    }
    if (uMsg == WM_CLOSE || uMsg == WM_QUIT)
    {
        birdSpeed = 0;
        gravity = 0;
        pipeX = pipeRectLeft;
        MessageBoxCheatCustom();
    }
    HandleFlap(uMsg, wParam);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Callback para a janela de pontuação
LRESULT CALLBACK ScoreWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Criar uma fonte personalizada "Sans Serif"
        HFONT hFont = CreateFont(
            28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Sans Serif");

        // Selecionar a fonte no contexto de dispositivo
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // Atualizar o texto da pontuação
        char scoreText[16];
        sprintf_s(scoreText, "Score: %d", score);
        TextOut(hdc, 5, 2.5 , scoreText, strlen(scoreText));

        // Restaurar a fonte original e deletar a nova fonte
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);

        EndPaint(hwnd, &ps);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Função principal
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(static_cast<unsigned int>(time(0)));

    WNDCLASS wcBird = { CS_HREDRAW | CS_VREDRAW, BirdWindowProc, 0, 0, hInstance, nullptr, nullptr, nullptr, nullptr, "BirdWindow" };
    WNDCLASS wcPipe = { CS_HREDRAW | CS_VREDRAW, PipeWindowProc, 0, 0, hInstance, nullptr, nullptr, nullptr, nullptr, "PipeWindow" };
    WNDCLASS wcScore = { CS_HREDRAW | CS_VREDRAW, ScoreWindowProc, 0, 0, hInstance, nullptr, nullptr, nullptr, nullptr, "ScoreWindow" };
    wcBird.hIcon = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    wcPipe.hIcon = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    wcScore.hIcon = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

    RegisterClass(&wcBird);
    RegisterClass(&wcPipe);
    RegisterClass(&wcScore);

    ResetGame(hInstance);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            UpdateGame(hInstance);
            Sleep(16);
        }
    }

    return 0;
}
