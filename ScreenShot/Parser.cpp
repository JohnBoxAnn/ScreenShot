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
// ��������:    	TrimString
// ����Ȩ��:    	public 
// ��������:		2017/01/05
// �� �� ��:		
// ����˵��:		ȥ���ո�
// ��������: 		tstring &	������ַ���
// �� �� ֵ:   		tstring &	����ַ���
//************************************************************************
tstring &Parser::TrimString(tstring &str) {
	tstring::size_type pos = 0;
	while (str.npos != (pos = str.find(TEXT(" "))))
		str = str.replace(pos, pos + 1, TEXT(""));
	return str;
}