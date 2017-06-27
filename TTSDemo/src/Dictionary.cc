#include "stdafx.h"
#include <fstream>
#include <stdio.h>

#include "Dictionary.h"
#include "StringUtil.h"


Dictionary::Dictionary(void): maxLen(0)
{
	//this->coreDictPath = "resource/dict/core.dic.wordlist";
	cout << "Here, construct dictionary!"<< endl;
	//loadDict(this->coreDictPath);
}

Dictionary::Dictionary(string dictPath): maxLen(0)
{
	this->coreDictPath = dictPath;
	loadDict(this->coreDictPath);
}


Dictionary::~Dictionary(void)
{
}

int  Dictionary::loadDict(string dictPath)
{
	
	cout << "Here, load dict!"<< endl;
	cout << "coreDictPath:\t" << this->coreDictPath << endl;

	const int ENTRY_BUFF_SIZE = 4096;
	char buff[ENTRY_BUFF_SIZE];

	FILE* fp = fopen(dictPath.c_str(), "r");
	if (fp == NULL)
	{
		return -1;
	}
	UTF8Util::SkipUtf8Bom(fp);

	size_t lineNum = 1;
	while(fgets(buff, ENTRY_BUFF_SIZE, fp))
	{
		const char* inbuff = buff;
		size_t length;

		const char* pbuff = UTF8Util::FindNextInline(inbuff, '\t');
		if(UTF8Util::IsLineEndingOrFileEnding(*pbuff))
		{
		}

		
		length = static_cast<size_t>(pbuff-inbuff);
		string key = UTF8Util::FromSubstr(inbuff, length);
	
		coreDict.insert(make_pair(key, 1));
		size_t key_length = UTF8Util::StrCharLength(key.c_str());
		if(key_length > maxLen)
		{
			maxLen = key_length;
			//cout << key << " \tLen="<<key_length<<endl;
		}

		//cout << key << endl;
		lineNum += 1;

	}

    // todo 添加数字 
    for(int ii=0; ii<100001; ii++)
    {
        char tmp[1024] = {0};
        snprintf(tmp,1024,"%d",ii);
        string key = tmp; 
        coreDict.insert(make_pair(key, 1));
    }

	fclose(fp);

	return 0;
}

int Dictionary::FindWord(string word)
{
	if(coreDict.find(word) != coreDict.end())
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int Dictionary::getMaxLen()
{
	return maxLen;
}
