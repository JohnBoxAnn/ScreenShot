#include "INIParser.h"

//#define INIDEBUG

INIParser::INIParser()
{
}


INIParser::~INIParser()
{
}

//************************************************************************
// ��������:    	TrimString
// ����Ȩ��:    	public 
// ��������:		2017/01/05
// �� �� ��:		
// ����˵��:		ȥ���ո�
// ��������: 		string & str	������ַ���
// �� �� ֵ:   		std::string &	����ַ���
//************************************************************************
tstring &TrimString(tstring &str)
{
	tstring::size_type pos = 0;
	while (str.npos != (pos = str.find(TEXT(" "))))
		str = str.replace(pos, pos + 1, TEXT(""));
	return str;
}

//************************************************************************
// ��������:    	ReadINI
// ����Ȩ��:    	public 
// ��������:		2017/01/05
// �� �� ��:		
// ����˵��:		��ȡINI�ļ��������䱣�浽map�ṹ��
// ��������: 		string path	INI�ļ���·��
// �� �� ֵ:   		int
//************************************************************************
int INIParser::ReadINI(tstring path)
{
	wifstream in_conf_file(path.c_str());
	if (!in_conf_file) return 0;
	tstring str_line = TEXT("");
	tstring str_root = TEXT("");
	vector<ININode> vec_ini;
	while (getline(in_conf_file, str_line))
	{
		tstring::size_type left_pos = 0;
		tstring::size_type right_pos = 0;
		tstring::size_type equal_div_pos = 0;
		tstring str_key = TEXT("");
		tstring str_value = TEXT("");
		if ((str_line.npos != (left_pos = str_line.find(TEXT("[")))) && (str_line.npos != (right_pos = str_line.find(TEXT("]")))))
		{
			//tcout << str_line.substr(left_pos+1, right_pos-1) << endl;
			str_root = str_line.substr(left_pos + 1, right_pos - 1);
		}

		if (str_line.npos != (equal_div_pos = str_line.find(TEXT("="))))
		{
			str_key = str_line.substr(0, equal_div_pos);
			str_value = str_line.substr(equal_div_pos + 1, str_line.size() - 1);
			str_key = TrimString(str_key);
			str_value = TrimString(str_value);
			//tcout << str_key << "=" << str_value << endl;
		}

		if ((!str_root.empty()) && (!str_key.empty()) && (!str_value.empty()))
		{
			ININode ini_node(str_root, str_key, str_value);
			vec_ini.push_back(ini_node);
			//tcout << vec_ini.size() << endl;
		}
	}
	in_conf_file.close();
	in_conf_file.clear();

	//vector convert to map
	map<tstring, tstring> map_tmp;
	for (vector<ININode>::iterator itr = vec_ini.begin(); itr != vec_ini.end(); ++itr)
	{
		map_tmp.insert(pair<tstring, tstring>(itr->root, TEXT("")));
	}	//��ȡ�����ڵ�
	for (map<tstring, tstring>::iterator itr = map_tmp.begin(); itr != map_tmp.end(); ++itr)
	{
#ifdef INIDEBUG
		tcout << "���ڵ㣺 " << itr->first << endl;
#endif	//INIDEBUG
		SubNode sn;
		for (vector<ININode>::iterator sub_itr = vec_ini.begin(); sub_itr != vec_ini.end(); ++sub_itr)
		{
			if (sub_itr->root == itr->first)
			{
#ifdef INIDEBUG
				tcout << "��ֵ�ԣ� " << sub_itr->key << "=" << sub_itr->value << endl;
#endif	//INIDEBUG
				sn.InsertElement(sub_itr->key, sub_itr->value);
			}
		}
		map_ini.insert(pair<tstring, SubNode>(itr->first, sn));
	}
	return 1;
}

//************************************************************************
// ��������:    	GetValue
// ����Ȩ��:    	public 
// ��������:		2017/01/05
// �� �� ��:		
// ����˵��:		���ݸ����ĸ����ͼ�ֵ�����������ֵ
// ��������: 		string root		������ĸ����
// ��������: 		string key		������ļ�
// �� �� ֵ:   		std::string		�������ֵ
//************************************************************************
tstring INIParser::GetValue(tstring root, tstring key)
{
	map<tstring, SubNode>::iterator itr = map_ini.find(root);
	map<tstring, tstring>::iterator sub_itr = itr->second.sub_node.find(key);
	if (!(sub_itr->second).empty())
		return sub_itr->second;
	return TEXT("");
}

