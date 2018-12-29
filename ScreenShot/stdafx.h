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
#include <list>
using namespace std;

// �ַ������䣬������ΪASC2�ַ�������ǰ��t
#ifdef UNICODE
typedef wstring tstring;				// UNICODE�ַ���
typedef wostringstream tostringstream;	// UNICODE�ַ��������
typedef wistringstream tistringstream;	// UNICODE�ַ���������
typedef wstringstream tstringstream;	// UNICODE�ַ������������
typedef wfstream tfstream;
typedef wifstream tifstream;
typedef wofstream tofstream;
typedef wiostream tiostream;
typedef wistream tistream;
typedef wostream tostream;
#define tcout wcout
#define tcin wcin
#define tcerr wcerr
#define taccess _waccess				// io.h
#define tmkdir _wmkdir					// direct.h
#define tmakepath _wmakepath			// stdlib.h
#define tmktemp _wmktemp				// io.h
#define tWinMain wWinMain				// �������
#define t_splitpath_s _wsplitpath_s		// stdlib.h
#define t_makepath_s _wmakepath_s		// stdlib.h
#else
typedef string tstring;					// ASC2�ַ���
typedef ostringstream tostringstream;	// ASC2�ַ��������
typedef istringstream tistringstream;	// ASC2�ַ���������
typedef stringstream tstringstream;		// ASC2�ַ������������
typedef fstream tfstream;
typedef ifstream tifstream;
typedef ofstream tofstream;
typedef iostream tiostream;
typedef istream tistream;
typedef ostream tostream;
#define tcout cout
#define tcin cin
#define tcerr cerr
#define taccess _access					// io.h
#define tmkdir _mkdir					// direct.h
#define tmakepath _makepath				// stdlib.h
#define tmktemp _mktemp					// io.h
#define tWinMain WinMain				// �������
#define t_splitpath_s _splitpath_s		// stdlib.h
#define t_makepath_s _makepath_s		// stdlib.h
#endif

// ���ַ���ת������������
template <class T>
inline T fromString(const tstring &str) {
	tistringstream is(str);
	T v;
	is >> v;
	return v;
}

// ����������ת�����ַ���
template <class T>
inline tstring toString(const T &v) {
	tostringstream os;
	os << v;
	return os.str();
}

// ��һ������ת������һ������
template <class V, class R>
inline R SwapClass(const V &v) {
	R r;
	tostringstream os;
	os << v;
	tistringstream is(os.str());
	is >> r;
	return r;
}
#endif