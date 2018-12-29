// FOR UNICODE
//
// <INTRODUCTION: A light Program with C++ & Windows 10 API to get & save screen shot in Windows 10.
// NOTICE: THIS PROGRAM IS ONLY FOR WINDOWS 10, CAUSE IT USES UNDOCUMENTED FUNCTIONS OF WINDOWS 10!>
//
// Copyright(C) 2018 @Boxann John
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>.
// 
// The project github site is <https://github.com/JohnBoxAnn/ScreenShot>.
// If there are any problems, please note me there.

#pragma once
#include "header.h"
#include "resource.h"

bool bLoadDriver = false;	// ����װ�ر�־
extern int R_days;

// ������֧��
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

	// �ж��ļ�·���Ƿ�Ϊ��
	if (FilePath == NULL)
	{
		MessageBox(NULL, TEXT("File path can not be Empty"), TEXT("ERROR!"), MB_OK);
		return hbmScreen;
	}

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

	// �жϼ����ڴ�DC�Ƿ�ɹ�����
	if (!hdcMemDC)
	{
		MessageBox(NULL, TEXT("CreateCompatibleDC has Failed"), TEXT("ERROR!"), MB_OK);
		//DeleteObject(hbmScreen);
		//DeleteDC(hdcMemDC);
		DeleteDC(hdcScreen);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// ͨ������DC ����һ������λͼ
	hbmScreen = CreateCompatibleBitmap(hdcScreen, iX, iY);

	if (!hbmScreen)
	{
		MessageBox(NULL, TEXT("CreateCompatibleBitmap has Failed"), TEXT("ERROR!"), MB_OK);
		//DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		DeleteDC(hdcScreen);
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
		DeleteDC(hdcScreen);
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
	if (!hDIB)
	{
		MessageBox(NULL, TEXT("GlobalAlloc has Failed"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		DeleteDC(hdcScreen);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}
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
	DeleteDC(hdcScreen);
	ReleaseDC(NULL, hdcScreen);

	return hbmScreen;
}
#pragma endregion

// �ļ�����·������
#pragma region File Name & File Path
// ʱ���ַ����ļ������ɺ���
LPCTSTR TimeStringFilePath(tstring &SavePath, tstring &ext) {
	time_t t = time(NULL);
	tm rtm;
	localtime_s(&rtm, &t);
	char time_c[20];
	strftime(time_c, 20, "%Y-%m-%d_%H-%M-%S", &rtm);

	oss << SavePath << TEXT('\\') << time_c << TEXT(".") << ext << TEXT('\0');

	LPCTSTR filepath(oss.str().c_str());
	oss.str(TEXT(""));
	oss.flush();

	return filepath;
}

// stock������ʱ���ļ������ɺ���
LPCTSTR CodeStringFilePath(tstring &SavePath, tstring &code, DATANode &data_node, tstring &ext) {
	oss << SavePath << TEXT('\\') << TEXT("[") << code << TEXT("]") << data_node.end.year
		<< data_node.end.month << data_node.end.day << TEXT("_") << toString(R_days)
		<< TEXT(".") << ext << TEXT('\0');

	LPCTSTR filepath(oss.str().c_str());
	oss.str(TEXT(""));
	oss.flush();

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

	case WM_PAINT:	// ����ϵͳ�ػ���Ϣ
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);	// ���ô��ڻ��ƺ���
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// ���¿ͻ�������ʾ��ʹ��Ч�������Ч
		DeleteObject(hdc);
		break;
	}
	case WM_PRINT:	// ���ǿͻ���������Ϣ
	{
		HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
		PAINTSTRUCT ps;
		ps.rcPaint = rectShow;
		HDC hdc = BeginPaint(hwnd, &ps);	// ���ô��ڻ��ƺ���
		FrameRect(hdc, &ps.rcPaint, hbr);
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// ���¿ͻ�������ʾ��ʹ��Ч�������Ч
		DeleteObject(hdc);
		break;
	}
	case WM_KEYDOWN:	// ���Ǽ��̰�����Ϣ
		if (wParam == VK_ESCAPE)	// ����ESC�� 
			DestroyWindow(hwnd);	// �ݻٴ��ڲ�����һ��WM_DESTROY��Ϣ
		break;

	case WM_MOUSEMOVE:	// ��������ڴ������ƶ�
		if (wParam == MK_LBUTTON) {	// ���ǰ�ס�������϶�
			points = MAKEPOINTS(lParam);
			rectShow.top = points_down.y < points.y ? points_down.y : points.y;
			rectShow.bottom = points_down.y > points.y ? points_down.y : points.y;
			rectShow.left = points_down.x < points.x ? points_down.x : points.x;
			rectShow.right = points_down.x > points.x ? points_down.x : points.x;
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
		bRSRrun = false;
		PostQuitMessage(0);	// ��ϵͳ�����и��߳�����ֹ����������ӦWM_DESTROY��Ϣ
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);	// ����Ĭ�ϴ��ڹ���ΪӦ�ó���û�д���Ĵ�����Ϣ�ṩĬ�ϵĴ���
	}
	return 0;
}

