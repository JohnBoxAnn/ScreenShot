#include "INIParser.h"

//#define INIDEBUG

INIParser::INIParser()
{
}


INIParser::~INIParser()
{
}

//************************************************************************
// 函数名称:    	TrimString
// 访问权限:    	public 
// 创建日期:		2017/01/05
// 创 建 人:		
// 函数说明:		去除空格
// 函数参数: 		string & str	输入的字符串
// 返 回 值:   		std::string &	结果字符串
//************************************************************************
tstring &TrimString(tstring &str)
{
	tstring::size_type pos = 0;
	while (str.npos != (pos = str.find(TEXT(" "))))
		str = str.replace(pos, pos + 1, TEXT(""));
	return str;
}

//************************************************************************
// 函数名称:    	ReadINI
// 访问权限:    	public 
// 创建日期:		2017/01/05
// 创 建 人:		
// 函数说明:		读取INI文件，并将其保存到map结构中
// 函数参数: 		string path	INI文件的路径
// 返 回 值:   		int
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
	}	//提取出根节点
	for (map<tstring, tstring>::iterator itr = map_tmp.begin(); itr != map_tmp.end(); ++itr)
	{
#ifdef INIDEBUG
		tcout << "根节点： " << itr->first << endl;
#endif	//INIDEBUG
		SubNode sn;
		for (vector<ININode>::iterator sub_itr = vec_ini.begin(); sub_itr != vec_ini.end(); ++sub_itr)
		{
			if (sub_itr->root == itr->first)
			{
#ifdef INIDEBUG
				tcout << "键值对： " << sub_itr->key << "=" << sub_itr->value << endl;
#endif	//INIDEBUG
				sn.InsertElement(sub_itr->key, sub_itr->value);
			}
		}
		map_ini.insert(pair<tstring, SubNode>(itr->first, sn));
	}
	return 1;
}

//************************************************************************
// 函数名称:    	GetValue
// 访问权限:    	public 
// 创建日期:		2017/01/05
// 创 建 人:		
// 函数说明:		根据给出的根结点和键值查找配置项的值
// 函数参数: 		string root		配置项的根结点
// 函数参数: 		string key		配置项的键
// 返 回 值:   		std::string		配置项的值
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
// 函数名称:    	WriteINI
// 访问权限:    	public 
// 创建日期:		2017/01/05
// 创 建 人:		
// 函数说明:		保存XML的信息到文件中
// 函数参数: 		string path	INI文件的保存路径
// 返 回 值:		int
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
// 函数名称:    	SetValue
// 访问权限:    	public 
// 创建日期:		2017/01/05
// 创 建 人:		
// 函数说明:		设置或添加配置项的值
// 函数参数: 		string root		配置项的根节点
// 函数参数: 		string key		配置项的键
// 函数参数: 		string value	配置项的值
// 返 回 值:   		std::vector<ININode>::size_type	
//************************************************************************
vector<ININode>::size_type INIParser::SetValue(tstring root, tstring key, tstring value)
{
	map<tstring, SubNode>::iterator itr = map_ini.find(root);	//查找
	if (map_ini.end() != itr)
	{
		//itr->second.sub_node.insert(pair<string, string>(key, value));
		itr->second.sub_node[key] = value;
	}	//根节点已经存在了，更新值
	else
	{
		SubNode sn;
		sn.InsertElement(key, value);
		map_ini.insert(pair<tstring, SubNode>(root, sn));
	}	//根节点不存在，添加值

	return map_ini.size();
}

//************************************************************************
// 函数名称:    	DeleteKey
// 访问权限:    	public 
// 创建日期:		2018/04/02
// 创 建 人:		
// 函数说明:		删除配置项的键
// 函数参数: 		string root		配置项的根节点
// 函数参数: 		string key		配置项的键
// 返 回 值:		std::vector<ININode>::size_type	
//************************************************************************
vector<ININode>::size_type INIParser::DeleteKey(tstring root, tstring key)
{
	map<tstring, SubNode>::iterator itr = map_ini.find(root);	//查找
	if (map_ini.end() != itr)
	{
		itr->second.sub_node[key].erase();
	}	//根节点已经存在了，删除键
	//根节点不存在，什么都不做

	return map_ini.size();
}

//************************************************************************
// 函数名称:    	DeleteRoot
// 访问权限:    	public 
// 创建日期:		2018/04/02
// 创 建 人:		
// 函数说明:		删除配置项的根节点
// 函数参数: 		string root		配置项的根节点
// 返 回 值:   		std::vector<ININode>::size_type	
//************************************************************************
vector<ININode>::size_type INIParser::DeleteRoot(tstring root)
{
	map<tstring, SubNode>::iterator itr = map_ini.find(root);	//查找
	if (map_ini.end() != itr)
	{
		map_ini.erase(root);
	}	//根节点已经存在了，删除根节点
	//根节点不存在，什么都不做

	return map_ini.size();
}

//************************************************************************
// 函数名称:    	Travel
// 访问权限:    	public 
// 创建日期:		2017/01/05
// 创 建 人:		
// 函数说明:		遍历打印INI文件
// 返 回 值:   		void
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
