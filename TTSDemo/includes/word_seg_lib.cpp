#include "afx.h"
#include "word_seg_lib.h"
#include "stdio.h"
#include <string.h>
#include "stdafx.h"
#include "Dictionary.h"
#include "MaxMatch.h"
#include "UTF8Util.h"
#include "MaxMatch_Engine.h"
#include "cppjieba/Jieba.hpp"

using namespace std;
std::map<std::string, std::string> map_pos;  // 用于 pos 转换的 map_list 
std::map<std::string, std::string> map_pos_set; // 韵律词模型中提取出来的 词性集列表 
const int MAX_LINE = 10000;

MaxMatch_Engine maxMatch_engine;
cppjieba::Jieba *g_jieba = NULL;


#if defined(_MSC_VER) && _MSC_VER > 1310
# include <Windows.h>
# define utf8(str) ConvertToUTF8(L ## str)
string ConvertToUTF8(const wchar_t* pStr) {
	static char szBuf[1024];
	WideCharToMultiByte(CP_UTF8, 0, pStr, -1, szBuf, sizeof(szBuf), NULL, NULL);
	return szBuf;
}

#else // if defined(_MSC_VER) && _MSC_VER > 1310
// Visual C++ 2003 and gcc will use the string literals as is, so the files
// should be saved as UTF-8. gcc requires the files to not have a UTF-8 BOM.
# define utf8(str) string(str)
#endif // if defined(_MSC_VER) && _MSC_VER > 1310

// const wchar_t * 转换成 string 或者 char * 
std::string CWTOA(const wchar_t* lpwcszWString)
{
	char* pElementText;//定义一个char类型指针
	int iTextLen;//定义长度

	iTextLen = ::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, NULL, 0, NULL, NULL);//获取传入字符串长度


	pElementText = new char[iTextLen + 1];//开辟空间
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));//清空

	// const wchar_t* 转换成  char * 
	::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);//转换
	// 对应的是 MultiByteToWideChar(__in UINT     CodePage,__in DWORD    dwFlags,__in_bcount(cbMultiByte) LPCSTR   lpMultiByteStr,__in int      cbMultiByte,__out_ecount_opt(cchWideChar) __transfer(lpMultiByteStr) LPWSTR  lpWideCharStr,__in int      cchWideChar);

	std::string strReturn(pElementText);
	
	delete[] pElementText;
	
	return strReturn;

}


/*
	后项最大匹配 中文分词 UTF8编码 
	将分词之后的“长词”重新拆分
	中华人民共和国/n ：中华人民/n 共和国/n
*/

int wordseg_so(const std::string &str_word, std::string &str_out, const std::string &str_pos)
{
	int ret = 0;	
	vector<string> vec_tmp;
	vector<string>::iterator it;

	vec_tmp = maxMatch_engine.seg_str2vec(str_word);

	for (it = vec_tmp.begin(); it != vec_tmp.end();it++)
	{
		str_out += *it + "/" + str_pos + " ";
	}
	

	return ret;
}


//注意：当字符串为空时，也会返回一个空字符串  
void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}

/*
	读取  Data/map_pos.txt 到 全局map: map_pos 中 
*/
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
		// line 需要删除末尾的 \r\n \n 

		// ude1	DE	u	的 底
		// 第一列 和 第三列 
		std::string str = line;
		int pos_1 = str.find('\t');
		if(pos_1 == NULL)
		{
			printf("map_pos error!\n");
			return -1;
		}

		// 得到词 word=ude1
		std::string word = str.substr(0, pos_1);

		int pos_2 = str.find('\t',pos_1+1); //DE	u	的 底
		str = str.substr(pos_2+1);//u	的 底
		int pos_end = str.find('\t');
		if(std::string::npos != pos_end)
		{
			str = str.substr(0,pos_end);
		}
		
		//printf("read line is:%s",line);
		//printf("pos:%s:pos:%s\n",word.c_str(), str.c_str());

		if(str[str.length()-1] == 0x0A)
		{
			str = str.substr(0,str.length()-1);
		}
		map_pos[word] = str;
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

/*
读取  Data/map_pos_set.txt 到 全局map: map_pos_set 中
*/
int load_map_pos_set(const char *file)
{
	FILE *fp = fopen(file, "r");
	if (fp == NULL)
	{
		printf("read %s failed!\n", file);
		return -1;
	}
	char line[1000] = "\0";
	while (fgets(line, 1000, fp))
	{

		std::string str = line;
		int pos_1 = str.find('\t');
		if (pos_1 == NULL)
		{
			printf("map_pos_set error!\n");
			return -1;
		}

		// 得到词 word=ude1
		std::string word = str.substr(0, pos_1);
		map_pos_set[word] = word;

	}

	if (fp != NULL){ fclose(fp); fp = NULL; }

	return 0;

}

