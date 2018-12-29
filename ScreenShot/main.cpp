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

bool bLoadDriver = false;	// 驱动装载标志
extern int R_days;

// 命令行支持
#pragma region CMD

#pragma endregion

// 屏幕截图
#pragma region Screen Capture
// 屏幕截图函数
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

	// 判断文件路径是否为空
	if (FilePath == NULL)
	{
		MessageBox(NULL, TEXT("File path can not be Empty"), TEXT("ERROR!"), MB_OK);
		return hbmScreen;
	}

	HDC hdcScreen = GetDC(NULL);	// 全屏幕DC
	HDC hdcMemDC = CreateCompatibleDC(hdcScreen);	// 创建兼容内存DC

	// 截取获取的长度及起点
	INT ixStart;
	INT iyStart;
	INT iX;
	INT iY;

	// 判断是否截全屏
	if (lpRect == NULL)
	{
		ixStart = iyStart = 0;
		iX = GetSystemMetrics(SM_CXSCREEN);	// 屏幕宽
		iY = GetSystemMetrics(SM_CYSCREEN);	// 屏幕高
	}
	else
	{
		ixStart = lpRect->left;
		iyStart = lpRect->top;
		iX = lpRect->right - lpRect->left;
		iY = lpRect->bottom - lpRect->top;
	}

	// 判断兼容内存DC是否成功建立
	if (!hdcMemDC)
	{
		MessageBox(NULL, TEXT("CreateCompatibleDC has Failed"), TEXT("ERROR!"), MB_OK);
		//DeleteObject(hbmScreen);
		//DeleteDC(hdcMemDC);
		DeleteDC(hdcScreen);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// 通过窗口DC 创建一个兼容位图
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

	// 将位图块传送到我们兼容的内存DC中
	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(
		hdcMemDC,				// 目的DC
		0, 0,					// 目的DC的 x,y 坐标
		iX, iY,					// 目的 DC 的宽高
		hdcScreen,				// 来源DC
		ixStart, iyStart,       // 来源DC的 x,y 坐标
		SRCCOPY))				// 粘贴方式
	{
		MessageBox(NULL, TEXT("BitBlt has Failed"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		DeleteDC(hdcScreen);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// 获取位图信息并存放在 bmpScreen 中
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

	// 在 32-bit Windows 系统上, GlobalAlloc 和 LocalAlloc 是由 HeapAlloc 封装来的
	// handle 指向进程默认的堆. 所以开销比 HeapAlloc 要大
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

	// 获取兼容位图的位并且拷贝结果到一个 lpbitmap 中.
	GetDIBits(
		hdcScreen,	// 设备环境句柄
		hbmScreen,  // 位图句柄
		0,			// 指定检索的第一个扫描线
		(UINT)bmpScreen.bmHeight,	// 指定检索的扫描线数
		lpbitmap,	// 指向用来检索位图数据的缓冲区的指针
		(BITMAPINFO *)&bi,	// 该结构体保存位图的数据格式
		DIB_RGB_COLORS	// 颜色表由红、绿、蓝（RGB）三个直接值构成
	);

	// 创建一个文件来保存文件截图
	hFile = CreateFile(
		FilePath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// 将图片头(headers)的大小, 加上位图的大小来获得整个文件的大小
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// 设置 Offset 偏移至位图的位(bitmap bits)实际开始的地方
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	// 文件大小
	bmfHeader.bfSize = dwSizeofDIB;

	// 位图的 bfType 必须是字符串 "BM"
	bmfHeader.bfType = 0x4D42;	// BM

	dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	// 解锁堆内存并释放
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	// 关闭文件句柄
	CloseHandle(hFile);

	// 释放DC
	DeleteDC(hdcMemDC);
	DeleteDC(hdcScreen);
	ReleaseDC(NULL, hdcScreen);

	return hbmScreen;
}
#pragma endregion

// 文件名与路径生成
#pragma region File Name & File Path
// 时间字符串文件名生成函数
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

// stock代码与时间文件名生成函数
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

// 设置窗口风格
#pragma region Composition
// 设置窗口风格函数
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

// 鼠标选取截图区域
#pragma region Mouse Select Rect
//opt.window_appearance = ACCENT_ENABLE_BLURBEHIND;				毛玻璃
//opt.color = 0x32282828;
//opt.window_appearance = ACCENT_ENABLE_TRANSPARENTGRADIENT;	透明
//opt.color = 0x00000000;

// 模糊背景窗口过程函数
LRESULT CALLBACK BlurBackGroundWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static RECT rectValidate;
	static POINTS points_down = { 0,0 };
	static POINTS points_up = { 0,0 };
	static POINTS points = { 0,0 };
	static HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
	
	switch (uMsg)	// 根据不同的消息类型进行不同的处理
	{
	case WM_CREATE:	// 若是创建窗口消息
		rectValidate.top = ((LPCREATESTRUCT)lParam)->y;
		rectValidate.bottom = rectValidate.top + ((LPCREATESTRUCT)lParam)->cy;
		rectValidate.left = ((LPCREATESTRUCT)lParam)->x;
		rectValidate.right = rectValidate.left + ((LPCREATESTRUCT)lParam)->cx;
		break;

	case WM_PAINT:	// 若是系统重绘消息
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);	// 调用窗口绘制函数
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// 更新客户区的显示，使无效区域变有效
		DeleteObject(hdc);
		break;
	}
	case WM_PRINT:	// 若是客户区绘制消息
	{
		HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
		PAINTSTRUCT ps;
		ps.rcPaint = rectShow;
		HDC hdc = BeginPaint(hwnd, &ps);	// 调用窗口绘制函数
		FrameRect(hdc, &ps.rcPaint, hbr);
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// 更新客户区的显示，使无效区域变有效
		DeleteObject(hdc);
		break;
	}
	case WM_KEYDOWN:	// 若是键盘按下消息
		if (wParam == VK_ESCAPE)	// 若是ESC键 
			DestroyWindow(hwnd);	// 摧毁窗口并发送一条WM_DESTROY消息
		break;

	case WM_MOUSEMOVE:	// 若是鼠标在窗口内移动
		if (wParam == MK_LBUTTON) {	// 若是按住鼠标左键拖动
			points = MAKEPOINTS(lParam);
			rectShow.top = points_down.y < points.y ? points_down.y : points.y;
			rectShow.bottom = points_down.y > points.y ? points_down.y : points.y;
			rectShow.left = points_down.x < points.x ? points_down.x : points.x;
			rectShow.right = points_down.x > points.x ? points_down.x : points.x;
			SendMessage(hwnd, WM_PRINT, NULL, NULL);
		}
		break;

	case WM_LBUTTONDOWN:	// 若是按下鼠标左键
		points_down = MAKEPOINTS(lParam);
		points = points_down;
		SendMessage(hwnd, WM_PRINT, NULL, NULL);
		break;
		
	case WM_LBUTTONUP:	// 若是释放鼠标左键
		points_up = MAKEPOINTS(lParam);
		rectShow.top = points_down.y < points_up.y ? points_down.y : points_up.y;
		rectShow.bottom = points_down.y > points_up.y ? points_down.y : points_up.y;
		rectShow.left = points_down.x < points_up.x ? points_down.x : points_up.x;
		rectShow.right = points_down.x > points_up.x ? points_down.x : points_up.x;
		DestroyWindow(hwnd);	// 摧毁窗口并发送一条WM_DESTROY消息
		break;

	case WM_DESTROY:	// 若是窗口摧毁消息
		DeleteObject(hbr);
		bRSRrun = false;
		PostQuitMessage(0);	// 向系统表明有个线程有终止请求，用来响应WM_DESTROY消息
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);	// 调用默认窗口过程为应用程序没有处理的窗口消息提供默认的处理
	}
	return 0;
}