// ����ģ����������
HWND CreateBlurBackgroundWindow(HINSTANCE &hInstance) {
	HWND hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_NOACTIVATE,
		L"BackGroundWindow",
		L"Background",
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
	opt.color = 0x23000000;
	SetWindowBlur(hwnd);
	ShowWindowAsync(hwnd, SW_SHOW);

	return hwnd;
}

// ���ѡȡ��ͼ�����������ã�
//void MouseSelectRect(RECT &rect) {
//	POINT point_down;
//	POINT point_up;
//	bool wait = true;
//	while (wait) {	// �������һ���ͷ����������¼�
//		if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
//			GetCursorPos(&point_up);
//			wait = false;
//		}
//		Sleep(100);	// ���͵ȴ�ʱ��CPUռ��
//	}
//	bool click = true;
//	while (click) {	// ��ȡ���ѡȡ�ľ��ε�����
//		if (GetAsyncKeyState(VK_LBUTTON) >> sizeof(char)) {
//			bool move = true;
//			GetCursorPos(&point_down);
//			while (move) {
//				if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
//					GetCursorPos(&point_up);
//					move = false;
//					click = false;
//				}
//				Sleep(100);	// ���͵ȴ�ʱ��CPUռ��
//			}
//		}
//		Sleep(100);	// ���͵ȴ�ʱ��CPUռ��
//	}
//
//	rect.top = point_down.y < point_up.y ? point_down.y : point_up.y;
//	rect.bottom = point_down.y < point_up.y ? point_up.y : point_down.y;
//	rect.left = point_down.x < point_up.x ? point_down.x : point_up.x;
//	rect.right = point_down.x < point_up.x ? point_up.x : point_down.x;
//}
#pragma endregion