int init_nlpir(const char *data_dir, const char *user_word_dict)
{
	int nCount;
	int ret = 0;
	
	// 初始化 jieba
	const char* const DICT_PATH = "data/jieba/jieba.dict.utf8";
	const char* const HMM_PATH = "data/jieba/hmm_model.utf8";
	const char* const USER_DICT_PATH = "data/jieba/user.dict.utf8";
	const char* const IDF_PATH = "data/jieba/idf.utf8";
	const char* const STOP_WORD_PATH = "data/jieba/stop_words.utf8";
	g_jieba = new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
	if (NULL == g_jieba)
	{
		return -1;
	}
	//cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);


	////初始化分词组件
	////数据在当前路径下，设置为UTF8编码的分词
	//if(!NLPIR_Init(data_dir))
	//{
	//	printf("ICTCLAS INIT FAILED!\n");
	//	return -2;
	//}
	//
	//nCount = NLPIR_ImportUserDict(user_word_dict);

	char map_pos_file[1000];
	_snprintf(map_pos_file, 1000, "%s/Data/map_pos.txt",data_dir);
	ret = load_map_pos(map_pos_file);
	if(ret <0)
	{
		printf("load_map_pos errer!\n");
		return -3;
	}

	char map_pos_file_set[1000];
	_snprintf(map_pos_file_set, 1000, "%s/Data/map_pos_set.txt", data_dir);
	ret = load_map_pos_set(map_pos_file_set);
	if (ret <0)
	{
		printf("load_map_pos_set errer!\n");
		return -4;
	}

	char dict_so[1000];
	_snprintf(dict_so, 1000, "%s/Data/dict_so.txt", data_dir);
	ret = maxMatch_engine.init(dict_so);

	return ret;
}

/*
// jieba 分词与词性标注  实际使用UTF8编码
// 长词处理  词性转换
*/
int SplitGBK(const char *sInput, char *sOutput, int size)
{

	const char * sResult;
	char dest[MAX_LINE] = { 0 };
	// 测试 split结果 
	std::vector<std::string> vec_spl_wp;
	std::vector<std::string> vec_wp;
	std::string delim = " ";
	std::string delim_2 = "//";
	std::vector<std::string>::iterator it;
	std::string str_out = "";

	// jieba
	vector<pair<string, string> > tagres;
	g_jieba->Tag(sInput, tagres);
	for (vector<pair<string, string> >::iterator it = tagres.begin(); it != tagres.end(); it++)
	{
		if (it->first.size() == 0 || it->first.compare(" ") == 0)
		{
			continue;
		}
		if (it != tagres.begin())
		{
			strcat(dest, " ");
		}
		
		strcat(dest, it->first.c_str());
		strcat(dest, "/");
		strcat(dest, it->second.c_str());
	}
	

	/*
	// 分词与词性标注 中科院 
	// 初始化函数中设定编码 UTF8 
	sResult = NLPIR_ParagraphProcess(sInput, 1);
	if (sResult == NULL)
	{
		printf("NLPIR_ParagraphProcess failed!\n");
		return -1;
	}

	_snprintf(dest, MAX_LINE, "%s", sResult);
	*/

	std::string str_tmp = dest;

	// 按照空格切分 
	split(str_tmp, delim, &vec_spl_wp);


	// 去掉不符合 word/pos 格式的  把相关pos转换 
	for (it = vec_spl_wp.begin(); it != vec_spl_wp.end(); it++)
	{
		//printf("test:%s\n", it->c_str());
		std::vector<std::string> vec_tmp;
		std::string str_tmp = *it;

		// 切分出 word 和 pos 
		split(str_tmp, delim_2, &vec_tmp);
		if (vec_tmp.size() < 2)
		{
			continue;
		}

		std::string str_word = vec_tmp[0]; // 分词 
		std::string str_pos = vec_tmp[1];  //  词性

		// 词性替换 
		std::map<std::string, std::string>::iterator it;
		it = map_pos.find(str_pos);
		// 标准词性集 list 
		std::map<std::string, std::string>::iterator it_set;
		it_set = map_pos_set.find(str_pos);

		// 在转换表里 并且 不在标准词性集里 
		if (it != map_pos.end() && it_set == map_pos_set.end())
		{
			str_pos = it->second;
		}

		// 词性序列检查
		it_set = map_pos_set.find(str_pos);
		// 最终还是不在list中的词性
		if (it_set == map_pos_set.end())
		{
			printf("error pos=%s\n", str_pos.c_str());
		}


		// 长词处理 UTF8编码 
		int nLen = str_word.size();
		// UTF8 3个byte代表一个字符 
		if (nLen>22)
			//if (nLen>12)
		{
			wordseg_so(str_word, str_out, str_pos);
		}
		else
		{
			str_out += str_word + "/" + str_pos + " ";
		}
		// split_long_word_4(str_word, str_out, str_pos);
		//split_long_word_2(str_word, str_out, str_pos);


	}

	// 拷贝 最终结果 
	_snprintf(sOutput, size, "%s", str_out.c_str());


	return 0;
}