// 创建模糊背景窗口
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

// 鼠标选取截图区域函数（弃用）
//void MouseSelectRect(RECT &rect) {
//	POINT point_down;
//	POINT point_up;
//	bool wait = true;
//	while (wait) {	// 捕获最后一次释放鼠标左键的事件
//		if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
//			GetCursorPos(&point_up);
//			wait = false;
//		}
//		Sleep(100);	// 降低等待时间CPU占用
//	}
//	bool click = true;
//	while (click) {	// 获取鼠标选取的矩形的坐标
//		if (GetAsyncKeyState(VK_LBUTTON) >> sizeof(char)) {
//			bool move = true;
//			GetCursorPos(&point_down);
//			while (move) {
//				if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
//					GetCursorPos(&point_up);
//					move = false;
//					click = false;
//				}
//				Sleep(100);	// 降低等待时间CPU占用
//			}
//		}
//		Sleep(100);	// 降低等待时间CPU占用
//	}
//
//	rect.top = point_down.y < point_up.y ? point_down.y : point_up.y;
//	rect.bottom = point_down.y < point_up.y ? point_up.y : point_down.y;
//	rect.left = point_down.x < point_up.x ? point_down.x : point_up.x;
//	rect.right = point_down.x < point_up.x ? point_up.x : point_down.x;
//}
#pragma endregion

