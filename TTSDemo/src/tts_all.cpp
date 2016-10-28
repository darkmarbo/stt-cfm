
#include "tts_all.h"
#include <stdio.h>


int TTS_ALL::init(const char *model_dir)
{
	int ret = 0;

	fp_log = fopen("word_seg.log","w");

	char *tn_dir = new char[1000];
	_snprintf(tn_dir, 999, "%s/tn", model_dir);
	m_tn_engine.Init(std::string(tn_dir));

	// 分词和词性标注 初始化
	char *uwd = new char[1000];
	_snprintf(uwd, 999, "%s/Data/user_word_dict.txt", model_dir); 
	ret = init_nlpir(model_dir, uwd);
	if(ret < 0)
	{
		printf("init_nlpir failed!\n");
		return -1;
	}
	
	ret = m_tts.init(model_dir);
	if(ret < 0)
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

	

    int ret;
	char * tline_seg = new char[MAX_LINE_SIZE];  
	char * str_cpy = new char[MAX_LINE_SIZE]; 
	_snprintf(str_cpy, MAX_LINE_SIZE, "%s", input);
	

	// TN 处理
	printf("read:%s\n", str_cpy);
	string out_str = m_tn_engine.ProcessText(str_cpy);
	printf("tn:%s\n", out_str.c_str());

	string str_gbk = m_utf8_tool.UTF8ToGBK(out_str);
	printf("gbk:%s\n", str_gbk.c_str());
	memset(str_cpy, 0, MAX_LINE_SIZE);
	_snprintf(str_cpy, MAX_LINE_SIZE, "%s", str_gbk.c_str());

	// 分词和词性标注		
	str_trim(str_cpy);		
	memset(tline_seg, 0, MAX_LINE_SIZE);
	SplitGBK(str_cpy, tline_seg, MAX_LINE_SIZE);			
	_snprintf(str_cpy, MAX_LINE_SIZE, "%s", tline_seg);
	printf("split:%s\n", str_gbk.c_str());
	fprintf(fp_log, "%s\n", str_cpy); 
	

	ret = m_tts.line2short_array(str_cpy, buff, buff_size);

	

	
	delete tline_seg;
	delete str_cpy;

	return ret;

    
}


TTS_ALL::~TTS_ALL()
{
	fclose(fp_log);
}