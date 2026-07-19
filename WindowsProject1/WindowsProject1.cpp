// WindowsProject1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WindowsProject1.h"
#include "resource.h"
#include "RestartableScheduledAction.h"
#include <cstdarg>
#include <cstdio>

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス

// このコード モジュールに含まれる関数の宣言を転送します:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);

// デバッグ出力にログを出力する関数
void LogDebug(const wchar_t* format, ...)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t timeBuffer[32];
    swprintf_s(timeBuffer, L"%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, format, args);
    va_end(args);

    OutputDebugStringW(timeBuffer);
    OutputDebugStringW(L" ");
    OutputDebugStringW(buffer);
    OutputDebugStringW(L"\n");
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    hInst = hInstance;

    DialogBox(hInst, L"MyTestDlgBase_Main", NULL, (DLGPROC)MyDlgProc);

    return (int)0;
}


RestartableScheduledAction action(
    []
    {
        LogDebug(L"=============!!!Executing action!!!=============");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        LogDebug(L"=============!!!Finish action!!!=============");
    },
    std::chrono::seconds(5)
);

// ダイアログプロシージャ
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDOK:
            EndDialog(hDlg, IDOK);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        case IDC_BUTTON1:
            LogDebug(L"IDC_BUTTON1 pressed: ScheduleAfterDelay");
            action.ScheduleAfterDelay();
            return TRUE;
        case IDC_BUTTON2:
            LogDebug(L"IDC_BUTTON2 pressed: Cancel");
            action.Cancel();
            return TRUE;
        }
    }
    return FALSE;
}