// Tray�������ڣ�
#pragma region Tray
// �����ڹ��̺���
LRESULT CALLBACK MAINPROC(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	switch (message)
	{
	case WM_CREATE:
		InitHotKey(hWnd);
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		break;

	case WM_CLOSE:
		if (MessageBox(NULL, L"���Ҫ�˳���", L"��ʾ", MB_YESNO) == IDYES)
			DestroyWindow(hWnd);	// �ݻٴ��ڲ�����һ��WM_DESTROY��Ϣ
		break;

	case WM_HOTKEY:
	{
		WORD high = HIWORD(lParam);	// �������
		WORD low = LOWORD(lParam);	// ��λ��ָ��Ҫ���λ��ָ���ļ����ʹ��������WM_HOTKEY��Ϣ�ļ�
		
		switch (low)
		{
		case MOD_CONTROL:
			switch (high)
			{
			case 'R':	// ����ѡȡ��ͼ����(Ctrl+R)
				SetForegroundWindow(hWnd);
				Rerect(hInstance, hWnd);
				break;
			case 'P':	// ���ô洢·��(Ctrl+P)
				SetSavePath(hWnd);
				break;
			case 'D':	// ����data·��(Ctrl+D)
				SetDataPath(hWnd);
				break;
			case 'X':	// �˳�(Ctrl+X)
				SetForegroundWindow(hWnd);
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
			}
			break;
		case MOD_CONTROL | MOD_ALT:
			switch (high) {
			case 'F':	// ��ȫ��(Ctrl+Alt+F)
				if (!FullSC())
					return 0;
				break;
			case 'R':	// �ؾ�������(Ctrl+Alt+R)
				if (!RectSC())
					return 0;
				break;
			case 'S':	// ��ʼ�ɼ�(Ctrl+Alt+S)
				if (!StartSC())
					return 0;
				break;
			}
			break;
		}
	}
	break;

	case WM_NOTIFY_MAIN:
		if (lParam == WM_LBUTTONUP || lParam == WM_RBUTTONUP)
		{
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
			UINT tray = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_VERNEGANIMATION, pt.x, pt.y, 0, hWnd, NULL);
			switch (tray)
			{
			case IDM_RERECT:	// ����ѡȡ��ͼ����(Ctrl+R)
				Rerect(hInstance, hWnd);
				break;

			case IDM_SETSAVEPATH:	// ���ô洢·��(Ctrl+P)
				SetSavePath(hWnd);
				break;
			
			case IDM_SETDATAPATH:	// ����data·��(Ctrl+D)
				SetDataPath(hWnd);
				break;
			
			case IDM_SETRDAYS:	// ����ʱ����
				SetRDays(hInstance, hWnd);
				break;

			case IDM_FULL:	// ��ȫ��(Ctrl+Alt+F)
				if (!FullSC())
					return 0;
				break;
			
			case IDM_RECT:	// �ؾ�������(Ctrl+Alt+R)
				if (!RectSC())
					return 0;
				break;
			
			case IDM_START:	// ��ʼ�ɼ�(Ctrl+Alt+S)
				if (!StartSC())
					return 0;
				break;
			
			case IDM_EXIT:	// �˳�(Ctrl+X)
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
			}
		}
		break;

	case WM_DESTROY:
		UninitHotKey(hWnd);
		CloseINI();
		CloseDATA();
		if (ghBitmap)
			DeleteObject(ghBitmap);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ��ʼ�ɼ�
int StartSC() {
	HANDLE h = CreateThread(NULL, 0, SSCThread, NULL, 0, NULL);
	if (h == INVALID_HANDLE_VALUE || h == NULL)
		return 0;
	WaitForSingleObject(h, INFINITE);
	CloseHandle(h);
	return 1;
}

// �ؾ�������
int RectSC() {
	// ��ͼλͼ�ļ��洢·����Ŀ¼�Ƿ���ڣ��������򴴽���Ŀ¼
	if (taccess(FileSavePath.c_str(), F_OK))
		if (tmkdir(FileSavePath.c_str())) {
			MessageBox(NULL, TEXT("Can not Found & Create folder"), TEXT("ERROR!"), MB_OK);
			return 0;
		}

	// ��ȫ��Ҫ��&rectShow����FULLSCREENCAPTURE�����ֶ�����INI�ļ�
	ghBitmap = ScreenCapture(TimeStringFilePath(FileSavePath, ext), ColorBit, &rectShow);

	return 1;
}

// ��ȫ��
int FullSC() {
	// ��ͼλͼ�ļ��洢·����Ŀ¼�Ƿ���ڣ��������򴴽���Ŀ¼
	if (taccess(FileSavePath.c_str(), F_OK))
		if (tmkdir(FileSavePath.c_str())) {
			MessageBox(NULL, TEXT("Can not Found & Create folder"), TEXT("ERROR!"), MB_OK);
			return 0;
		}

	// ��ȫ��Ҫ��&rectShow����FULLSCREENCAPTURE�����ֶ�����INI�ļ�
	ghBitmap = ScreenCapture(TimeStringFilePath(FileSavePath, ext), ColorBit, FULLSCREENCAPTURE);

	return 1;
}

// ����ʱ����
inline void SetRDays(HINSTANCE hInstance, HWND hWnd) {
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_INPUTRDAYS), hWnd, (DLGPROC)RDaysDlgPro);
}

