#include "DATAParser.h"

int R_days = 0;	// 时间跨度

int DATAParser::Read()
{
	tstring str_line = TEXT("");
	streampos pos = fs.tellg();
	while (getline(fs, str_line))
	{
		tstring::size_type left_pos = 0;
		tstring::size_type right_pos = 0;
		tstring str_end = TEXT("");
		
		if ((str_line.npos != (left_pos = str_line.find(TEXT("[")))) && (str_line.npos != (right_pos = str_line.find(TEXT("]")))))
		{
			if (this->bRead) {
				this->code = str_line.substr(left_pos + 1, right_pos - 1);
				this->bRead = false;
			}
			else {
				fs.seekg(pos);
				this->bRead = true;
				return 1;
			}
		}

		if ((str_line.npos != (left_pos = str_line.find(TEXT("-")))) && (str_line.npos != (right_pos = str_line.rfind(TEXT("-")))))
			if (left_pos != right_pos)
				str_end = this->TrimString(str_line);

		if ((!this->code.empty()) && (!str_end.empty()))
		{
			Date date_end(str_end);
			DATANode data_node(date_end);
			list_data.push_back(data_node);
		}

		pos = fs.tellg();
	}
	if (!bRead) {
		bRead = true;
		return 1;
	}
	return 0;
}

// 获得值
int DATAParser::GetValue(DATANode& data_node)
{
	if (!list_data.empty()) {
		list<DATANode>::iterator itr = this->list_data.begin();
		data_node = *itr;
		list_data.pop_front();
		return 1;
	}
	else
		return 0;
}

// 遍历
void DATAParser::Travel()
{
	tcout << TEXT("[") << code << TEXT("]") << endl;
	for (list<DATANode>::iterator itr = this->list_data.begin(); itr != this->list_data.end(); ++itr)
		tcout << itr->begin.Transto() << TEXT(" -> ") << itr->end.Transto() << endl;
}

tstring Date::Transto() {
	tstring str = TEXT("");
	if (!this->empty())
		str = this->year + TEXT("-") + this->month + TEXT("-") + this->day;
	return str;
}

void Date::Transfrom(tstring date)
{
	tstring::size_type first_pos = 0;
	tstring::size_type second_pos = 0;
	if ((date.npos != (first_pos = date.find(TEXT("-")))) && (date.npos != (second_pos = date.rfind(TEXT("-")))))
	{
		this->year = date.substr(0, first_pos);
		this->month = date.substr(first_pos + 1, second_pos - first_pos - 1);
		this->day = date.substr(second_pos + 1, date.npos);
	}
}

bool isBissextile(int year) {
	if (year % 100 == 0) {
		if (year % 400 == 0)
			return true;
	}
	else
		if (year % 4 == 0)
			return true;
	return false;
}

// 计算起始日期
Date DATANode::computBegin(Date end)
{
	int month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	Date begin;
	int year = fromString<int>(end.year);
	int mon = fromString<int>(end.month) - 1;
	int day = fromString<int>(end.day);
	if (R_days > 0) {
		int range = R_days - 1;
		while (range >= day) {
			range -= day;
			mon--;
			if (mon < 0) {
				year--;
				mon = 11;
			}
			day = month[mon];
			if (mon == 1 && isBissextile(year))
				day++;
		}
		day -= range;
		begin.year = toString(year);
		if (mon < 9)
			begin.month = L"0" + toString(mon + 1);
		else
			begin.month = toString(mon + 1);
		if (day < 10)
			begin.day = L"0" + toString(day);
		else
			begin.day = toString(day);
	}
	return begin;
}
