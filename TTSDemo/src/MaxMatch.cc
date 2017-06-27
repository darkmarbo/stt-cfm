//#include "StdAfx.h"
#include "stdafx.h"
#include "MaxMatch.h"


MaxMatch::MaxMatch(void)
{
}


MaxMatch::~MaxMatch(void)
{
}

int MaxMatch::init(const std::string path_dict)
{
	int ret = 0;
	ret = dictionary.loadDict(path_dict);
	return ret;
}

vector<Term> MaxMatch::ForwardMaximumMatching(const string& str)
{
	vector<Term> termVec;
	string segStr = str;
	unsigned int len = dictionary.getMaxLen();

	//cout << "str length = "<<length<<endl;

	//cout<<"maxlen="<<len<<endl;

	while(UTF8Util::StrCharLength(segStr.c_str()) > 0)
	{
		len = dictionary.getMaxLen();
		if(UTF8Util::StrCharLength(segStr.c_str()) < len)
		{
			len  = UTF8Util::StrCharLength(segStr.c_str());
		}
		string temp_word = UTF8Util::NextNChars(segStr.c_str(), len);
		bool match = false;

		//cout<<"len="<<len<<", temp_word="<<temp_word<<", len of segStr="<<UTF8Util::StrCharLength(segStr.c_str())<<endl;

		while(!match && len>1)
		{
			if(dictionary.FindWord(temp_word) != -1)
			{
				match = true;
				//cout<<"Here! Match"<<endl;
			}
			else
			{
				len -= 1;
				temp_word = UTF8Util::NextNChars(segStr.c_str(), len);
			}
		}

		Term term(temp_word);
		termVec.push_back(term);
		//cout<<"term=\t"<<temp_word<<endl;
		segStr = UTF8Util::SkipNChars(segStr.c_str(),len);

	}

	return termVec;
}

vector<Term> MaxMatch::BackwardMaximumMatching(const string& str)
{
	vector<Term> termVec;
	string segStr = str;
	unsigned int len = dictionary.getMaxLen();

	while(UTF8Util::StrCharLength(segStr.c_str()) > 0)
	{
		len = dictionary.getMaxLen();
		if(UTF8Util::StrCharLength(segStr.c_str()) < len)
		{
			len = UTF8Util::StrCharLength(segStr.c_str());
		}
		//string temp_word = segStr.substr(segStr.length()-len, len);
		string temp_word = UTF8Util::PrevNChars(segStr.c_str()+strlen(segStr.c_str()), len);
		bool match = false;

		while(!match && len>1)
		{
			if(dictionary.FindWord(temp_word) != -1)
			{
				match = true;
			}
			else
			{
				len -= 1;
				//temp_word = segStr.substr(segStr.length()-len, len);
				temp_word = UTF8Util::PrevNChars(segStr.c_str()+strlen(segStr.c_str()), len);
			}
		}

		Term term(temp_word);
		termVec.insert(termVec.begin(),term);
		//termVec.insert(termVec.begin(), term);
		//segStr = segStr.substr(0, segStr.length()-len);
		segStr = UTF8Util::NextNChars(segStr.c_str(), UTF8Util::StrCharLength(segStr.c_str())-len);

	}

	return termVec;
}
