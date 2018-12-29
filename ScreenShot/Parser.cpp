#include "Parser.h"
int Parser::Init(tstring path, int mode) {
	fs.open(path, mode);
	if (!fs) return 0;
	return 1;
}

int Parser::Read()
{
	return 0;
}

int Parser::Write()
{
	return 0;
}

void Parser::Clear() {}

void Parser::Travel() {}

//************************************************************************
// 函数名称:    	TrimString
// 访问权限:    	public 
// 创建日期:		2017/01/05
// 创 建 人:		
// 函数说明:		去除空格
// 函数参数: 		tstring &	输入的字符串
// 返 回 值:   		tstring &	结果字符串
//************************************************************************
tstring &Parser::TrimString(tstring &str) {
	tstring::size_type pos = 0;
	while (str.npos != (pos = str.find(TEXT(" "))))
		str = str.replace(pos, pos + 1, TEXT(""));
	return str;
}