// ����data·��
void SetDataPath(HWND hWnd) {
	TCHAR szBuffer[MAX_PATH] = { 0 };
	OPENFILENAME file = { 0 };
	file.hwndOwner = hWnd;
	file.lStructSize = sizeof(file);
	file.lpstrFilter = L"�ı��ļ�(*.txt)\0*.txt\0�����ļ�(*.*)\0*.*\0";	// Ҫѡ����ļ���׺ 
	file.lpstrInitialDir = L".\\";	// Ĭ�ϵ��ļ�·�� 
	file.lpstrFile = szBuffer;	// ����ļ�·���Ļ����� 
	file.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	file.nFilterIndex = 0;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;	// ��־����Ƕ�ѡҪ����OFN_ALLOWMULTISELECT
	if (GetOpenFileName(&file)) {
		DataPath = szBuffer;
		ini_parser->SetValue(TEXT("Common"), TEXT("DataPath"), DataPath);
		WriteINI();
	}
}

// ���ô洢·��
void SetSavePath(HWND hWnd) {
	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = hWnd;	// ӵ���ߴ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
	bi.pszDisplayName = szBuffer;	// �����ļ��еĻ�����  
	bi.lpszTitle = TEXT("ѡ��һ���ļ���");	// ����  
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (SHGetPathFromIDList(idl, szBuffer)) {
		FileSavePath = szBuffer;
		ini_parser->SetValue(TEXT("Common"), TEXT("ScreenCaptureSavePath"), FileSavePath);
		WriteINI();
	}
}

// ����ѡȡ��ͼ����
int Rerect(HINSTANCE hInstance, HWND hWnd) {
	MessageBox(NULL, TEXT("��ѡȡ��ͼ����"), TEXT("��ʾ"), MB_OK);
	HANDLE h = CreateThread(NULL, 0, RSRThread, &hInstance, 0, NULL);
	if (h == INVALID_HANDLE_VALUE || h == NULL)
		return 0;
	WaitForSingleObject(h, INFINITE);
	CloseHandle(h);
	ini_parser->SetValue(TEXT("Rect"), TEXT("top"), toString(rectShow.top));
	ini_parser->SetValue(TEXT("Rect"), TEXT("bottom"), toString(rectShow.bottom));
	ini_parser->SetValue(TEXT("Rect"), TEXT("left"), toString(rectShow.left));
	ini_parser->SetValue(TEXT("Rect"), TEXT("right"), toString(rectShow.right));
	WriteINI();
	return 1;
}

// ��ʼ������
void initTray(HWND parent) {
	Tray.cbSize = sizeof(Tray);
	Tray.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(MAINICON));
	Tray.hWnd = parent;
	wcscpy_s(Tray.szTip, L"ͼ��ɼ�����");
	Tray.uCallbackMessage = WM_NOTIFY_MAIN;
	Tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Tray.uID = 101;
	Shell_NotifyIcon(NIM_ADD, &Tray);
	Shell_NotifyIcon(NIM_SETVERSION, &Tray);
}

