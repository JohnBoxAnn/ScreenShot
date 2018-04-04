#pragma once
#ifndef _STDAFX_H_
#define _STDAFX_H_
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <map>
#include <io.h>
#include <direct.h>
#include <ctime>
using namespace std;

// 自动字符集适配，新名字为ASC2字符集名字前加t
#ifdef UNICODE
typedef wstring tstring;				// UNICODE字符串
typedef wostringstream tostringstream;	// UNICODE字符串输出流
typedef wistringstream tistringstream;	// UNICODE字符串输入流
typedef wstringstream tstringstream;	// UNICODE字符串输入输出流
typedef wfstream tfstream;
typedef wifstream tifstream;
typedef wofstream tofstream;
typedef wiostream tiostream;
typedef wistream tistream;
typedef wostream tostream;
#define tcout wcout
#define tcin wcin
#define tcerr wcerr
#define taccess _waccess
#define tmkdir _wmkdir
#define tmakepath _wmakepath
#define tmktemp _wmktemp
#else
typedef string tstring;					// ASC2字符串
typedef ostringstream tostringstream;	// ASC2字符串输出流
typedef istringstream tistringstream;	// ASC2字符串输入流
typedef stringstream tstringstream;		// ASC2字符串输入输出流
typedef fstream tfstream;
typedef ifstream tifstream;
typedef ofstream tofstream;
typedef iostream tiostream;
typedef istream tistream;
typedef ostream tostream;
#define tcout cout
#define tcin cin
#define tcerr cerr
#define taccess _access
#define tmkdir _mkdir
#define tmakepath _makepath
#define tmktemp _mktemp
#endif

#endif