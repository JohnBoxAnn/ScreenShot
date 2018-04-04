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

// �Զ��ַ������䣬������ΪASC2�ַ�������ǰ��t
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
#define taccess _waccess
#define tmkdir _wmkdir
#define tmakepath _wmakepath
#define tmktemp _wmktemp
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
#define taccess _access
#define tmkdir _mkdir
#define tmakepath _makepath
#define tmktemp _mktemp
#endif

#endif