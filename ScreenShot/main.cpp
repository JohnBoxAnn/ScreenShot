// FOR UNICODE
//
// <INTRODUCTION: A light Program with C++ & Windows 10 API to get & save screen shot in Windows 10.
// NOTICE: THIS PROGRAM IS ONLY FOR WINDOWS 10, CAUSE IT USES UNDOCUMENTED FUNCTIONS OF WINDOWS 10!>
//
// Copyright(C) 2018 @JohnBoxAnn
//
// This program is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>.
// 
// The project github site is <https://github.com/JohnBoxAnn/ScreenShot>.
#pragma once
#include "header.h"
#include "resource.h"

// For future release
#pragma region CMD

#pragma endregion

// ��Ļ��ͼ
#pragma region Screen Capture
// ��Ļ��ͼ����
HBITMAP ScreenCapture(LPCTSTR FilePath, WORD BitCount, LPCRECT lpRect) {
	HANDLE hDIB;
	HANDLE hFile;
	DWORD dwBmpSize;
	DWORD dwSizeofDIB;
	DWORD dwBytesWritten;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;
	CHAR *lpbitmap;
	HDC hdcScreen = GetDC(NULL);	// ȫ��ĻDC
	HDC hdcMemDC = CreateCompatibleDC(hdcScreen);	// ���������ڴ�DC

	// ��ȡ��ȡ�ĳ��ȼ����
	INT ixStart;
	INT iyStart;
	INT iX;
	INT iY;

	// �ж��Ƿ��ȫ��
	if (lpRect == NULL)
	{
		ixStart = iyStart = 0;
		iX = GetSystemMetrics(SM_CXSCREEN);	// ��Ļ��
		iY = GetSystemMetrics(SM_CYSCREEN);	// ��Ļ��
	}
	else
	{
		ixStart = lpRect->left;
		iyStart = lpRect->top;
		iX = lpRect->right - lpRect->left;
		iY = lpRect->bottom - lpRect->top;
	}

	// �ж��ļ�·���Ƿ�Ϊ��
	if (FilePath == NULL)
	{
		MessageBox(NULL, TEXT("File path can not be Empty"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// �жϼ����ڴ�DC�Ƿ�ɹ�����
	if (!hdcMemDC)
	{
		MessageBox(NULL, TEXT("CreateCompatibleDC has Failed"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// ͨ������DC ����һ������λͼ
	hbmScreen = CreateCompatibleBitmap(hdcScreen, iX, iY);

	if (!hbmScreen)
	{
		MessageBox(NULL, TEXT("CreateCompatibleBitmap has Failed"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// ��λͼ�鴫�͵����Ǽ��ݵ��ڴ�DC��
	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(
		hdcMemDC,				// Ŀ��DC
		0, 0,					// Ŀ��DC�� x,y ����
		iX, iY,					// Ŀ�� DC �Ŀ��
		hdcScreen,				// ��ԴDC
		ixStart, iyStart,       // ��ԴDC�� x,y ����
		SRCCOPY))				// ճ����ʽ
	{
		MessageBox(NULL, TEXT("BitBlt has Failed"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// ��ȡλͼ��Ϣ������� bmpScreen ��
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = BitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// �� 32-bit Windows ϵͳ��, GlobalAlloc �� LocalAlloc ���� HeapAlloc ��װ����
	// handle ָ�����Ĭ�ϵĶ�. ���Կ����� HeapAlloc Ҫ��
	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char *)GlobalLock(hDIB);

	// ��ȡ����λͼ��λ���ҿ��������һ�� lpbitmap ��.
	GetDIBits(
		hdcScreen,	// �豸�������
		hbmScreen,  // λͼ���
		0,			// ָ�������ĵ�һ��ɨ����
		(UINT)bmpScreen.bmHeight,	// ָ��������ɨ������
		lpbitmap,	// ָ����������λͼ���ݵĻ�������ָ��
		(BITMAPINFO *)&bi,	// �ýṹ�屣��λͼ�����ݸ�ʽ
		DIB_RGB_COLORS	// ��ɫ���ɺ졢�̡�����RGB������ֱ��ֵ����
	);

	// ����һ���ļ��������ļ���ͼ
	hFile = CreateFile(
		FilePath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// ��ͼƬͷ(headers)�Ĵ�С, ����λͼ�Ĵ�С����������ļ��Ĵ�С
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// ���� Offset ƫ����λͼ��λ(bitmap bits)ʵ�ʿ�ʼ�ĵط�
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	// �ļ���С
	bmfHeader.bfSize = dwSizeofDIB;

	// λͼ�� bfType �������ַ��� "BM"
	bmfHeader.bfType = 0x4D42;	// BM

	dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	// �������ڴ沢�ͷ�
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	// �ر��ļ����
	CloseHandle(hFile);

	// �ͷ�DC
	DeleteDC(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);

	return hbmScreen;
}
#pragma endregion

// ʱ���ַ����ļ�������
#pragma region Time String File Path
// ʱ���ַ����ļ������ɺ���
LPCTSTR TimeStringFilePath(tstring SavePath) {
	time_t t = time(NULL);
	tm rtm;
	localtime_s(&rtm, &t);
	char time_c[20];
	strftime(time_c, 20, "%Y-%m-%d_%H-%M-%S", &rtm);

	tostringstream oss;
	oss << SavePath.c_str() << TEXT('\\') << time_c << TEXT(".bmp") << TEXT('\0');

	LPCTSTR filepath(oss.str().c_str());
	oss.str(TEXT(""));
	oss.clear();

	return filepath;
}
#pragma endregion

// ���ô��ڷ��
#pragma region Composition
// ���ô��ڷ����
void SetWindowBlur(HWND hWnd)
{
	if (SetWindowCompositionAttribute)
	{
		ACCENTPOLICY policy;

		policy = { opt.window_appearance, 2, opt.color, 0 };

		WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) }; // WCA_ACCENT_POLICY=19
		SetWindowCompositionAttribute(hWnd, &data);
	}
	else
		MessageBox(NULL, TEXT("This Program is for Windows 10"), TEXT("ERROR!"), MB_OK);
}
#pragma endregion 

// ���ѡȡ��ͼ����
#pragma region Mouse Select Rect
//opt.window_appearance = ACCENT_ENABLE_BLURBEHIND;				ë����
//opt.color = 0x32282828;
//opt.window_appearance = ACCENT_ENABLE_TRANSPARENTGRADIENT;	͸��
//opt.color = 0x00000000;

// ģ���������ڹ��̺���
LRESULT CALLBACK BlurBackGroundWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static RECT rectValidate;
	static POINTS points_down = { 0,0 };
	static POINTS points_up = { 0,0 };
	static POINTS points = { 0,0 };
	static HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
	switch (uMsg)	// ���ݲ�ͬ����Ϣ���ͽ��в�ͬ�Ĵ���
	{
	case WM_CREATE:	// ���Ǵ���������Ϣ
		rectValidate.top = ((LPCREATESTRUCT)lParam)->y;
		rectValidate.bottom = rectValidate.top + ((LPCREATESTRUCT)lParam)->cy;
		rectValidate.left = ((LPCREATESTRUCT)lParam)->x;
		rectValidate.right = rectValidate.left + ((LPCREATESTRUCT)lParam)->cx;
		break;

	case WM_PAINT:	// ���ǿͻ����ػ���Ϣ
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);	// ���ô��ڻ��ƺ���
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// ���¿ͻ�������ʾ��ʹ��Ч�������Ч
		break;

	case WM_PRINT:	// ���ǿͻ�����ӡ��Ϣ
		ps.rcPaint = rectShow;
		hdc = BeginPaint(hwnd, &ps);	// ���ô��ڻ��ƺ���
		FrameRect(hdc, &ps.rcPaint, hbr);
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// ���¿ͻ�������ʾ��ʹ��Ч�������Ч
		break;

	case WM_KEYDOWN:	// ���Ǽ��̰�����Ϣ
		if (wParam == VK_ESCAPE)	// ����ESC�� 
			DestroyWindow(hwnd);	// �ݻٴ��ڲ�����һ��WM_DESTROY��Ϣ
		break;

	case WM_MOUSEMOVE:	// ��������ڴ������ƶ�
		if (wParam == MK_LBUTTON) {	// ���ǰ�ס�������϶�
			points = MAKEPOINTS(lParam);
			SendMessage(hwnd, WM_PRINT, NULL, NULL);
		}
		break;

	case WM_LBUTTONDOWN:	// ���ǰ���������
		points_down = MAKEPOINTS(lParam);
		points = points_down;
		SendMessage(hwnd, WM_PRINT, NULL, NULL);
		break;
		
	case WM_LBUTTONUP:	// �����ͷ�������
		points_up = MAKEPOINTS(lParam);
		rectShow.top = points_down.y < points_up.y ? points_down.y : points_up.y;
		rectShow.bottom = points_down.y > points_up.y ? points_down.y : points_up.y;
		rectShow.left = points_down.x < points_up.x ? points_down.x : points_up.x;
		rectShow.right = points_down.x > points_up.x ? points_down.x : points_up.x;
		DestroyWindow(hwnd);	// �ݻٴ��ڲ�����һ��WM_DESTROY��Ϣ
		break;

	case WM_DESTROY:	// ���Ǵ��ڴݻ���Ϣ
		DeleteObject(hbr);
		PostQuitMessage(0);	// ��ϵͳ�����и��߳�����ֹ����������ӦWM_DESTROY��Ϣ
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);	// ����Ĭ�ϴ��ڹ���ΪӦ�ó���û�д���Ĵ�����Ϣ�ṩĬ�ϵĴ���  
	}
	return 0;
}

// ����ģ����������
HWND CreateBlurBackgroundWindow(HINSTANCE &hInstance) {
	WNDCLASSEX wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.lpfnWndProc = BlurBackGroundWindowProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = NULL;
	wndclassex.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndclassex.hbrBackground = NULL;
	wndclassex.lpszMenuName = NULL;
	wndclassex.lpszClassName = szWindowName;
	if (!RegisterClassEx(&wndclassex))
		MessageBox(NULL, TEXT("This Program is for Windows 10"), TEXT("ERROR!"), MB_OK);
	HWND hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_NOPARENTNOTIFY | WS_EX_NOACTIVATE,
		szWindowName,
		NULL,
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		hInstance,
		NULL);
	if (hwnd == NULL)
		return hwnd;
	opt.window_appearance = ACCENT_ENABLE_TRANSPARENTGRADIENT;
	opt.color = 0x00000000;
	SetWindowBlur(hwnd);
	ShowWindow(hwnd, SW_SHOW);
	
	return hwnd;
}

// ���ѡȡ��ͼ�����������ã�
void MouseSelectRect(RECT &rect) {
	POINT point_down;
	POINT point_up;
	bool wait = true;
	while (wait) {	// �������һ���ͷ����������¼�
		if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
			GetCursorPos(&point_up);
			wait = false;
		}
		Sleep(100);	// ���͵ȴ�ʱ��CPUռ��
	}
	bool click = true;
	while (click) {	// ��ȡ���ѡȡ�ľ��ε�����
		if (GetAsyncKeyState(VK_LBUTTON) >> sizeof(char)) {
			bool move = true;
			GetCursorPos(&point_down);
			while (move) {
				if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
					GetCursorPos(&point_up);
					move = false;
					click = false;
				}
				Sleep(100);	// ���͵ȴ�ʱ��CPUռ��
			}
		}
		Sleep(100);	// ���͵ȴ�ʱ��CPUռ��
	}

	rect.top = point_down.y < point_up.y ? point_down.y : point_up.y;
	rect.bottom = point_down.y < point_up.y ? point_up.y : point_down.y;
	rect.left = point_down.x < point_up.x ? point_down.x : point_up.x;
	rect.right = point_down.x < point_up.x ? point_up.x : point_down.x;
}
#pragma endregion

// INI���
#pragma region INIRelation
// ��ȡINI�ļ���ʼ��INI�ṹ
void initINI() {
	if (!ini_parser.ReadINI(INIPath)) {	// �ļ����ڣ���ȡ
		// �ļ������ڣ��������ļ�������Ĭ��ֵ
		ini_parser.SetValue(TEXT("Common"), TEXT("ScreenCaptureSavePath"), DEFULTSavePath);
		ini_parser.SetValue(TEXT("Common"), TEXT("CaptureWindowName"), toString(false));
		ini_parser.SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(true));
		ini_parser.SetValue(TEXT("Rect"), TEXT("top"), toString(0));
		ini_parser.SetValue(TEXT("Rect"), TEXT("bottom"), toString(GetSystemMetrics(SM_CYSCREEN)));
		ini_parser.SetValue(TEXT("Rect"), TEXT("left"), toString(0));
		ini_parser.SetValue(TEXT("Rect"), TEXT("right"), toString(GetSystemMetrics(SM_CXSCREEN)));
		WriteINI();
	}
}

// ��ȡINI�ṹ��Ϣ���������
void ReadINI(tstring &SavePath, RECT &rect, const TCHAR* CaptureWindowName) {
	SavePath = ini_parser.GetValue(TEXT("Common"), TEXT("ScreenCaptureSavePath"));
	CaptureWindowName = ini_parser.GetValue(TEXT("Common"), TEXT("CaptureWindowName")).c_str();
	rect.top = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("top")));
	rect.bottom = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("bottom")));
	rect.left = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("left")));
	rect.right = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("right")));
}