// ��ʼ��������
int InitWindow(HINSTANCE &hInstance) {
	WNDCLASSEX Wndclassex = { 0 };
	Wndclassex.hInstance = hInstance;
	Wndclassex.lpszClassName = L"ScreenShot";
	Wndclassex.lpfnWndProc = MAINPROC;
	Wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	Wndclassex.cbClsExtra = 0;
	Wndclassex.cbWndExtra = 0;
	Wndclassex.cbSize = sizeof(WNDCLASSEX);
	Wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	Wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	Wndclassex.lpszMenuName = L"POPUP";
	Wndclassex.hbrBackground = (HBRUSH)BLACK_BRUSH;

	WNDCLASSEX wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.lpfnWndProc = BlurBackGroundWindowProc;
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = NULL;
	wndclassex.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndclassex.hbrBackground = (HBRUSH)WHITE_BRUSH;
	wndclassex.lpszMenuName = NULL;
	wndclassex.lpszClassName = L"BackGroundWindow";
	if (!RegisterClassEx(&wndclassex) || !RegisterClassEx(&Wndclassex)) {
		MessageBox(NULL, TEXT("ע�ᴰ����ʧ��"), TEXT("ERROR!"), MB_OK);
		return 0;
	}
	return 1;
}

// ���������ڣ����̣�
HWND CreateMainWindow(HINSTANCE &hInstance) {
	if (!InitWindow(hInstance))
		return 0;
	HMENU popup = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_POPUP_MENU));
	menu = GetSubMenu(popup, 0);
	HWND tray_hwnd = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		L"ScreenShot",
		L"TrayWindow",
		WS_OVERLAPPED,
		0,
		0,
		400,
		400,
		NULL,
		NULL,
		hInstance,
		NULL);
	initTray(tray_hwnd);
	ShowWindow(tray_hwnd, WM_SHOWWINDOW);

	return tray_hwnd;
}
#pragma endregion

// �Ի���
#pragma region Dialog
LRESULT CALLBACK RDaysDlgPro(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	TCHAR str[256];
	switch (message) {
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			GetDlgItemText(hDlg, IDC_TEXTBOX, str, 32);
			R_days = fromString<int>(str);
			ini_parser->SetValue(TEXT("Common"), TEXT("RangeDays"), toString(R_days));
			WriteINI();
			EndDialog(hDlg, 0);
		}
		if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, 1);
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 1);
		break;
	}
	return 0;
}
#pragma endregion

// INI���
#pragma region INIRelation
// ��ȡINI�ļ���ʼ��INI��
void initINI() {
	TCHAR *inipath = new TCHAR[MAX_PATH];
	TCHAR *driver = new TCHAR[10];
	TCHAR *dir = new TCHAR[MAX_PATH];
	TCHAR *filename = new TCHAR[50];
	TCHAR *ext = new TCHAR[10];
	GetModuleFileName(NULL, inipath, MAX_PATH);
	t_splitpath_s(inipath, driver, 10, dir, MAX_PATH, filename, 50, ext, 10);
	t_makepath_s(inipath, MAX_PATH, driver, dir, L"config", L"ini");
	INIPath = inipath;	// �̶�INI�ļ�·��
	delete[] inipath, driver, dir, filename, ext;

	if (ini_parser->Init(INIPath, ios_base::in))	// �ļ����ڣ���ȡ
		ini_parser->Read();
	else
	{	// �ļ������ڣ��������ļ�������Ĭ��ֵ
		ini_parser->SetValue(TEXT("Common"), TEXT("DataPath"), DEFULTDataPath);
		ini_parser->SetValue(TEXT("Common"), TEXT("ScreenCaptureSavePath"), DEFULTSavePath);
		ini_parser->SetValue(TEXT("Common"), TEXT("CaptureWindowName"), toString('\0'));
		ini_parser->SetValue(TEXT("Common"), TEXT("RangeDays"), toString(0));
		ini_parser->SetValue(TEXT("Rect"), TEXT("top"), toString(0));
		ini_parser->SetValue(TEXT("Rect"), TEXT("bottom"), toString(GetSystemMetrics(SM_CYSCREEN)));
		ini_parser->SetValue(TEXT("Rect"), TEXT("left"), toString(0));
		ini_parser->SetValue(TEXT("Rect"), TEXT("right"), toString(GetSystemMetrics(SM_CXSCREEN)));
		WriteINI();
	}
	ini_parser->Close();
}

