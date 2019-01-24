#pragma once
#ifndef _DATAPARSER_H_
#define _DATAPARSER_H_
#include "Parser.h"

// Date�洢�ṹ
class Date {
public:
	Date(tstring date) {
		Transfrom(date);
	}
	Date() :year(TEXT("1970")), month(TEXT("01")), day(TEXT("01")) {}
	~Date() {}
	void Transfrom(tstring date);
	tstring Transto();
	bool empty() const {
		if (year.empty() || month.empty() || day.empty())
			return false;
		return true;
	}
	tstring year;
	tstring month;
	tstring day;
};

// DATA�ļ��ڵ�洢�ṹ  
class DATANode {
public:
	DATANode(Date end) :begin(), end(end) {
		this->begin = computBegin(end);
	}
	DATANode() :begin(), end() {}
	~DATANode() {}
	Date computBegin(Date end);
	Date begin;
	Date end;
};

// DATA�ļ�������  
class DATAParser : public virtual Parser {
public:
	DATAParser() {}
	~DATAParser() {
		this->Clear();
	}
	int Read();															// ��ȡDATA�ļ�
	int GetValue(DATANode& data_node);									// ��ȡֵ
	list<DATANode>::size_type GetSize() { return list_data.size(); }	// ��ȡlist_data�Ľڵ���
	void Clear() { code = TEXT(""); list_data.clear(); }		// ���
	void Travel();												// ������ӡlist_data
	tstring code = TEXT("");	// stock code
private:
	bool bRead = true;
	list<DATANode> list_data;	// DATA�ļ��������ݵĴ洢����
};
#endif