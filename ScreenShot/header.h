#pragma once
#ifndef _HEADER_H_
#define _HEADER_H_
#include "INIParser.h"
#include "DATAParser.h"
#include "key.h"
#include "km.h"
#include <ctime>
#include <Shlobj.h>
#include <io.h>
#include <direct.h>

#define ColorBit (WORD)32						// 颜色位数
#define FULLSCREENCAPTURE NULL					// 截全屏指令
#define DEFULTSavePath TEXT(".\\screenshot")	// 默认截图文件存储路径
#define DEFULTDataPath TEXT(".\\data.txt")		// 默认数据文件路径
#define R_OK 4									// Test for read permission.
#define W_OK 2									// Test for write permission.
#define X_OK 1									// Test for execute permission.
#define F_OK 0									// Test for existence.
#define WM_NOTIFY_MAIN 3141

// 以下用作改变窗口风格（感谢Github上的TranslucentTB项目给予的灵感）
struct ACCENTPOLICY
{
	int nAccentState;
	int nFlags;
	int nColor;
	int nAnimationId;
};

struct WINCOMPATTRDATA
{
	int nAttribute;
	PVOID pData;
	ULONG ulDataSize;
};

struct OPTIONS
{
	int window_appearance;
	int color;
}opt;

const int ACCENT_DISABLED = 0;
const int ACCENT_ENABLE_GRADIENT = 1;			// Makes the window a solid color specified by nColor. This mode doesn't care about the alpha channel.
const int ACCENT_ENABLE_TRANSPARENTGRADIENT = 2;// Makes the window a tinted transparent overlay. nColor is the tint color, sending nothing results in it interpreted as 0x00000000 (totally transparent, blends in with desktop)
const int ACCENT_ENABLE_BLURBEHIND = 3;			// Makes the window a tinted blurry overlay. nColor is same as above.
const int ACCENT_INVALID_STATE = 4;

typedef BOOL(WINAPI*pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
static pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(GetModuleHandle(TEXT("user32.dll")), "SetWindowCompositionAttribute");

// 全局变量声明
INIParser* ini_parser = new INIParser();	// INI类
DATAParser* data_parser = new DATAParser();	// DATA类
RECT rectShow;			// 截图矩形区域
NOTIFYICONDATA Tray;
HMENU menu;
HWND hwndCap = NULL;						// 截图指定窗口句柄
tstring FileSavePath = DEFULTSavePath;		// 图像文件存储路径
tstring DataPath = DEFULTDataPath;			// 数据文件存储路径
tstring INIPath;							// 配置文件存储路径
tstring ext = L"bmp";						// 图像文件后缀名
HBITMAP ghBitmap = NULL;					// 位图句柄
TCHAR szCaptureWindowName[100] = { L'\0' };	// 截图指定窗口名称
tostringstream oss;							// 字符串转换连接器
// int sleep_time = 100;	// 等待时间
bool bRSRrun = true;	// 选择截图区域子窗口运行标志

// 函数声明
LRESULT CALLBACK BlurBackGroundWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	// 窗口过程函数
LRESULT CALLBACK MAINPROC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RDaysDlgPro(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
unsigned long WINAPI RSRThread(void* param);
unsigned long WINAPI SSCThread(void* param);
HWND CreateMainWindow(HINSTANCE &hInstance);
int InitWindow(HINSTANCE &hInstance);
void initTray(HWND parent);
HBITMAP ScreenCapture(LPCTSTR FilePath, WORD BitCount, LPCRECT lpRect);
LPCTSTR TimeStringFilePath(tstring &SavePath, tstring &ext);
LPCTSTR CodeStringFilePath(tstring &SavePath, tstring &code, DATANode &data_node, tstring &ext);
void SetWindowBlur(HWND hWnd);
HWND CreateBlurBackgroundWindow(HINSTANCE &hInstance);
void initINI();
void ReadINI(const TCHAR* szCaptureWindowName);
inline void WriteINI();
//bool isMouseSelectRect();
inline void CloseINI();
int ReadData2Start();
inline void CloseDATA();
void InitSendkbInput(HWND hwnd);
//void SimSendkbInput(WORD nScanCode, int nSleepTime, bool isExtended);
//void SimSendStringInput(tstring &str);
int Rerect(HINSTANCE hInstance, HWND hWnd);
void SetSavePath(HWND hWnd);
void SetDataPath(HWND hWnd);
inline void SetRDays(HINSTANCE hInstance, HWND hWnd);
int FullSC();
int RectSC();
int StartSC();
void InitHotKey(HWND hwnd);
void UninitHotKey(HWND hwnd);
void InitDriver();
void ExitDriver();
#endif