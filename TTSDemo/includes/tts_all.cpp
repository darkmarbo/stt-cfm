#include "afx.h"
#include "tts_all.h"
#include <stdio.h>

/*
处理分词后的字符串： // 分别/d 达/v 二万三千一百七十/m 元/q
://分别/d 达/v 二万/m 三千/m 一百/m 七十/m 元/q
*/
int split_long_word()
{
	return 0;
}

/*
	传入 model\LC\model-like-77  直接就是声学模型
	只有ttsdll用到了此 目录
	其他 tn str_pro 分词 都写死在data里面了 
*/
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
	_snprintf(tn_dir, 999, "data/str_pro");
	ret = this->m_pp.init(tn_dir);
	if (ret < 0){ return -1; }

	// tn 初始化

	_snprintf(tn_dir, 999, "data/tn");
	ret = m_tn_engine.Init(std::string(tn_dir));
	if (ret < 0){ return -1; }

	// 分词和词性标注 初始化
	char *uwd = new char[1000];
	_snprintf(uwd, 999, "data/Data/user_word_dict.txt");
	ret = init_nlpir("data", uwd);
	if (ret < 0)
	{
		printf("init_nlpir failed!\n");
		return -1;
	}
	this->nlpir_flag = 1; // 没有NLPIR 控制jieba的  

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

/*
	// 处理首尾静音 找到第一个value值大于min_val的点  然后往左移动400ms(0.4*44100) 
	//buff: buff数据  长度为len
	// left:左侧端点的idx   right:右侧端点的idx  
	// min_val: 判定静音的阈值 400 
*/
int TTS_ALL::silence_pro(const short *buff, int len, int &left, int &right, short min_val)
{
	int ret = 0;
	left = 0;
	right = 0;
	short thr = 400;
	int pos = 4 * 4410; // 400ms

	for (int i = 0; i < len;i++)
	{
		if (abs(buff[i]) > thr)
		{
			left = i - pos;
			left = (left > 0 ? left : 0);
			break;
		}
	}
	for (int j = len - 1; j > 0;j--)
	{
		if (abs(buff[j]) > thr)
		{
			right = j + pos;
			right = (right<len ? right : len-1);
			break;
		}
	}

	return ret;
}