// ����ǰINI�ṹ��Ϣд��INI�ļ�
inline void WriteINI() {
	ini_parser.WriteINI(INIPath);
}

// �ж��Ƿ������ѡȡ��ͼ����
bool isMouseSelectRect() {
	if (fromString<bool>(ini_parser.GetValue(TEXT("Common"), TEXT("MouseSelectRect")))) {
		if (IDNO == MessageBox(NULL, TEXT("�´��Ƿ�����ѡȡ��ͼ����"), TEXT("��ʾ"), MB_YESNO))
			ini_parser.SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(false));
		else
			ini_parser.SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(true));
		return true;
	}
	return false;
}
#pragma endregion

// �������
#pragma region wWinMain
// ������ں���
int CALLBACK tWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPTSTR pCmdLine, int nCmdShow) {
	HWND hwndCap = NULL;						// ��ͼָ�����ھ��
	tstring FileSavePath;						// ��ͼλͼ�ļ��洢·��
	HBITMAP ghBitmap = NULL;					// λͼ���
	TCHAR szCaptureWindowName[100] = { NULL };	// ��ͼָ����������
	SetProcessDPIAware();						// ��DPIʱ��������
	initINI();
	ReadINI(FileSavePath, rectShow, szCaptureWindowName);
	
	// �ж��Ƿ�����ѡȡ��ͼ��������
	if (isMouseSelectRect()) {
		MessageBox(NULL, TEXT("��ѡȡ��ͼ����"), TEXT("��ʾ"), MB_OK);
		HWND hWnd = CreateBlurBackgroundWindow(hInstance);
		if (hWnd == NULL)
			return 0;
		MSG msg = {};
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		ini_parser.SetValue(TEXT("Rect"), TEXT("top"), toString(rectShow.top));
		ini_parser.SetValue(TEXT("Rect"), TEXT("bottom"), toString(rectShow.bottom));
		ini_parser.SetValue(TEXT("Rect"), TEXT("left"), toString(rectShow.left));
		ini_parser.SetValue(TEXT("Rect"), TEXT("right"), toString(rectShow.right));
		WriteINI();
	}

	// ��ͼλͼ�ļ��洢·����Ŀ¼�Ƿ���ڣ��������򴴽���Ŀ¼
	if (taccess(FileSavePath.c_str(), F_OK))
		if (tmkdir(FileSavePath.c_str())) {
			MessageBox(NULL, TEXT("Can not Found direction"), TEXT("ERROR!"), MB_OK);
			return 0;
		}

	// �����ָ��������ı�rectShow������ʹ�ô�INI��ȡ��ֵ��ͼ
	if (*szCaptureWindowName != NULL) {
		hwndCap = FindWindow(NULL, szCaptureWindowName);
		if (!GetWindowRect(hwndCap, &rectShow)) {
			MessageBox(NULL, TEXT("Can not Find the window to capture"), TEXT("ERROR!"), MB_OK);
			return 0;
		}
	}

	// ��ȫ��Ҫ��&rectShow����FULLSCREENCAPTURE�����ֶ�����INI�ļ�
	ghBitmap = ScreenCapture(TimeStringFilePath(FileSavePath), ColorBit, &rectShow);

	return 0;
}
#pragma endregion