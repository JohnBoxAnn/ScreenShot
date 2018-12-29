#pragma once
#ifndef _PARSER_H_
#define _PARSER_H_
#include "stdafx.h"

// ½âÎöÆ÷Àà
class Parser {
public:
	Parser() { fs.imbue(locale("")); }
	~Parser() {}
	int Init(tstring path, int mode);
	explicit __CLR_OR_THIS_CALL operator bool() const { return !!fs; }
	bool __CLR_OR_THIS_CALL operator!() const { return !fs; }
	int Read();
	int Write();
	tstring &TrimString(tstring &str);
	void Clear();
	void Travel();
	void Close() { fs.flush(); fs.clear(); fs.close(); }
protected:
	tfstream fs;
};
#endif
