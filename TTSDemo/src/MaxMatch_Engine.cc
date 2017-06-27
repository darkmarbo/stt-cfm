//#include "StdAfx.h"
#include "stdafx.h"
#include "MaxMatch_Engine.h"


MaxMatch_Engine::MaxMatch_Engine(void)
{
}


MaxMatch_Engine::~MaxMatch_Engine(void)
{
}

int MaxMatch_Engine::init(const std::string path_dict)
{
	int ret = 0;
	ret = this->maxMatch.init(path_dict);
	return ret;

}

vector<Term> MaxMatch_Engine::wordSegmentor(string sent)
{
	return maxMatch.BackwardMaximumMatching(sent);
}

vector<string> MaxMatch_Engine::seg_str2vec(string sent)
{
	
	vector<string> vec_tmp;
	vector<Term> termVec = wordSegmentor(sent);
	for (vector<Term>::iterator it = termVec.begin(); it != termVec.end(); ++it)
	{
		
		vec_tmp.push_back((*it).getText());
	}

	return vec_tmp;
	
}

string MaxMatch_Engine::handleSent(string sent)
{
	string seg_str = "";
	vector<Term> termVec = wordSegmentor(sent);
	for(vector<Term>::iterator it = termVec.begin(); it!=termVec.end(); ++it)
	{
		seg_str += (*it).getText()+ " ";
	}

	return StringUtil::trim(seg_str);
}

void MaxMatch_Engine::handleFile(string filePath, string outputPath)
{

	const int ENTRY_BUFF_SIZE = 40960;
	char buff[ENTRY_BUFF_SIZE];

	FILE* fp = fopen(filePath.c_str(), "r");
	UTF8Util::SkipUtf8Bom(fp);

	FILE* fpw = fopen(outputPath.c_str(), "w");

	size_t lineNum = 1;
    int ii_line = 0;
	while(fgets(buff, ENTRY_BUFF_SIZE, fp))
	{
		lineNum += 1;
        if(ii_line % 10000 ==0)
        {
		    cout << "line:\t" << ii_line << endl;
        }
        ii_line ++;

		const char* inbuff = buff;
		size_t length;

		const char* pbuff = UTF8Util::FindNextInline(inbuff, '\n');
		if(UTF8Util::IsLineEndingOrFileEnding(*pbuff))
		{}
		
		length = static_cast<size_t>(pbuff-inbuff);
		string key = UTF8Util::FromSubstr(inbuff, length);
	
		string seg_str = handleSent(key);

		//cout << seg_str << endl;
		fwrite(seg_str.c_str(), seg_str.length(), 1, fpw);
		fwrite("\n", 1, 1, fpw);

	}
	

	fclose(fp);
	fclose(fpw);

}

void MaxMatch_Engine::handleDir(string inputPath, string outputPath)
{

}