// ��ȡINI�ṹ��Ϣ���������
void ReadINI(const TCHAR* CaptureWindowName) {
	DataPath = ini_parser->GetValue(TEXT("Common"), TEXT("DataPath"));
	FileSavePath = ini_parser->GetValue(TEXT("Common"), TEXT("ScreenCaptureSavePath"));
	CaptureWindowName = ini_parser->GetValue(TEXT("Common"), TEXT("CaptureWindowName")).c_str();
	R_days = fromString<int>(ini_parser->GetValue(TEXT("Common"), TEXT("RangeDays")));
	rectShow.top = fromString<LONG>(ini_parser->GetValue(TEXT("Rect"), TEXT("top")));
	rectShow.bottom = fromString<LONG>(ini_parser->GetValue(TEXT("Rect"), TEXT("bottom")));
	rectShow.left = fromString<LONG>(ini_parser->GetValue(TEXT("Rect"), TEXT("left")));
	rectShow.right = fromString<LONG>(ini_parser->GetValue(TEXT("Rect"), TEXT("right")));
}

// ����ǰINI�ṹ��Ϣд��INI�ļ�
inline void WriteINI() {
	ini_parser->Init(INIPath, ios_base::out | ios_base::trunc);
	ini_parser->Write();
	ini_parser->Close();
}

// ����INI��
inline void CloseINI() {
	delete ini_parser;
}

// �ж��Ƿ������ѡȡ��ͼ���򣨷�����
//bool isMouseSelectRect() {
//	if (fromString<bool>(ini_parser->GetValue(TEXT("Common"), TEXT("MouseSelectRect")))) {
//		if (IDNO == MessageBox(NULL, TEXT("�´��Ƿ�����ѡȡ��ͼ����"), TEXT("��ʾ"), MB_YESNO))
//			ini_parser->SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(false));
//		else
//			ini_parser->SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(true));
//		return true;
//	}
//	return false;
//}
#pragma endregion

// DATA���
#pragma region DATARelation
// ��ȡdata���ݲ��ɼ�ͼ��
int ReadData2Start() {
	DATANode data_node;
	InitDriver();
	if (!bLoadDriver)
		return 0;
	if (!data_parser->Init(DataPath, ios_base::in)) {
		ExitDriver();
		return 0;
	}
	
	// ��ͼλͼ�ļ��洢·����Ŀ¼�Ƿ���ڣ��������򴴽���Ŀ¼
	if (taccess(FileSavePath.c_str(), F_OK))
		if (tmkdir(FileSavePath.c_str())) {
			MessageBox(NULL, TEXT("Can not Found & Create folder"), TEXT("ERROR!"), MB_OK);
			data_parser->Close();
			ExitDriver();
			return 0;
		}

	HWND hwnd = FindWindow(L"TdxW_MainFrame_Class", NULL);
	//HWND hwnd = FindWindow(L"Notepad", NULL);

	if (hwnd == NULL) {
		data_parser->Close();
		ExitDriver();
		return 0;
	}

	InitSendkbInput(hwnd);
	if (R_days > 0) {
		while (data_parser->Read())
			while (data_parser->GetValue(data_node)) {
				InputString(data_parser->code);
				InputKey(__Enter, KEY_STD, 50);
				Sleep(500);
				InputString(L"49");
				InputKey(__Enter, KEY_STD, 50);
				Sleep(500);
				InputString(data_node.end.year);
				InputKey(__Right, KEY_E0, 50);
				InputString(data_node.end.month);
				InputKey(__Right, KEY_E0, 50);
				InputString(data_node.end.day);
				InputKey(__Enter, KEY_STD, 50);
				Sleep(500);
				InputString(L"42");
				InputKey(__Enter, KEY_STD, 50);
				Sleep(500);
				InputString(data_node.begin.year);
				InputKey(__Right, KEY_E0, 50);
				InputString(data_node.begin.month);
				InputKey(__Right, KEY_E0, 50);
				InputString(data_node.begin.day);
				InputKey(__Enter, KEY_STD, 50);
				Sleep(1000);
				ghBitmap = ScreenCapture(CodeStringFilePath(FileSavePath,
					data_parser->code, data_node, ext), ColorBit, &rectShow);
				InputString(L"42");
				InputKey(__Enter, KEY_STD, 50);
				Sleep(500);
				InputString(L"49");
				InputKey(__Enter, KEY_STD, 50);
				Sleep(1000);
			}
	}
	else {
		while (data_parser->Read())
			while (data_parser->GetValue(data_node)) {
				InputString(data_parser->code);
				InputKey(__Enter, KEY_STD, 50);
				Sleep(500);
				InputString(L"49");
				InputKey(__Enter, KEY_STD, 50);
				Sleep(500);
				InputString(data_node.end.year);
				InputKey(__Right, KEY_E0, 50);
				InputString(data_node.end.month);
				InputKey(__Right, KEY_E0, 50);
				InputString(data_node.end.day);
				InputKey(__Enter, KEY_STD, 50);
				Sleep(1000);
				ghBitmap = ScreenCapture(CodeStringFilePath(FileSavePath,
					data_parser->code, data_node, ext), ColorBit, &rectShow);
				InputString(L"49");
				InputKey(__Enter, KEY_STD, 50);
				Sleep(1000);
			}
	}
	data_parser->Close();
	ExitDriver();
	return 1;
}

