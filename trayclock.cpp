#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>
#include <stdint.h>
#include <time.h>

static const int ICON_W = 16;
static const int ICON_H = 16;

static HICON hIcon = nullptr;
static NOTIFYICONDATAW nid = {};
static HWND hWnd = nullptr;

/*
7px-high bitmap font, width = 4px per digit
1 uses x=3 vertical stroke, foot width = 4
*/
static const uint8_t font[10][7] = {
/*0*/ {0b1111, 0b1001, 0b1001, 0b1001, 0b1001, 0b1001, 0b1111},
/*1*/ {0b0010, 0b0110, 0b1010, 0b0010, 0b0010, 0b0010, 0b1111},
/*2*/ {0b1111, 0b0001, 0b0001, 0b1111, 0b1000, 0b1000, 0b1111},
/*3*/ {0b1111, 0b0001, 0b0001, 0b1111, 0b0001, 0b0001, 0b1111},
/*4*/ {0b1001, 0b1001, 0b1001, 0b1111, 0b0001, 0b0001, 0b0001},
/*5*/ {0b1111, 0b1000, 0b1000, 0b1111, 0b0001, 0b0001, 0b1111},
/*6*/ {0b1111, 0b1000, 0b1000, 0b1111, 0b1001, 0b1001, 0b1111},
/*7*/ {0b1111, 0b0001, 0b0001, 0b0001, 0b0001, 0b0001, 0b0001},
/*8*/ {0b1111, 0b1001, 0b1001, 0b1111, 0b1001, 0b1001, 0b1111},
/*9*/ {0b1111, 0b1001, 0b1001, 0b1111, 0b0001, 0b0001, 0b1111}
};

static void DrawDigit(uint32_t* px, int ox, int oy, int digit, uint32_t color) {
    for (int y = 0; y < 7; y++) {
        uint8_t row = font[digit][y];
        for (int x = 0; x < 4; x++) {
            if (row & (1 << (3 - x))) {
                int ix = ox + x;
                int iy = oy + y;
                if (ix >= 0 && ix < ICON_W && iy >= 0 && iy < ICON_H)
                    px[iy * ICON_W + ix] = color;
            }
        }
    }
}

static void UpdateIcon() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    uint32_t pixels[ICON_W * ICON_H];

    const uint32_t BG = 0xFFF5F5F5;
//    const uint32_t FG = 0xFF6A6A6A;
    const uint32_t FG = 0xFF1E1E1E;

    for (int i = 0; i < ICON_W * ICON_H; i++)
        pixels[i] = BG;

    int h1 = st.wHour / 10;
    int h2 = st.wHour % 10;
    int m1 = st.wMinute / 10;
    int m2 = st.wMinute % 10;

    // layout: 4px digit + 2px gap + 4px digit = 10px total
    int total_w = 10;
    int x0 = (ICON_W - total_w) / 2;

    DrawDigit(pixels, x0,     0, h1, FG);
    DrawDigit(pixels, x0 + 6, 0, h2, FG);
    DrawDigit(pixels, x0,     8, m1, FG);
    DrawDigit(pixels, x0 + 6, 8, m2, FG);

    HBITMAP colorBmp = CreateBitmap(
        ICON_W, ICON_H, 1, 32, pixels
    );
    HBITMAP maskBmp = CreateBitmap(
        ICON_W, ICON_H, 1, 1, nullptr
    );

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = colorBmp;
    ii.hbmMask = maskBmp;

    if (hIcon)
        DestroyIcon(hIcon);

    hIcon = CreateIconIndirect(&ii);

    DeleteObject(colorBmp);
    DeleteObject(maskBmp);

    nid.hIcon = hIcon;

    wsprintfW(
        nid.szTip,
        L"%04d-%02d-%02d %02d:%02d",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute
    );

    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TIMER)
        UpdateIcon();
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"TrayClockWnd";
    RegisterClassW(&wc);

    hWnd = CreateWindowW(
        wc.lpszClassName, L"",
        WS_OVERLAPPED, 0, 0, 0, 0,
        nullptr, nullptr, hInst, nullptr
    );

    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;

    UpdateIcon();
    Shell_NotifyIconW(NIM_ADD, &nid);

    SetTimer(hWnd, 1, 1000, nullptr);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIconW(NIM_DELETE, &nid);
    if (hIcon) DestroyIcon(hIcon);
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"TrayClockWnd";
    RegisterClassW(&wc);

    hWnd = CreateWindowW(
        wc.lpszClassName, L"",
        WS_OVERLAPPED, 0, 0, 0, 0,
        nullptr, nullptr, hInst, nullptr
    );

    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;

    UpdateIcon();
    Shell_NotifyIconW(NIM_ADD, &nid);

    SetTimer(hWnd, 1, 1000, nullptr);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIconW(NIM_DELETE, &nid);
    if (hIcon) DestroyIcon(hIcon);
    return 0;
}

