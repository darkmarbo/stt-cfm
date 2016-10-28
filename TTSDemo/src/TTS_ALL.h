
#ifndef  __TTS_ALL_H_
#define  __TTS_ALL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "TN_Engine.h"
#include "UTF8Util.h"
#include "inter_lib.h"
#include "word_seg_lib.h"



#pragma comment(lib, "lib/ttsdll.lib")
#pragma comment(lib, "lib/tn_dll.lib")

const int MAX_LINE_SIZE = 10000;

inline int str_trim(char *c)
{	
    if(strlen(c)>0 && c[strlen(c)-1] == 0x0A)
	{		
        c[strlen(c)-1] = '\0';
    }
	if(strlen(c)>0 && c[strlen(c)-1] == 0x0D)
	{		
        c[strlen(c)-1] = '\0';
    }
	return 0;
}

class TTS_ALL
{

public:
	~TTS_ALL();

	int init(const char* model_dir);
	int tts(const char *input, short *buff, int buff_size);

private:
	TN_Engine m_tn_engine;
	UTF8Util m_utf8_tool;
	TTS m_tts;
	FILE *fp_log;

};




#endif