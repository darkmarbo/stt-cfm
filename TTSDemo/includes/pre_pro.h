
#ifndef  __PRE_PRO_H_
#define  __PRE_PRO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include<io.h>

const int MAX_LINE_LEN = 1000;

int getDirs(std::string path, std::vector<std::string>& files);

class PreProClass
{
public:
	PreProClass();
	~PreProClass();

	std::string& replace_all(std::string &str, const std::string &old_value, const std::string &new_value);
	std::string& replace_all_distinct(std::string &str, const std::string &old_value, const std::string &new_value);

	/*
	按照 str_pro/map.txt 中配置的几级转换规则  将原始大文本(多行) 转转成句子。
	转换过程中，将字数小于10的小句子进行合并（不然会有韵律问题）。
	*/
	int pre_pro_long(const std::string str_line, std::vector<std::string> &vec_out, int min_count);

	/*
	按照 str_pro/map.txt 中配置的几级转换规则  将原始大文本(多行) 转转成句子。

	*/
	int pre_pro_small(const std::string str_line, std::vector<std::string> &vec_out);

	int init(const char *map_dir);

private:
	std::map<std::string, std::string> map_trans;

};

#endif





