#include "../stdafx.h"
#include "pre_pro.h"

int getDirs(std::string path, std::vector<std::string>& files)
{
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		// 循环每一个文件
		do
		{
			//如果是目录,加入	//如果不是,拉倒
			if ((fileinfo.attrib &  _A_SUBDIR)  && 
				strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
			{			
					files.push_back(fileinfo.name);								
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
	return 0;
}


PreProClass::PreProClass()
{}
PreProClass::~PreProClass()
{}

int PreProClass::init(const char *map_dir)
{
	char *tmp = NULL;
	char line[MAX_LINE_LEN];
	std::map<std::string, std::string>::iterator it_map;

	char map_file[MAX_LINE_LEN];
	_snprintf(map_file, MAX_LINE_LEN, "%s/map.txt", map_dir);
	FILE *fp_map = fopen(map_file, "r");
	if (fp_map == NULL)
	{
		printf("open file err!\n");
		return -1;
	}

	// load map file !
	int num_line = 0;
	while (fgets(line, MAX_LINE_LEN, fp_map))
	{
		num_line++;
		if (strlen(line) == 0)
		{
			continue;
		}
		if (line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\r')
		{
			line[strlen(line) - 1] = '\0';
			printf("delete last char=enter\n");
		}

		tmp = strtok(line, " ");
		while (tmp != NULL)
		{
			
			std::string str_key = tmp;
			std::string str_val;

			if (num_line == 1)
			{ 
				str_val = "\n"; 
			}
			else if (num_line == 2)
			{
				str_val = " ";
			}
			else if (num_line == 3)
			{
				str_val = "";
			}
			else
			{
				break;
			}

			it_map = this->map_trans.find(str_key);
			if (it_map == this->map_trans.end())
			{
				this->map_trans.insert(std::pair<std::string, std::string>(str_key, str_val));
				
			}
			tmp = strtok(NULL, " ");
		}

	}

	for (it_map = this->map_trans.begin(); it_map != this->map_trans.end(); it_map++)
	{
		printf("map:\t%s:\t%s\n",it_map->first.c_str(), it_map->second.c_str());
	}

	return 0;
}

/*
	按照 str_pro/map.txt 中配置的几级转换规则  将原始大文本(多行) 转转成句子。
	转换过程中，将字数小于 17 的小句子进行合并（不然会有韵律问题）。
*/
int PreProClass::pre_pro_long(const std::string str_line, std::vector<std::string> &vec_out, int min_count)
{
	
	char *tmp = NULL;
	char line_char[MAX_LINE_LEN];
	std::string line = str_line;
	std::string str_cat = "";
	std::map<std::string, std::string>::iterator it_map;

	if (line.size() == 0){return 0;}

	if (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r')
	{
		line[line.size() - 1] = '\0';
	}
	
	// map.txt 中 特殊符号替换 
	for (it_map = map_trans.begin(); it_map != map_trans.end(); it_map++)
	{
		line = replace_all(line, it_map->first, it_map->second);
	}

	// 去除掉多余的空格和tab
	line = replace_all(line, "\t", "");
	line = replace_all(line, "　　　　　", "");
	line = replace_all(line, "　　　　", "");
	line = replace_all(line, "　　　", "");
	line = replace_all(line, "　　", "");
	line = replace_all(line, "　", "");
	line = replace_all(line, "      ", " "); 
	line = replace_all(line, "     ", " ");
	line = replace_all(line, "    ", " ");
	line = replace_all(line, "   ", " ");
	line = replace_all(line, "  ", " ");


	_snprintf(line_char, MAX_LINE_LEN, "%s", line.c_str());
	tmp = strtok(line_char, "\n");
	while (tmp != NULL)
	{
		// tmp 是以\n分割后的每一行(小句子)
		// 添加判断：如果当前句子太小（UTF8 三个字符一个字 50个字符16个字）
		str_cat += tmp; // 把当前小句子 加到累计string后面 

		// 当前 已经攒够一句话了
		// 10个字 
		if (str_cat.size()>min_count)
		{
			vec_out.push_back(str_cat);
			str_cat = "";
		}
		else // 还不够...... 后面肯定还要添加 所以加上空格 
		{
			str_cat += " ";
		}
		
		tmp = strtok(NULL, "\n");
	}

	if (str_cat.size() > 1)
	{
		vec_out.push_back(str_cat);
	}

	return 0;
}

/*
	按照 str_pro/map.txt 中配置的几级转换规则  将原始大文本(多行) 转转成句子。

*/
int PreProClass::pre_pro_small(const std::string str_line, std::vector<std::string> &vec_out)
{

	char *tmp = NULL;
	char line_char[MAX_LINE_LEN];
	std::string line = str_line;
	std::string str_cat = "";
	std::map<std::string, std::string>::iterator it_map;

	if (line.size() == 0){ return 0; }

	if (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r')
	{
		line[line.size() - 1] = '\0';
	}

	// map.txt 中 特殊符号替换 
	for (it_map = map_trans.begin(); it_map != map_trans.end(); it_map++)
	{
		line = replace_all(line, it_map->first, it_map->second);
	}

	// 去除掉多余的空格和tab
	line = replace_all(line, "\t", "");
	line = replace_all(line, "　　　　　", "");
	line = replace_all(line, "　　　　", "");
	line = replace_all(line, "　　　", "");
	line = replace_all(line, "　　", "");
	line = replace_all(line, "　", "");
	line = replace_all(line, "      ", " ");
	line = replace_all(line, "     ", " ");
	line = replace_all(line, "    ", " ");
	line = replace_all(line, "   ", " ");
	line = replace_all(line, "  ", " ");


	_snprintf(line_char, MAX_LINE_LEN, "%s", line.c_str());
	tmp = strtok(line_char, "\n");
	while (tmp != NULL)
	{
		// tmp 是以\n分割后的每一行(小句子)
		// 添加判断：如果当前句子太小（UTF8 三个字符一个字 4个字 12个字符 ）
		str_cat += tmp; // 把当前小句子 加到累计string后面 

		// 当前 已经攒够一句话了
		// 10个字 
		if (str_cat.size()>12)
		{
			vec_out.push_back(str_cat);
			str_cat = "";
		}
		else // 还不够...... 后面肯定还要添加 所以加上空格 
		{
			str_cat += " ";
		}

		tmp = strtok(NULL, "\n");
	}

	if (str_cat.size() > 1)
	{
		vec_out.push_back(str_cat);
	}

	return 0;
}




std::string&   PreProClass::replace_all(std::string &str, const std::string &old_value, const std::string &new_value)
{
	while (true)
	{
		std::string::size_type   pos(0);
		if ((pos = str.find(old_value)) != std::string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}
	return   str;
}

std::string&   PreProClass::replace_all_distinct(std::string &str, const std::string &old_value, const std::string &new_value)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != std::string::npos)
		{

			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}
	return   str;
}