// ����DATA��
inline void CloseDATA() {
	delete data_parser;
}
#pragma endregion

// �����������
#pragma region Simulate Input Control
void InitSendkbInput(HWND hwnd) {
	SetForegroundWindow(hwnd); // �������ᵽǰ̨
	SetFocus(hwnd); // �����
	Sleep(5000);
}

//void SimSendkbInput(WORD nScanCode, int nSleepTime, bool isExtended)
//{
//	if (isExtended) {
//		INPUT stInput;
//		stInput.type = INPUT_KEYBOARD;	// ����Ϊ����
//		stInput.ki.wVk = 0;
//		stInput.ki.wScan = nScanCode;
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;	// ֻʹ��ScanCode����
//		stInput.ki.time = GetTickCount();
//		stInput.ki.dwExtraInfo = GetMessageExtraInfo();
//
//		int nCnt = SendInput(1, &stInput, sizeof(INPUT));	// ģ�ⰴ��
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;
//		stInput.ki.time = GetTickCount();
//		Sleep(nSleepTime);
//		nCnt = SendInput(1, &stInput, sizeof(INPUT));	// ģ���ɿ�
//	}
//	else {
//		INPUT stInput;
//		stInput.type = INPUT_KEYBOARD;	// ����Ϊ����
//		stInput.ki.wVk = 0;
//		stInput.ki.wScan = nScanCode;
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE;	// ֻʹ��ScanCode����
//		stInput.ki.time = GetTickCount();
//		stInput.ki.dwExtraInfo = GetMessageExtraInfo();
//
//		int nCnt = SendInput(1, &stInput, sizeof(INPUT));	// ģ�ⰴ��
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
//		stInput.ki.time = GetTickCount();
//		Sleep(nSleepTime);
//		nCnt = SendInput(1, &stInput, sizeof(INPUT));	// ģ���ɿ�
//	}
//}
//
//void SimSendStringInput(tstring &str) {
//	for (int i = 0; i < str.length(); i++)
//		switch (str.at(i))
//		{
//		case L'0':
//			SimSendkbInput(__Pad0, 0, false);
//			break;
//		case L'1':
//			SimSendkbInput(__Pad1, 0, false);
//			break;
//		case L'2':
//			SimSendkbInput(__Pad2, 0, false);
//			break;
//		case L'3':
//			SimSendkbInput(__Pad3, 0, false);
//			break;
//		case L'4':
//			SimSendkbInput(__Pad4, 0, false);
//			break;
//		case L'5':
//			SimSendkbInput(__Pad5, 0, false);
//			break;
//		case L'6':
//			SimSendkbInput(__Pad6, 0, false);
//			break;
//		case L'7':
//			SimSendkbInput(__Pad7, 0, false);
//			break;
//		case L'8':
//			SimSendkbInput(__Pad8, 0, false);
//			break;
//		case L'9':
//			SimSendkbInput(__Pad9, 0, false);
//			break;
//		default:
//			MessageBox(NULL, L"������Ч�ַ�", L"ERROR::SimSendStringInput(tstring&)", MB_OK);
//			break;
//		}
//}
#pragma endregion