// Tray（主窗口）
#pragma region Tray
// 主窗口过程函数
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
		if (MessageBox(NULL, L"真的要退出吗？", L"提示", MB_YESNO) == IDYES)
			DestroyWindow(hWnd);	// 摧毁窗口并发送一条WM_DESTROY消息
		break;

	case WM_HOTKEY:
	{
		WORD high = HIWORD(lParam);	// 虚拟键码
		WORD low = LOWORD(lParam);	// 低位字指定要与高位字指定的键结合使用以生成WM_HOTKEY消息的键
		
		switch (low)
		{
		case MOD_CONTROL:
			switch (high)
			{
			case 'R':	// 重新选取截图区域(Ctrl+R)
				SetForegroundWindow(hWnd);
				Rerect(hInstance, hWnd);
				break;
			case 'P':	// 设置存储路径(Ctrl+P)
				SetSavePath(hWnd);
				break;
			case 'D':	// 设置data路径(Ctrl+D)
				SetDataPath(hWnd);
				break;
			case 'X':	// 退出(Ctrl+X)
				SetForegroundWindow(hWnd);
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
			}
			break;
		case MOD_CONTROL | MOD_ALT:
			switch (high) {
			case 'F':	// 截全屏(Ctrl+Alt+F)
				if (!FullSC())
					return 0;
				break;
			case 'R':	// 截矩形区域(Ctrl+Alt+R)
				if (!RectSC())
					return 0;
				break;
			case 'S':	// 开始采集(Ctrl+Alt+S)
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
			case IDM_RERECT:	// 重新选取截图区域(Ctrl+R)
				Rerect(hInstance, hWnd);
				break;

			case IDM_SETSAVEPATH:	// 设置存储路径(Ctrl+P)
				SetSavePath(hWnd);
				break;
			
			case IDM_SETDATAPATH:	// 设置data路径(Ctrl+D)
				SetDataPath(hWnd);
				break;
			
			case IDM_SETRDAYS:	// 设置时间跨度
				SetRDays(hInstance, hWnd);
				break;

			case IDM_FULL:	// 截全屏(Ctrl+Alt+F)
				if (!FullSC())
					return 0;
				break;
			
			case IDM_RECT:	// 截矩形区域(Ctrl+Alt+R)
				if (!RectSC())
					return 0;
				break;
			
			case IDM_START:	// 开始采集(Ctrl+Alt+S)
				if (!StartSC())
					return 0;
				break;
			
			case IDM_EXIT:	// 退出(Ctrl+X)
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

// 开始采集
int StartSC() {
	HANDLE h = CreateThread(NULL, 0, SSCThread, NULL, 0, NULL);
	if (h == INVALID_HANDLE_VALUE || h == NULL)
		return 0;
	WaitForSingleObject(h, INFINITE);
	CloseHandle(h);
	return 1;
}

// 截矩形区域
int RectSC() {
	// 截图位图文件存储路径的目录是否存在，不存在则创建该目录
	if (taccess(FileSavePath.c_str(), F_OK))
		if (tmkdir(FileSavePath.c_str())) {
			MessageBox(NULL, TEXT("Can not Found & Create folder"), TEXT("ERROR!"), MB_OK);
			return 0;
		}

	// 截全屏要把&rectShow换成FULLSCREENCAPTURE，或手动设置INI文件
	ghBitmap = ScreenCapture(TimeStringFilePath(FileSavePath, ext), ColorBit, &rectShow);

	return 1;
}

// 截全屏
int FullSC() {
	// 截图位图文件存储路径的目录是否存在，不存在则创建该目录
	if (taccess(FileSavePath.c_str(), F_OK))
		if (tmkdir(FileSavePath.c_str())) {
			MessageBox(NULL, TEXT("Can not Found & Create folder"), TEXT("ERROR!"), MB_OK);
			return 0;
		}

	// 截全屏要把&rectShow换成FULLSCREENCAPTURE，或手动设置INI文件
	ghBitmap = ScreenCapture(TimeStringFilePath(FileSavePath, ext), ColorBit, FULLSCREENCAPTURE);

	return 1;
}

// 设置时间跨度
inline void SetRDays(HINSTANCE hInstance, HWND hWnd) {
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_INPUTRDAYS), hWnd, (DLGPROC)RDaysDlgPro);
}

// 设置data路径
void SetDataPath(HWND hWnd) {
	TCHAR szBuffer[MAX_PATH] = { 0 };
	OPENFILENAME file = { 0 };
	file.hwndOwner = hWnd;
	file.lStructSize = sizeof(file);
	file.lpstrFilter = L"文本文件(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0";	// 要选择的文件后缀 
	file.lpstrInitialDir = L".\\";	// 默认的文件路径 
	file.lpstrFile = szBuffer;	// 存放文件路径的缓冲区 
	file.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	file.nFilterIndex = 0;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;	// 标志如果是多选要加上OFN_ALLOWMULTISELECT
	if (GetOpenFileName(&file)) {
		DataPath = szBuffer;
		ini_parser->SetValue(TEXT("Common"), TEXT("DataPath"), DataPath);
		WriteINI();
	}
}

// 设置存储路径
void SetSavePath(HWND hWnd) {
	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = hWnd;	// 拥有者窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
	bi.pszDisplayName = szBuffer;	// 接收文件夹的缓冲区  
	bi.lpszTitle = TEXT("选择一个文件夹");	// 标题  
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (SHGetPathFromIDList(idl, szBuffer)) {
		FileSavePath = szBuffer;
		ini_parser->SetValue(TEXT("Common"), TEXT("ScreenCaptureSavePath"), FileSavePath);
		WriteINI();
	}
}

// 重新选取截图区域
int Rerect(HINSTANCE hInstance, HWND hWnd) {
	MessageBox(NULL, TEXT("请选取截图区域"), TEXT("提示"), MB_OK);
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

// 初始化托盘
void initTray(HWND parent) {
	Tray.cbSize = sizeof(Tray);
	Tray.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(MAINICON));
	Tray.hWnd = parent;
	wcscpy_s(Tray.szTip, L"图像采集程序");
	Tray.uCallbackMessage = WM_NOTIFY_MAIN;
	Tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Tray.uID = 101;
	Shell_NotifyIcon(NIM_ADD, &Tray);
	Shell_NotifyIcon(NIM_SETVERSION, &Tray);
}

// 初始化窗口类
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
		MessageBox(NULL, TEXT("注册窗口类失败"), TEXT("ERROR!"), MB_OK);
		return 0;
	}
	return 1;
}