/*
	// 对输入的UTF-8文本，依次执行：TN,断句，分词、词性、转换GBK 
	// 对单句语音 进行首尾静音处理
	// input: 输入文本
	// buff_size: buff的最大长度 
*/
int TTS_ALL::tts(const char *input, short *buff, int buff_size)
{

	int ret = 0;
	int len = 0;
	int len_write = 0;  // 写入buff的总长度
	char *delim = "\n";
	
	char *tmp;
	char lines[MAX_LINE_SIZE];
	char input_new[MAX_LINE_SIZE];
	char * input_new_tmp = input_new;
	short *buff_tmp = new short[WAV_SIZE];
	

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

		// 字符串处理  断句！ 内部将短句子合并成长句子 。
		// long 将短句子合并  small 正常处理
		std::vector<std::string> vec_out;

		// 使用small的时候  短句子  kehu-165会出现“短句子的开头部分丢失”！
		//ret = this->m_pp.pre_pro_small(str_2, vec_out);
		
		ret = this->m_pp.pre_pro_long(str_2, vec_out, 3); // 50正常 
		//if (ret < 0){ return -1; }

		// 每一个字符串 不包含其他字符
		std::vector<std::string >::iterator it_vec;
		for (it_vec = vec_out.begin(); it_vec != vec_out.end(); it_vec++)
		{
			
			str_1 = *it_vec;
			fprintf(fp_log, "pre_pro:%s\n", str_1.c_str());
		
			// 直接使用 UTF8 
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_1.c_str());
			str_trim(line_split_tmp1);

			// add  处理分词和词性标注后  特别长的词 一百九十六 亿 美元。中华人民共和国  。 
			SplitGBK(line_split_tmp1, line_split_tmp2, MAX_LINE_SIZE);
			fprintf(fp_log, "split=%s\n", line_split_tmp2);
			std::string str_utf8 = line_split_tmp2;

			// UTF8 转成 GBK
			string str_gbk = m_utf8_tool.UTF8ToGBK(str_utf8);
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_gbk.c_str());

			
			//"新华网/nt 北京/ns 十二月/t 二十七日/t 电/n   记者/n 邹伟/nr "
			len = m_tts.line2short_array(line_split_tmp1, buff_tmp, WAV_SIZE);
			if (len < 0)
			{ 
				fprintf(fp_log, "line2short_array failed !: %s\n", line_split_tmp1);
				return len; 
			}

			// 对 buff_tmp（长度为 ret） 进行首尾静音处理
			int left = 0;
			int right = len - 1;
			silence_pro(buff_tmp, len, left, right, 400);
			len = right - left;  // 多少个short 

			if (buff_size < len_write+len)
			{
				fprintf(fp_log, "buff_len to small! str= %s\n", line_split_tmp1);
				return len_write;
			}

			// copy 处理后的buff 到输出结果中
			int sss = sizeof(short);
			memcpy(buff + len_write, buff_tmp + left, len*sss);
			
			len_write += len;

			fflush(fp_log);
		}

	}


	if (buff_tmp){ delete buff_tmp; buff_tmp = NULL; }

	fclose(fp_test_out);

	return len_write;

}
/*
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

	FILE *fp_test_out = fopen("temp.txt", "w");
	if (fp_test_out == NULL){ return -1; }
	fprintf(fp_test_out, "%s", input);
	fclose(fp_test_out);
	fp_test_out = fopen("temp.txt", "r");



	while ((tmp = fgets(lines, MAX_LINE_SIZE, fp_test_out)) != NULL)
	{
		str_1 = lines;

		// TN 处理
		fprintf(fp_log, "=split_str1:%s\n", str_1.c_str());
		str_2 = m_tn_engine.ProcessText(str_1);
		fprintf(fp_log, "tn:%s\n", str_2.c_str());

		// 字符串处理  断句！ 内部将短句子合并成长句子 。
		// long 将短句子合并  small 正常处理
		std::vector<std::string> vec_out;

		// 使用small的时候  短句子  kehu-165会出现“短句子的开头部分丢失”！
		//ret = this->m_pp.pre_pro_small(str_2, vec_out);

		ret = this->m_pp.pre_pro_long(str_2, vec_out, 50); // 17个字 
		//if (ret < 0){ return -1; }

		// 每一个字符串 不包含其他字符
		std::vector<std::string >::iterator it_vec;
		for (it_vec = vec_out.begin(); it_vec != vec_out.end(); it_vec++)
		{

			str_1 = *it_vec;
			fprintf(fp_log, "pre_pro:%s\n", str_1.c_str());

			// 直接使用 UTF8 
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_1.c_str());
			str_trim(line_split_tmp1);

			// add  处理分词和词性标注后  特别长的词 一百九十六 亿 美元。中华人民共和国  。 
			SplitGBK(line_split_tmp1, line_split_tmp2, MAX_LINE_SIZE);
			fprintf(fp_log, "split=%s\n", line_split_tmp2);
			std::string str_utf8 = line_split_tmp2;

			// UTF8 转成 GBK
			string str_gbk = m_utf8_tool.UTF8ToGBK(str_utf8);
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_gbk.c_str());


			//"新华网/nt 北京/ns 十二月/t 二十七日/t 电/n   记者/n 邹伟/nr "
			ret = m_tts.line2short_array(line_split_tmp1, buff_tmp, buff_size - len_write);
			if (ret < 0)
			{
				fprintf(fp_log, "line2short_array failed !: %s\n", line_split_tmp1);
				return ret;
			}
			buff_tmp += ret;
			len_write += ret;

			fflush(fp_log);
		}

	}



	fclose(fp_test_out);

	return len_write;

}
*/

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


