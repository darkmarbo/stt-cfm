#include "afx.h"
#include "tts_all.h"
#include <stdio.h>


int TTS_ALL::init(const char *model_dir)
{
	int ret = 0;
	char *tn_dir = new char[1000];

	if (fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}
	fp_log = fopen("word_seg.log", "w+");

	// 字符串 预处理
	_snprintf(tn_dir, 999, "%s/str_pro", model_dir);
	ret = this->m_pp.init(tn_dir);
	if (ret < 0){ return -1; }

	// tn 初始化

	_snprintf(tn_dir, 999, "%s/tn", model_dir);
	ret = m_tn_engine.Init(std::string(tn_dir));
	if (ret < 0){ return -1; }

	// 分词和词性标注 初始化
	char *uwd = new char[1000];
	_snprintf(uwd, 999, "%s/Data/user_word_dict.txt", model_dir);
	ret = init_nlpir(model_dir, uwd);
	if (ret < 0)
	{
		printf("init_nlpir failed!\n");
		return -1;
	}
	this->nlpir_flag = 1;

	// tts 初始化
	ret = m_tts.init(model_dir);
	if (ret < 0)
	{
		printf("tts init failed!\n");
		return -1;
	}

	delete uwd;
	delete tn_dir;
	return 0;
}

int TTS_ALL::tts(const char *input, short *buff, int buff_size)
{

	int ret = 0;
	int len = 0;
	int len_write = 0;  // 写入buff的总长度
	char *delim = "\n";
	short *buff_tmp = buff;
	char *tmp;
	char lines[MAX_LINE_SIZE];
	char input_new[MAX_LINE_SIZE];
	char * input_new_tmp = input_new;

	char line_split_tmp1[MAX_LINE_SIZE];
	char line_split_tmp2[MAX_LINE_SIZE];
	std::string str_1;
	std::string str_2;
	fprintf(fp_log, "input_char*:%s\n", input);

	FILE *fp_test_out = fopen("temp.txt","w");
	if (fp_test_out == NULL){ return -1; }
	fprintf(fp_test_out, "%s", input);
	fclose(fp_test_out);
	fp_test_out = fopen("temp.txt", "r");
	

	
	while ( (tmp = fgets(lines, MAX_LINE_SIZE,fp_test_out)) != NULL )
	{
		str_1 = lines;

		// TN 处理
		fprintf(fp_log, "=split_str1:%s\n", str_1.c_str());
		str_2 = m_tn_engine.ProcessText(str_1);
		fprintf(fp_log, "tn:%s\n", str_2.c_str());

		// 字符串处理！
		std::vector<std::string> vec_out;
		ret = this->m_pp.pre_pro(str_2, vec_out);
		//if (ret < 0){ return -1; }
		std::vector<std::string >::iterator it_vec;
		for (it_vec = vec_out.begin(); it_vec != vec_out.end(); it_vec++)
		{
			// 每一个字符串 不包含其他字符
			str_1 = *it_vec;
			fprintf(fp_log, "pre_pro:%s\n", str_1.c_str());


			// 分词和词性标注  NLPIR 
			string str_gbk = m_utf8_tool.UTF8ToGBK(str_1);

			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_gbk.c_str());
			str_trim(line_split_tmp1);

			SplitGBK(line_split_tmp1, line_split_tmp2, MAX_LINE_SIZE);
			fprintf(fp_log, "split=%s\n", line_split_tmp2);

			//"新华网/nt 北京/ns 十二月/t 二十七日/t 电/n   记者/n 邹伟/nr "
			ret = m_tts.line2short_array(line_split_tmp2, buff_tmp, buff_size - len_write);
			if (ret < 0){ return ret; }
			buff_tmp += ret;
			len_write += ret;

			//fflush(fp_log);
		}

	}



	fclose(fp_test_out);

	return len_write;

}

TTS_ALL::TTS_ALL()
{

	this->fp_log = NULL;
	this->nlpir_flag = 0;
}

TTS_ALL::~TTS_ALL()
{
	if (fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}

	if (this->nlpir_flag == 1)
	{
		delete_nlpir();
		this->nlpir_flag = 0;
	}
}


