#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("変更"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR szDefaultPrinter[256] = { 0 };
			DWORD dwSize = _countof(szDefaultPrinter);
			if (GetDefaultPrinter(szDefaultPrinter, &dwSize))
			{
				HANDLE hPrinter;
				PRINTER_DEFAULTS pd = { 0 };
				pd.DesiredAccess = PRINTER_ALL_ACCESS;
				if (OpenPrinter(szDefaultPrinter, &hPrinter, &pd) != 0)
				{
					const DWORD dwLevel = 2;
					DWORD dwNeeded;
					GetPrinter(hPrinter, dwLevel, 0, 0, &dwNeeded);
					LPBYTE pPrinter = (LPBYTE)LocalAlloc(LPTR, dwNeeded);
					if (GetPrinter(hPrinter, dwLevel, pPrinter, dwNeeded, &dwNeeded) != 0)
					{
						PRINTER_INFO_2 *pInfo = (PRINTER_INFO_2 *)pPrinter;
						if (pInfo->Attributes & PRINTER_ATTRIBUTE_ENABLE_BIDI)
						{
							pInfo->Attributes &= ~PRINTER_ATTRIBUTE_ENABLE_BIDI;
							SetPrinter(hPrinter, dwLevel, pPrinter, 0);
						}
						LocalFree(pPrinter);
					}
					ClosePrinter(hPrinter);
				}
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("デフォルトプリンターの設定を変更"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