//************************************************************************
// ��������:    	WriteINI
// ����Ȩ��:    	public 
// ��������:		2017/01/05
// �� �� ��:		
// ����˵��:		����XML����Ϣ���ļ���
// ��������: 		string path	INI�ļ��ı���·��
// �� �� ֵ:		int
//************************************************************************
int INIParser::WriteINI(tstring path)
{
	tofstream out_conf_file(path.c_str(), ios_base::out | ios_base::trunc);
	if (!out_conf_file)
		return 0;
	//tcout << map_ini.size() << endl;
	for (map<tstring, SubNode>::iterator itr = map_ini.begin(); itr != map_ini.end(); ++itr)
	{
		//tcout << itr->first << endl;
		out_conf_file << TEXT("[") << itr->first << TEXT("]") << endl;
		for (map<tstring, tstring>::iterator sub_itr = itr->second.sub_node.begin(); sub_itr != itr->second.sub_node.end(); ++sub_itr)
		{
			//tcout << sub_itr->first << "=" << sub_itr->second << endl;
			out_conf_file << TEXT("    ") << sub_itr->first << TEXT(" = ") << sub_itr->second << endl;
		}
		out_conf_file << endl;
	}
	
	out_conf_file.close();
	out_conf_file.clear();

	return 1;
}

//************************************************************************
// ��������:    	SetValue
// ����Ȩ��:    	public 
// ��������:		2017/01/05
// �� �� ��:		
// ����˵��:		���û�����������ֵ
// ��������: 		string root		������ĸ��ڵ�
// ��������: 		string key		������ļ�
// ��������: 		string value	�������ֵ
// �� �� ֵ:   		std::vector<ININode>::size_type	
//************************************************************************
vector<ININode>::size_type INIParser::SetValue(tstring root, tstring key, tstring value)
{
	map<tstring, SubNode>::iterator itr = map_ini.find(root);	//����
	if (map_ini.end() != itr)
	{
		//itr->second.sub_node.insert(pair<string, string>(key, value));
		itr->second.sub_node[key] = value;
	}	//���ڵ��Ѿ������ˣ�����ֵ
	else
	{
		SubNode sn;
		sn.InsertElement(key, value);
		map_ini.insert(pair<tstring, SubNode>(root, sn));
	}	//���ڵ㲻���ڣ����ֵ

	return map_ini.size();
}

//************************************************************************
// ��������:    	DeleteKey
// ����Ȩ��:    	public 
// ��������:		2018/04/02
// �� �� ��:		
// ����˵��:		ɾ��������ļ�
// ��������: 		string root		������ĸ��ڵ�
// ��������: 		string key		������ļ�
// �� �� ֵ:		std::vector<ININode>::size_type	
//************************************************************************
vector<ININode>::size_type INIParser::DeleteKey(tstring root, tstring key)
{
	map<tstring, SubNode>::iterator itr = map_ini.find(root);	//����
	if (map_ini.end() != itr)
	{
		itr->second.sub_node[key].erase();
	}	//���ڵ��Ѿ������ˣ�ɾ����
	//���ڵ㲻���ڣ�ʲô������

	return map_ini.size();
}

//************************************************************************
// ��������:    	DeleteRoot
// ����Ȩ��:    	public 
// ��������:		2018/04/02
// �� �� ��:		
// ����˵��:		ɾ��������ĸ��ڵ�
// ��������: 		string root		������ĸ��ڵ�
// �� �� ֵ:   		std::vector<ININode>::size_type	
//************************************************************************
vector<ININode>::size_type INIParser::DeleteRoot(tstring root)
{
	map<tstring, SubNode>::iterator itr = map_ini.find(root);	//����
	if (map_ini.end() != itr)
	{
		map_ini.erase(root);
	}	//���ڵ��Ѿ������ˣ�ɾ�����ڵ�
	//���ڵ㲻���ڣ�ʲô������

	return map_ini.size();
}

//************************************************************************
// ��������:    	Travel
// ����Ȩ��:    	public 
// ��������:		2017/01/05
// �� �� ��:		
// ����˵��:		������ӡINI�ļ�
// �� �� ֵ:   		void
//************************************************************************
void INIParser::Travel()
{
	for (map<tstring, SubNode>::iterator itr = this->map_ini.begin(); itr != this->map_ini.end(); ++itr)
	{
		//root
		tcout << TEXT("[") << itr->first << TEXT("]") << endl;
		for (map<tstring, tstring>::iterator itr1 = itr->second.sub_node.begin(); itr1 != itr->second.sub_node.end();
			++itr1)
		{
			tcout << TEXT("    ") << itr1->first << TEXT(" = ") << itr1->second << endl;
		}
	}
}

inline void SubNode::InsertElement(tstring key, tstring value)
{
	sub_node.insert(pair<tstring, tstring>(key, value));
}
