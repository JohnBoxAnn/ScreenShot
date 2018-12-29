#pragma once
#include "stdafx.h"

inline void Poperr(tstring str, int nCode) {
	str += toString(nCode);
	MessageBox(NULL, str.c_str(), L"ERROR", MB_OK);
}

inline void Poperr(tstring str) {
	MessageBox(NULL, str.c_str(), L"ERROR", MB_OK);
}