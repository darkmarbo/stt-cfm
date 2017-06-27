#pragma once

#include "stdafx.h"
#include <map>
#include "UTF8Util.h"

class Dictionary
{

	string coreDictPath; //

	map<string, int> coreDict;

	unsigned int maxLen;

public:
	Dictionary(void);
	Dictionary(string dictPath);
	~Dictionary(void);

	int  loadDict(string dictPath);

	int FindWord(string word);

	int getMaxLen();

};

