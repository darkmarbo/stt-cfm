
#include "word_seg_lib.h"
#include "stdio.h"
#include <string.h>

using namespace std;

std::map<std::string, std::string> map_pos;
const int MAX_LINE = 10000;

int load_map_pos(const char *file)
{
	FILE *fp = fopen(file,"r");
	if(fp == NULL)
	{
		printf("read %s failed!\n",file);
		return -1;
	}
	char line[1000] = "\0";
	while(fgets(line, 1000, fp))
	{
		std::string str = line;
		int pos_1 = str.find('\t');
		if(pos_1 == NULL)
		{
			printf("map_pos error!\n");
			return -1;
		}
		std::string str_1 = str.substr(0, pos_1);

		int pos_2 = str.find('\t',pos_1+1);
		str = str.substr(pos_2+1);
		int pos_end = str.find('\t');
		if(std::string::npos != pos_end)
		{
			str = str.substr(0,pos_end);
		}
		
		//printf("read line is:%s",line);
		//printf("pos:%s:pos:%s\n",str_1.c_str(), str.c_str());

		if(str[str.length()-1] == 0x0A)
		{
			str = str.substr(0,str.length()-1);
		}
		map_pos["/"+str_1] = "/"+str;
	}

	std::map<std::string,std::string>::iterator it;
	printf("print map_pos!\n");
	for(it=map_pos.begin(); it != map_pos.end(); it++)
	{
		printf("###%s--->%s",it->first.c_str(), it->second.c_str());
	}
	printf("print map_pos over!\n");
	return 0;
}

int init_nlpir(const char *data_dir, const char *user_word_dict)
{
	int nCount;
	int ret;
	
	//初始化分词组件
	//数据在当前路径下，设置为UTF8编码的分词
	if(!NLPIR_Init(data_dir))
	{
		printf("ICTCLAS INIT FAILED!\n");
		return -1;
	}

	nCount = NLPIR_ImportUserDict(user_word_dict);

	char map_pos_file[1000];
	_snprintf(map_pos_file, 1000, "%s/Data/map_pos.txt",data_dir);
	ret = load_map_pos(map_pos_file);
	if(ret <0)
	{
		printf("load_map_pos errer!\n");
		return -1;
	}

	return ret;
}
int SplitGBK(const char *sInput, char *sOutput, int size)
{

	const char * sResult;
	char dest[MAX_LINE] = {0};

	sResult = NLPIR_ParagraphProcess(sInput,1);
	if(sResult == NULL)
	{
		printf("NLPIR_ParagraphProcess failed!\n");
		return -1;
	}

	_snprintf(dest, MAX_LINE, "%s", sResult);
	printf("SplitGBK read line:%s\n",dest);
	
	char *pos = dest;
	char *dest_st = sOutput;
	while(*pos != '\0' && *pos != '\n')
	{
		if(*pos == '/')
		{
			char *pos_find_space = strchr(pos,' ');
			if(pos_find_space == NULL)
			{
				printf("pos format err!\n");
				return -1;
			}

			char temp_name[100] = {0};
			strncpy(temp_name, pos, pos_find_space-pos);

			//printf("....%s",temp_name);
			std::map<std::string,std::string>::iterator it;
			it = map_pos.find(std::string(temp_name));
			if(it != map_pos.end())
			{
				//printf(":::%s",temp_name);
				_snprintf(temp_name,100,"%s",it->second.c_str());
				//printf("--->%s:::",temp_name);
			}

			_snprintf(dest_st, size-strlen(dest_st), "%s ", temp_name);
			dest_st += strlen(temp_name)+1;
			pos = pos_find_space + 1;
			
			
		}
		else
		{
			strncpy(dest_st, pos, 2);
			pos++;
			dest_st++;
		}
	}
	printf("\noutput line:%s\n", sOutput);
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	

	
	return 0;
}

int delete_nlpir()
{
	NLPIR_Exit();
	return 0;
}
