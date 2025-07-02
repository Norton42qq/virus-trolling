#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

const int FONT_SIZE = 16;
const wchar_t* DISPLAY_TEXT = L"ВЗЛОМ ЖОПЫ";
const int MATRIX_LENGTH = 20;

int screenWidth, screenHeight;
HDC desktopDC;

struct MatrixColumn {
    int x;          // позиция X
    float y;        // позиция падения
    float speed;    // скорость падения
};

// взлом жопы
void drawCenteredText(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 0, 0));

    HFONT hFont = CreateFontW(
        72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    SIZE textSize;
    GetTextExtentPoint32W(hdc, DISPLAY_TEXT, wcslen(DISPLAY_TEXT), &textSize);

    int x = (screenWidth - textSize.cx) / 2;
    int y = (screenHeight - textSize.cy) / 2;

    TextOutW(hdc, x, y, DISPLAY_TEXT, wcslen(DISPLAY_TEXT));

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

// матрица
void drawMatrix(std::vector<MatrixColumn>& columns, HDC hdc, int charHeight, int charWidth) {
    SetBkMode(hdc, TRANSPARENT);

    HFONT hFont = CreateFontW(
        FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    for (auto& col : columns) {
        for (int i = 0; i < MATRIX_LENGTH; ++i) {
            int posY = (int)(col.y) - i;
            if (posY < 0) continue;

            int px = col.x * charWidth;
            int py = posY * charHeight;

            if (py > screenHeight) continue;

            wchar_t ch = (rand() % 2) ? L'1' : L'0';

            int intensity = max(0, 255 - i * 12);

            SetTextColor(hdc, RGB(0, intensity, 0));

            TextOutW(hdc, px, py, &ch, 1);
        }

        col.y += col.speed;
        if (col.y > screenHeight / charHeight + MATRIX_LENGTH) {
            col.y = 0;
            col.speed = 0.5f + static_cast<float>(rand()) / RAND_MAX * 1.5f;
        }
    }

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

// инверсия
void flickerScreen(HDC hdc) {
    PatBlt(hdc, 0, 0, screenWidth, screenHeight, DSTINVERT);
}

void jerkCursor(int width, int height) {
    int x = rand() % width;
    int y = rand() % height;
    SetCursorPos(x, y);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    HWND desktop = GetDesktopWindow();
    desktopDC = GetDC(desktop);

    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    srand((unsigned)time(0));

    int charHeight = FONT_SIZE + 4;
    int charWidth = FONT_SIZE / 1.5;

    int columnsCount = screenWidth / charWidth;

    std::vector<MatrixColumn> columns;
    for (int i = 0; i < columnsCount; ++i) {
        columns.push_back({ i, static_cast<float>(rand() % screenHeight), 0.5f + static_cast<float>(rand()) / RAND_MAX * 1.5f });
    }

    int flickerCounter = 0;

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        if (flickerCounter++ > 10) {
            flickerScreen(desktopDC);
            flickerCounter = 0;
        }

        drawMatrix(columns, desktopDC, charHeight, charWidth);
        drawCenteredText(desktopDC);
        jerkCursor(screenWidth, screenHeight);

        Sleep(50);
    }

    ReleaseDC(desktop, desktopDC);
    return 0;
}
