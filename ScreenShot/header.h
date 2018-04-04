#pragma once
#ifndef _HEADER_H_
#define _HEADER_H_
#include "INIParser.h"
#define ColorBit (WORD)32
#define FULLSCREENCAPTURE NULL	// 截全屏指令
#define INIPath TEXT(".\\config.ini")	// INI文件路径
#define DEFULTSavePath TEXT(".\\screenshot")	//默认截图文件存储路径
#define R_OK 4					// Test for read permission.
#define W_OK 2					// Test for write permission.
#define X_OK 1					// Test for execute permission.
#define F_OK 0					// Test for existence.

// 从字符串转换到其他类型
template <class T>
inline T fromString(const tstring &str) {
	tistringstream is(str);
	T v;
	is >> v;
	return v;
}

// 从其他类型转换到字符串
template <class T>
inline tstring toString(const T &v) {
	tostringstream os;
	os << v;
	return os.str();
}

// 从一个类型转换到另一个类型
template <class V, class R>
inline R SwapClass(const V &v) {
	R r;
	tostringstream os;
	os << v;
	tistringstream is(os.str());
	is >> r;
	return r;
}

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

LRESULT CALLBACK BlurBackGroundWindowProc(HWND, UINT, WPARAM, LPARAM);	// 窗口过程函数
LPCTSTR szWindowName = TEXT("BackGroundWindow");	// 窗口类名
#endif