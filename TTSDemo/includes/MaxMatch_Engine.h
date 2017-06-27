#pragma once

#include "Term.h"
#include "MaxMatch.h"
#include "StringUtil.h"

class MaxMatch_Engine
{
	MaxMatch maxMatch;

public:
	MaxMatch_Engine(void);
	~MaxMatch_Engine(void);

	int init(const std::string path_dict); 

	vector<Term> wordSegmentor(string sent);

	/*
		新添加的 切分string到vector中 
	*/
	vector<string> seg_str2vec(string sent);

	string handleSent(string sent);

	void handleFile(string filePath, string outputPath);

	void handleDir(string inputPath, string outputPath);

};