// �߳�
#pragma region Thread
// ѡ���ͼ���򴰿��̺߳���
unsigned long WINAPI RSRThread(void* param) {
	MSG msg = { 0 };
	HWND hwnd = CreateBlurBackgroundWindow(*(HINSTANCE*)param);
	
	if (hwnd == NULL) {
		MessageBox(NULL, L"����δ����", L"ERROR!", MB_OK);
		return 0;
	}
	SetForegroundWindow(hwnd);

	while (bRSRrun) {
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	bRSRrun = true;
	return 0;
}

unsigned long WINAPI SSCThread(void* param) {
	if (!ReadData2Start())
		MessageBox(NULL, L"ReadData2Start()��������",L"ERROR!", MB_OK);
	return 0;
}
#pragma endregion

// �ȼ�
#pragma region HotKey
void InitHotKey(HWND hwnd) {
	RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_NOREPEAT, 'R');	// ����ѡȡ��ͼ����(Ctrl+R)
	RegisterHotKey(hwnd, 2, MOD_CONTROL | MOD_NOREPEAT, 'P');	// ���ô洢·��(Ctrl+P)
	RegisterHotKey(hwnd, 3, MOD_CONTROL | MOD_NOREPEAT, 'D');	// ����data·��(Ctrl+D)
	RegisterHotKey(hwnd, 4, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'F');	// ��ȫ��(Ctrl+Alt+F)
	RegisterHotKey(hwnd, 5, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'R');	// �ؾ�������(Ctrl+Alt+R)
	RegisterHotKey(hwnd, 6, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'S');	// ��ʼ�ɼ�(Ctrl+Alt+S)
	RegisterHotKey(hwnd, 7, MOD_CONTROL | MOD_NOREPEAT, 'X');	// �˳�(Ctrl+X)
}

void UninitHotKey(HWND hwnd) {
	UnregisterHotKey(hwnd, 1);
	UnregisterHotKey(hwnd, 2);
	UnregisterHotKey(hwnd, 3);
	UnregisterHotKey(hwnd, 4);
	UnregisterHotKey(hwnd, 5);
	UnregisterHotKey(hwnd, 6);
	UnregisterHotKey(hwnd, 7);
}
#pragma endregion

// ����
#pragma region Driver
// ��ʼ������
void InitDriver() {
	//��������
	bLoadDriver = ManageDriver(KEYMOUSE_DRIVER_NAME, DRIVER_FUNC_INSTALL);
	if (!bLoadDriver)
		MessageBox(NULL, L"��������ʧ��!", L"ERROR!", MB_OK);
}

// ж������
void ExitDriver() {
	if (bLoadDriver)
		ManageDriver(KEYMOUSE_DRIVER_NAME, DRIVER_FUNC_REMOVE);
}
#pragma endregion

// �������
#pragma region WinMain
// ������ں���
int CALLBACK tWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	SetProcessDPIAware();						// ��DPIʱ��������
	initINI();
	ReadINI(szCaptureWindowName);

	HWND hwnd = CreateMainWindow(hInstance);
	
	if (hwnd == NULL)
		return 0;

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
#pragma endregion