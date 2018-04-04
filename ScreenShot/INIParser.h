#pragma once
#ifndef _INIPARSER_H_
#define _INIPARSER_H_
#include "stdafx.h"

//INI文件节点存储结构  
class ININode
{
public:
	ININode(tstring root, tstring key, tstring value)
	{
		this->root = root;
		this->key = key;
		this->value = value;
	}
	tstring root;
	tstring key;
	tstring value;
};

//键值对结构体  
class SubNode
{
public:
	void InsertElement(tstring key, tstring value);
	map<tstring, tstring> sub_node;
};

//INI文件操作类  
class INIParser
{
public:
	INIParser();
	~INIParser();

public:
	int ReadINI(tstring path);                                                   //读取INI文件
	tstring GetValue(tstring root, tstring key);                                   //由根节点和键获取值
	vector<ININode>::size_type GetSize() { return map_ini.size(); }               //获取INI文件的节点数
	vector<ININode>::size_type SetValue(tstring root, tstring key, tstring value);   //设置根节点和键获取值
	vector<ININode>::size_type DeleteKey(tstring root, tstring key);	//删除键
	vector<ININode>::size_type DeleteRoot(tstring root);				//删除根节点
	int WriteINI(tstring path);				//写入INI文件
	void Clear() { map_ini.clear(); }		//清空
	void Travel();							//遍历打印INI文件
private:
	map <tstring, SubNode> map_ini;     //INI文件内容的存储变量
};
#endif