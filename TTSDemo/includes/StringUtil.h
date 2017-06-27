#pragma once

#include "stdafx.h"

class StringUtil
{
public:
	StringUtil(void);
	~StringUtil(void);

	static string& trim(string &);

	static void split(vector<string>& res, const string& input, const char dem = '\t');
	static void split(vector<string>& res, const string& input, const string& dem);

};

