#include "stdafx.h"
#include "StringUtil.h"


StringUtil::StringUtil(void)
{
}


StringUtil::~StringUtil(void)
{
}

string& StringUtil::trim(string& str)
{
	if(str.empty())
	{
		return str;
	}

	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ")+1);

	return str;
}

void StringUtil::split(vector<string>& res, const string& input, const char dem)
{
	size_t pos = input.find(dem);
	size_t beg = 0;
	while(pos != string::npos)
	{
		string w = input.substr(beg, pos-beg);
		if(w.size() > 0)
		{
			res.push_back(w);
		}
		beg = pos+1;
		pos = input.find(dem, beg);
	}
	string w = input.substr(beg, pos-beg);
	if(w.size() > 0)
	{
		res.push_back(w);
	}
}

void StringUtil::split(vector<string>& res, const string& input, const string& dem)
{
	size_t pos = input.find(dem);
	size_t beg = 0;
	while(pos != string::npos)
	{
		string w = input.substr(beg, pos-beg);
		if(w.size() > 0)
		{
			res.push_back(w);
		}
		beg = pos+dem.length();
		pos = input.find(dem, beg);
	}
	string w = input.substr(beg, pos-beg);
	if(w.size() > 0)
	{
		res.push_back(w);
	}
}

