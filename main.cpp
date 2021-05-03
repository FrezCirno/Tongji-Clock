#include <windows.h>
#include <cstdio>

constexpr auto CLIENT_WIDTH = 400;
constexpr auto CLIENT_HEIGHT = 150;
constexpr auto TRANS_COLOR = RGB(249, 201, 201);

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

HBRUSH bgBrush = NULL;
RECT wndSize = { 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };
HDC dc;
SYSTEMTIME time;

wchar_t str[64];

// �˴���ģ���а����ĺ�����ǰ������:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ��ʼ��ȫ���ַ���
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	// ����Ϣѭ��:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//KillTimer(hWnd, 1);
	//DeleteObject(bgBrush);
	//ReleaseDC(hWnd, dc);

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	
	wcex.hIcon = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Tongji Clock";
	wcex.hIconSm = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_APPLICATION));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	//��ȡ���������С  
	RECT r;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
	r.left = r.right - CLIENT_WIDTH;
	r.top = r.bottom - CLIENT_HEIGHT;

	HWND hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		szWindowClass, nullptr, WS_POPUP,
		r.left, r.top, CLIENT_WIDTH, CLIENT_HEIGHT,
		GetDesktopWindow(), nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	SetLayeredWindowAttributes(hWnd, TRANS_COLOR, 255, LWA_COLORKEY);
	bgBrush = CreateSolidBrush(TRANS_COLOR);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

DATE ToDate(int year, WORD month, WORD day, WORD h = 0, WORD m = 0, WORD s = 0, WORD ms = 0)
{
	SYSTEMTIME stm = { year, month, 0, day, h, m, s, ms };
	DATE res;
	if (!SystemTimeToVariantTime(&stm, &res)) {/* Handle error */ }
	return res;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		{
			SetTimer(hWnd, 1, 1000, NULL); //�趨ʱ��
			dc = GetDC(hWnd);
			SetTextColor(dc, RGB(10, 15, 10));
			SetBkMode(dc, TRANSPARENT); // ��������ɫ
			DeleteObject(SelectObject(dc, CreateFont(
				50, 20,    //�߶�50, ��ȡ20��ʾ��ϵͳѡ�����ֵ  
				0, 0,    //�ı���б����������б��Ϊ0  
				FW_HEAVY,    //����  
				0, 0, 0,        //��б�壬���»��ߣ����л���  
				DEFAULT_CHARSET,    //�ַ���  
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,        //һϵ�е�Ĭ��ֵ  
				DEFAULT_PITCH | FF_DONTCARE,
				L"���� Light"    //��������  
			)));
		}
		break;

	case WM_TIMER:
		{
			if (wParam == 1)
			{
				DATE date;
				GetLocalTime(&time);
				SystemTimeToVariantTime(&time, &date);
				DATE termStartDate = ToDate(2021, 3, 1);
				INT termWeek = (date - termStartDate + 7) / 7;

				wchar_t dateStr[64];
				GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &time, L"dddd", dateStr, _countof(dateStr), NULL);

				wsprintf(str, L"%hu/%hu/%hu\n��%d�� %s\n%02d:%02d:%02d",
					time.wYear, time.wMonth, time.wDay, termWeek, dateStr, time.wHour, time.wMinute, time.wSecond);

				FillRect(dc, &wndSize, bgBrush); // clear window
				DrawText(dc, str, lstrlen(str), &wndSize, DT_CENTER | DT_WORDBREAK | DT_MODIFYSTRING);
			}
		}
		break;

	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0); // �����ƶ���ק�����ƶ�ָ��
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}