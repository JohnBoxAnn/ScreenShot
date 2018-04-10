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

	// 判断文件路径是否为空
	if (FilePath == NULL)
	{
		MessageBox(NULL, TEXT("File path can not be Empty"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// 判断兼容内存DC是否成功建立
	if (!hdcMemDC)
	{
		MessageBox(NULL, TEXT("CreateCompatibleDC has Failed"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return hbmScreen;
	}

	// 通过窗口DC 创建一个兼容位图
	hbmScreen = CreateCompatibleBitmap(hdcScreen, iX, iY);

	if (!hbmScreen)
	{
		MessageBox(NULL, TEXT("CreateCompatibleBitmap has Failed"), TEXT("ERROR!"), MB_OK);
		DeleteObject(hbmScreen);
		DeleteDC(hdcMemDC);
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
	ReleaseDC(NULL, hdcScreen);

	return hbmScreen;
}
#pragma endregion

// 时间字符串文件名生成
#pragma region Time String File Path
// 时间字符串文件名生成函数
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

	case WM_PAINT:	// 若是客户区重绘消息
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);	// 调用窗口绘制函数
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// 更新客户区的显示，使无效区域变有效
		break;

	case WM_PRINT:	// 若是客户区打印消息
		ps.rcPaint = rectShow;
		hdc = BeginPaint(hwnd, &ps);	// 调用窗口绘制函数
		FrameRect(hdc, &ps.rcPaint, hbr);
		EndPaint(hwnd, &ps);
		ValidateRect(hwnd, &rectValidate);	// 更新客户区的显示，使无效区域变有效
		break;

	case WM_KEYDOWN:	// 若是键盘按下消息
		if (wParam == VK_ESCAPE)	// 若是ESC键 
			DestroyWindow(hwnd);	// 摧毁窗口并发送一条WM_DESTROY消息
		break;

	case WM_MOUSEMOVE:	// 若是鼠标在窗口内移动
		if (wParam == MK_LBUTTON) {	// 若是按住鼠标左键拖动
			points = MAKEPOINTS(lParam);
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
		PostQuitMessage(0);	// 向系统表明有个线程有终止请求，用来响应WM_DESTROY消息
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);	// 调用默认窗口过程为应用程序没有处理的窗口消息提供默认的处理  
	}
	return 0;
}

// 创建模糊背景窗口
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

// 鼠标选取截图区域函数（弃用）
void MouseSelectRect(RECT &rect) {
	POINT point_down;
	POINT point_up;
	bool wait = true;
	while (wait) {	// 捕获最后一次释放鼠标左键的事件
		if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
			GetCursorPos(&point_up);
			wait = false;
		}
		Sleep(100);	// 降低等待时间CPU占用
	}
	bool click = true;
	while (click) {	// 获取鼠标选取的矩形的坐标
		if (GetAsyncKeyState(VK_LBUTTON) >> sizeof(char)) {
			bool move = true;
			GetCursorPos(&point_down);
			while (move) {
				if (!(GetAsyncKeyState(VK_LBUTTON) >> sizeof(char))) {
					GetCursorPos(&point_up);
					move = false;
					click = false;
				}
				Sleep(100);	// 降低等待时间CPU占用
			}
		}
		Sleep(100);	// 降低等待时间CPU占用
	}

	rect.top = point_down.y < point_up.y ? point_down.y : point_up.y;
	rect.bottom = point_down.y < point_up.y ? point_up.y : point_down.y;
	rect.left = point_down.x < point_up.x ? point_down.x : point_up.x;
	rect.right = point_down.x < point_up.x ? point_up.x : point_down.x;
}
#pragma endregion

// INI相关
#pragma region INIRelation
// 读取INI文件初始化INI结构
void initINI() {
	if (!ini_parser.ReadINI(INIPath)) {	// 文件存在，读取
		// 文件不存在，创建新文件，生成默认值
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

// 读取INI结构信息至程序变量
void ReadINI(tstring &SavePath, RECT &rect, const TCHAR* CaptureWindowName) {
	SavePath = ini_parser.GetValue(TEXT("Common"), TEXT("ScreenCaptureSavePath"));
	CaptureWindowName = ini_parser.GetValue(TEXT("Common"), TEXT("CaptureWindowName")).c_str();
	rect.top = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("top")));
	rect.bottom = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("bottom")));
	rect.left = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("left")));
	rect.right = fromString<LONG>(ini_parser.GetValue(TEXT("Rect"), TEXT("right")));
}

// 将当前INI结构信息写入INI文件
inline void WriteINI() {
	ini_parser.WriteINI(INIPath);
}

// 判断是否用鼠标选取截图区域
bool isMouseSelectRect() {
	if (fromString<bool>(ini_parser.GetValue(TEXT("Common"), TEXT("MouseSelectRect")))) {
		if (IDNO == MessageBox(NULL, TEXT("下次是否重新选取截图区域？"), TEXT("提示"), MB_YESNO))
			ini_parser.SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(false));
		else
			ini_parser.SetValue(TEXT("Common"), TEXT("MouseSelectRect"), toString(true));
		return true;
	}
	return false;
}
#pragma endregion

// 程序入口
#pragma region wWinMain
// 程序入口函数
int CALLBACK tWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPTSTR pCmdLine, int nCmdShow) {
	HWND hwndCap = NULL;						// 截图指定窗口句柄
	tstring FileSavePath;						// 截图位图文件存储路径
	HBITMAP ghBitmap = NULL;					// 位图句柄
	TCHAR szCaptureWindowName[100] = { NULL };	// 截图指定窗口名称
	SetProcessDPIAware();						// 高DPI时禁用缩放
	initINI();
	ReadINI(FileSavePath, rectShow, szCaptureWindowName);
	
	// 判断是否重新选取截图矩形区域
	if (isMouseSelectRect()) {
		MessageBox(NULL, TEXT("请选取截图区域"), TEXT("提示"), MB_OK);
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

	// 截图位图文件存储路径的目录是否存在，不存在则创建该目录
	if (taccess(FileSavePath.c_str(), F_OK))
		if (tmkdir(FileSavePath.c_str())) {
			MessageBox(NULL, TEXT("Can not Found direction"), TEXT("ERROR!"), MB_OK);
			return 0;
		}

	// 如果截指定窗口则改变rectShow，否则使用从INI读取的值截图
	if (*szCaptureWindowName != NULL) {
		hwndCap = FindWindow(NULL, szCaptureWindowName);
		if (!GetWindowRect(hwndCap, &rectShow)) {
			MessageBox(NULL, TEXT("Can not Find the window to capture"), TEXT("ERROR!"), MB_OK);
			return 0;
		}
	}

	// 截全屏要把&rectShow换成FULLSCREENCAPTURE，或手动设置INI文件
	ghBitmap = ScreenCapture(TimeStringFilePath(FileSavePath), ColorBit, &rectShow);

	return 0;
}
#pragma endregion