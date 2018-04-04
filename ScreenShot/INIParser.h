#pragma once
#ifndef _INIPARSER_H_
#define _INIPARSER_H_
#include "stdafx.h"

//INI�ļ��ڵ�洢�ṹ  
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

//��ֵ�Խṹ��  
class SubNode
{
public:
	void InsertElement(tstring key, tstring value);
	map<tstring, tstring> sub_node;
};

//INI�ļ�������  
class INIParser
{
public:
	INIParser();
	~INIParser();

public:
	int ReadINI(tstring path);                                                   //��ȡINI�ļ�
	tstring GetValue(tstring root, tstring key);                                   //�ɸ��ڵ�ͼ���ȡֵ
	vector<ININode>::size_type GetSize() { return map_ini.size(); }               //��ȡINI�ļ��Ľڵ���
	vector<ININode>::size_type SetValue(tstring root, tstring key, tstring value);   //���ø��ڵ�ͼ���ȡֵ
	vector<ININode>::size_type DeleteKey(tstring root, tstring key);	//ɾ����
	vector<ININode>::size_type DeleteRoot(tstring root);				//ɾ�����ڵ�
	int WriteINI(tstring path);				//д��INI�ļ�
	void Clear() { map_ini.clear(); }		//���
	void Travel();							//������ӡINI�ļ�
private:
	map <tstring, SubNode> map_ini;     //INI�ļ����ݵĴ洢����
};
#endif