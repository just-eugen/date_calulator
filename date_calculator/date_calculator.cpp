#include "framework.h"
#include "date_calculator.h"

#define MAX_LOADSTRING 100
#define ONE_DAY 864000000000 // количество 100 наносекундных интервалов в дне
#define RADIX 10

TCHAR string_part_of_date_copy[MAX_LOADSTRING];

INT_PTR CALLBACK    Calculate(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK       EnumCalendarInfoProc(LPTSTR);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, Calculate);

    int i = GetLastError();

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DATECALCULATOR));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

INT_PTR CALLBACK Calculate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    const TCHAR space[2] = { ' ', '\0' };
    const TCHAR comma[2] = { ',', '\0' };

    TCHAR string_date[MAX_LOADSTRING] = { 0 };
    TCHAR buffer[MAX_LOADSTRING / 10] = { 0 };
    ULARGE_INTEGER ui_max;

    ui_max.QuadPart = MAXINT64;
    
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        SYSTEMTIME system_time;

        GetLocalTime(&system_time);

        if (system_time.wDayOfWeek == 0)
        {
            EnumCalendarInfo(EnumCalendarInfoProc, LOCALE_USER_DEFAULT, CAL_GREGORIAN, CAL_SDAYNAME7);
        }
        else
        {
            EnumCalendarInfo(EnumCalendarInfoProc, LOCALE_USER_DEFAULT, CAL_GREGORIAN,
                CAL_SDAYNAME1 + system_time.wDayOfWeek - 1);
        }

        lstrcat(string_date, string_part_of_date_copy);
        lstrcat(string_date, comma);
        lstrcat(string_date, space);

        _itot_s(system_time.wDay, buffer, RADIX);

        lstrcat(string_date, buffer);
        lstrcat(string_date, space);

        EnumCalendarInfo(EnumCalendarInfoProc, LOCALE_USER_DEFAULT, CAL_GREGORIAN,
            CAL_SMONTHNAME1 + system_time.wMonth - 1);

        lstrcat(string_date, string_part_of_date_copy);
        lstrcat(string_date, space);

        _itot_s(system_time.wYear, buffer, RADIX);

        lstrcat(string_date, buffer);

        SetDlgItemText(hDlg, IDC_EDIT1, string_date);
        SendDlgItemMessage(hDlg, IDC_EDIT2, EM_SETLIMITTEXT, 9, NULL);
    }
    return (INT_PTR)TRUE;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON1:
        {
            INT days;
            SYSTEMTIME system_time;
            FILETIME file_time;
            ULARGE_INTEGER ui;
            INT64 diff;

            GetLocalTime(&system_time);
            SystemTimeToFileTime(&system_time, &file_time);

            ui.LowPart = file_time.dwLowDateTime;
            ui.HighPart = file_time.dwHighDateTime;
            days = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, TRUE);

            if (days == 0)
            {
                SetDlgItemInt(hDlg, IDC_EDIT2, 0, TRUE);
            }

            diff = ui_max.QuadPart - ui.QuadPart;

            if (days > diff / ONE_DAY)
            {
                SetDlgItemInt(hDlg, IDC_EDIT2, (UINT)(diff / ONE_DAY), TRUE);
                days = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, TRUE);
            }

            diff = -1 * ui.QuadPart;

            if (days < diff / ONE_DAY)
            {
                SetDlgItemInt(hDlg, IDC_EDIT2, (UINT)(diff / ONE_DAY), TRUE);
                days = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, TRUE);
            }

            ui.QuadPart += days * ONE_DAY;
            file_time.dwLowDateTime = ui.LowPart;
            file_time.dwHighDateTime = ui.HighPart;

            FileTimeToSystemTime(&file_time, &system_time);

            if (system_time.wDayOfWeek == 0)
            {
                EnumCalendarInfo(EnumCalendarInfoProc, LOCALE_USER_DEFAULT, CAL_GREGORIAN, CAL_SDAYNAME7);
            }
            else
            {
                EnumCalendarInfo(EnumCalendarInfoProc, LOCALE_USER_DEFAULT, CAL_GREGORIAN,
                    CAL_SDAYNAME1 + system_time.wDayOfWeek - 1);
            }

            lstrcat(string_date, string_part_of_date_copy);
            lstrcat(string_date, comma);
            lstrcat(string_date, space);

            _itot_s(system_time.wDay, buffer, RADIX);

            lstrcat(string_date, buffer);
            lstrcat(string_date, space);

            EnumCalendarInfo(EnumCalendarInfoProc, LOCALE_USER_DEFAULT, CAL_GREGORIAN,
                CAL_SMONTHNAME1 + system_time.wMonth - 1);

            lstrcat(string_date, string_part_of_date_copy);
            lstrcat(string_date, space);

            _itot_s(system_time.wYear, buffer, RADIX);

            lstrcat(string_date, buffer);

            SetDlgItemText(hDlg, IDC_EDIT3, string_date);
        }
        return (INT_PTR)TRUE;

        case IDCANCEL:
        {
            DestroyWindow(hDlg);
            return (INT_PTR)FALSE;
        }
        break;

        default:
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return (INT_PTR)TRUE;
    }

    return (INT_PTR)FALSE;
}

BOOL CALLBACK EnumCalendarInfoProc(LPTSTR string_part_of_date)
{
    lstrcpy(string_part_of_date_copy, string_part_of_date);
    return TRUE;
}