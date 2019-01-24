#pragma once
#ifndef _DATAPARSER_H_
#define _DATAPARSER_H_
#include "Parser.h"

// Date存储结构
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

// DATA文件节点存储结构  
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

// DATA文件操作类  
class DATAParser : public virtual Parser {
public:
	DATAParser() {}
	~DATAParser() {
		this->Clear();
	}
	int Read();															// 读取DATA文件
	int GetValue(DATANode& data_node);									// 获取值
	list<DATANode>::size_type GetSize() { return list_data.size(); }	// 获取list_data的节点数
	void Clear() { code = TEXT(""); list_data.clear(); }		// 清空
	void Travel();												// 遍历打印list_data
	tstring code = TEXT("");	// stock code
private:
	bool bRead = true;
	list<DATANode> list_data;	// DATA文件部分内容的存储变量
};
#endif