// 创建主窗口（托盘）
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

// 对话框
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

// INI相关
#pragma region INIRelation
// 读取INI文件初始化INI类
void initINI() {
	TCHAR *inipath = new TCHAR[MAX_PATH];
	TCHAR *driver = new TCHAR[10];
	TCHAR *dir = new TCHAR[MAX_PATH];
	TCHAR *filename = new TCHAR[50];
	TCHAR *ext = new TCHAR[10];
	GetModuleFileName(NULL, inipath, MAX_PATH);
	t_splitpath_s(inipath, driver, 10, dir, MAX_PATH, filename, 50, ext, 10);
	t_makepath_s(inipath, MAX_PATH, driver, dir, L"config", L"ini");
	INIPath = inipath;	// 固定INI文件路径
	delete[] inipath, driver, dir, filename, ext;

	if (ini_parser->Init(INIPath, ios_base::in))	// 文件存在，读取
		ini_parser->Read();
	else
	{	// 文件不存在，创建新文件，生成默认值
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

// 读取INI结构信息至程序变量
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

// 将当前INI结构信息写入INI文件
inline void WriteINI() {
	ini_parser->Init(INIPath, ios_base::out | ios_base::trunc);
	ini_parser->Write();
	ini_parser->Close();
}

// 清理INI类
inline void CloseINI() {
	delete ini_parser;
}

// 判断是否用鼠标选取截图区域（废弃）
//bool isMouseSelectRect() {
//	if (fromString<bool>(ini_parser->GetValue(TEXT("Common"), TEXT("MouseSelectRect")))) {
//		if (IDNO == MessageBox(NULL, TEXT("下次是否重新选取截图区域？"), TEXT("提示"), MB_YESNO))
//			ini_parser->SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(false));
//		else
//			ini_parser->SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(true));
//		return true;
//	}
//	return false;
//}
#pragma endregion

// DATA相关
#pragma region DATARelation
// 读取data内容并采集图像
int ReadData2Start() {
	DATANode data_node;
	InitDriver();
	if (!bLoadDriver)
		return 0;
	if (!data_parser->Init(DataPath, ios_base::in)) {
		ExitDriver();
		return 0;
	}
	
	// 截图位图文件存储路径的目录是否存在，不存在则创建该目录
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

// 清理DATA类
inline void CloseDATA() {
	delete data_parser;
}
#pragma endregion

// 虚拟输入控制
#pragma region Simulate Input Control
void InitSendkbInput(HWND hwnd) {
	SetForegroundWindow(hwnd); // 将窗口提到前台
	SetFocus(hwnd); // 激活窗口
	Sleep(5000);
}

//void SimSendkbInput(WORD nScanCode, int nSleepTime, bool isExtended)
//{
//	if (isExtended) {
//		INPUT stInput;
//		stInput.type = INPUT_KEYBOARD;	// 类型为键盘
//		stInput.ki.wVk = 0;
//		stInput.ki.wScan = nScanCode;
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;	// 只使用ScanCode构造
//		stInput.ki.time = GetTickCount();
//		stInput.ki.dwExtraInfo = GetMessageExtraInfo();
//
//		int nCnt = SendInput(1, &stInput, sizeof(INPUT));	// 模拟按下
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;
//		stInput.ki.time = GetTickCount();
//		Sleep(nSleepTime);
//		nCnt = SendInput(1, &stInput, sizeof(INPUT));	// 模拟松开
//	}
//	else {
//		INPUT stInput;
//		stInput.type = INPUT_KEYBOARD;	// 类型为键盘
//		stInput.ki.wVk = 0;
//		stInput.ki.wScan = nScanCode;
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE;	// 只使用ScanCode构造
//		stInput.ki.time = GetTickCount();
//		stInput.ki.dwExtraInfo = GetMessageExtraInfo();
//
//		int nCnt = SendInput(1, &stInput, sizeof(INPUT));	// 模拟按下
//		stInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
//		stInput.ki.time = GetTickCount();
//		Sleep(nSleepTime);
//		nCnt = SendInput(1, &stInput, sizeof(INPUT));	// 模拟松开
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
//			MessageBox(NULL, L"不是有效字符", L"ERROR::SimSendStringInput(tstring&)", MB_OK);
//			break;
//		}
//}
#pragma endregion

// 线程
#pragma region Thread
// 选择截图区域窗口线程函数
unsigned long WINAPI RSRThread(void* param) {
	MSG msg = { 0 };
	HWND hwnd = CreateBlurBackgroundWindow(*(HINSTANCE*)param);
	
	if (hwnd == NULL) {
		MessageBox(NULL, L"窗口未创建", L"ERROR!", MB_OK);
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
		MessageBox(NULL, L"ReadData2Start()函数出错",L"ERROR!", MB_OK);
	return 0;
}
#pragma endregion

// 热键
#pragma region HotKey
void InitHotKey(HWND hwnd) {
	RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_NOREPEAT, 'R');	// 重新选取截图区域(Ctrl+R)
	RegisterHotKey(hwnd, 2, MOD_CONTROL | MOD_NOREPEAT, 'P');	// 设置存储路径(Ctrl+P)
	RegisterHotKey(hwnd, 3, MOD_CONTROL | MOD_NOREPEAT, 'D');	// 设置data路径(Ctrl+D)
	RegisterHotKey(hwnd, 4, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'F');	// 截全屏(Ctrl+Alt+F)
	RegisterHotKey(hwnd, 5, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'R');	// 截矩形区域(Ctrl+Alt+R)
	RegisterHotKey(hwnd, 6, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'S');	// 开始采集(Ctrl+Alt+S)
	RegisterHotKey(hwnd, 7, MOD_CONTROL | MOD_NOREPEAT, 'X');	// 退出(Ctrl+X)
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

// 驱动
#pragma region Driver
// 初始化驱动
void InitDriver() {
	//加载驱动
	bLoadDriver = ManageDriver(KEYMOUSE_DRIVER_NAME, DRIVER_FUNC_INSTALL);
	if (!bLoadDriver)
		MessageBox(NULL, L"驱动加载失败!", L"ERROR!", MB_OK);
}

// 卸载驱动
void ExitDriver() {
	if (bLoadDriver)
		ManageDriver(KEYMOUSE_DRIVER_NAME, DRIVER_FUNC_REMOVE);
}
#pragma endregion

// 程序入口
#pragma region WinMain
// 程序入口函数
int CALLBACK tWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	SetProcessDPIAware();						// 高DPI时禁用缩放
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