/*
	将分词之后的“长词”重新拆分  GBK 
	中华人民共和国/n ：中华人民/n 共和国/n
*/
int split_long_word_4(const std::string &str_word, std::string &str_out, const std::string &str_pos)
{
	int ret = 0;
	

	// 长度 大于8的词  (9 10 11 12 ) 拆成 // 2+2+2+2+x    二万三千四百五十六元
	if (str_word.size()>16)
	{
		// 中华人民共和国宪法 : 中华人民  共和国宪  法 
		std::string str_word_1 = str_word.substr(0, 8); // 前4个字
		std::string str_word_2 = str_word.substr(8, 8); // 中间 4个字 
		std::string str_word_x = str_word.substr(16, str_word.size() - 16);

		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else if (str_word.size()>8) 
	{
		// 一万二千三百 : 一万二千 三百
		std::string str_word_1 = str_word.substr(0, 8);
		std::string str_word_x = str_word.substr(8, str_word.size() - 8);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else
	{
		// 最长容许4个字 ： 一万二千 
		str_out += str_word + "/" + str_pos + " ";

	}

	return ret;
}

/*
将分词之后的“长词”重新拆分  GBK 
中华人民共和国/n ：中华/n  人民/n 共和/n 国/n 
*/
int split_long_word_2(const std::string &str_word, std::string &str_out, const std::string &str_pos)
{
	int ret = 0;


	// 长度 大于8的词  (9 10 11 12 ) 拆成 // 2+2+2+2+x    二万三千四百五十六元
	if (str_word.size()>16) 
	{
		std::string str_word_1 = str_word.substr(0, 4);
		std::string str_word_2 = str_word.substr(4, 4);
		std::string str_word_3 = str_word.substr(8, 4);
		std::string str_word_4 = str_word.substr(12, 4);
		std::string str_word_x = str_word.substr(16, str_word.size()-16);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_3 + "/" + str_pos + " ";
		str_out += str_word_4 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else if (str_word.size()>12) //(7,8)  //2+2+2+x 
	{
		std::string str_word_1 = str_word.substr(0, 4);
		std::string str_word_2 = str_word.substr(4, 4);
		std::string str_word_3 = str_word.substr(8, 4);
		std::string str_word_x = str_word.substr(12, str_word.size()-12);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_3 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else if (str_word.size()>8) //(5,6)  //2+2+x 
	{
		int tmp_size = str_word.size();
		std::string str_word_1 = str_word.substr(0, 4);
		std::string str_word_2 = str_word.substr(4, 4);
		std::string str_word_x = str_word.substr(8, str_word.size()-8);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	//else if (str_word.size()>6) //(4) // 2+x
	//{
	//	std::string str_word_1 = str_word.substr(0, 4);
	//	std::string str_word_2 = str_word.substr(4, str_word.size()-4);
	//	str_out += str_word_1 + "/" + str_pos + " ";
	//	str_out += str_word_2 + "/" + str_pos + " ";
	//}
	else
	{
		str_out += str_word + "/" + str_pos + " ";
	
	}

	return ret;
}







//// 分词与词性标注
//// 长词处理  词性转换
//int SplitGBK(const char *sInput, char *sOutput, int size)
//{
//
//	const char * sResult;
//	char dest[MAX_LINE] = {0};
//
//	sResult = NLPIR_ParagraphProcess(sInput,1);
//	if(sResult == NULL)
//	{
//		printf("NLPIR_ParagraphProcess failed!\n");
//		return -1;
//	}
//
//	_snprintf(dest, MAX_LINE, "%s", sResult);
//	printf("SplitGBK read line:%s\n",dest);
//
//	char *pos = dest;
//	char *dest_st = sOutput;
//	while(*pos != '\0' && *pos != '\n')
//	{
//		if(*pos == '/')
//		{
//			char *pos_find_space = strchr(pos,' ');
//			if(pos_find_space == NULL)
//			{
//				printf("pos format err!\n");
//				return -1;
//			}
//
//			char temp_name[100] = {0};
//			strncpy(temp_name, pos, pos_find_space-pos);
//
//			//printf("....%s",temp_name);
//			std::map<std::string,std::string>::iterator it;
//			it = map_pos.find(std::string(temp_name));
//			if(it != map_pos.end())
//			{
//				//printf(":::%s",temp_name);
//				_snprintf(temp_name,100,"%s",it->second.c_str());
//				//printf("--->%s:::",temp_name);
//			}
//
//			_snprintf(dest_st, size-strlen(dest_st), "%s ", temp_name);
//			dest_st += strlen(temp_name)+1;
//			pos = pos_find_space + 1;
//
//
//		}
//		else
//		{
//			strncpy(dest_st, pos, 2);
//			pos++;
//			dest_st++;
//		}
//	}
//
//
//	return 0;
//
//}






int delete_nlpir()
{
	//NLPIR_Exit();
	if (NULL != g_jieba)
	{
		delete g_jieba;
		g_jieba = NULL;
	}
	return 0;
}
