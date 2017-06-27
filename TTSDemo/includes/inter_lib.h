
#ifndef  __INTER_LIB_H_
#define  __INTER_LIB_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>


#ifdef  MYLIBDLL
#define MYLIBDLL extern "C" _declspec(dllimport)
#else
#define MYLIBDLL extern "C" _declspec(dllexport)
#endif

const int MAX_WAV_SIZE = 100000000;
const int MAX_PATH_SIZE = 1000;
const int LEN_SIL = 300;

struct   HTS_Engine;
struct STable;
struct Table ;


class __declspec(dllexport) TTS
{
public:
    TTS();
    ~TTS();

    int init(const char *model_dir);



    int line2short_array(const char *line, short *out, int out_size);
    int lines2short_array(const char *lines, short *out, int out_size);
	
	// 其他变量 
	int sampling_rate ;
	
	/* Error: output error message */
	void Error(const int error, char *message, ...);

	/* Getfp: wrapper for fopen */
	FILE *Getfp(const char *name, const char *opt);

	/* GetNumInterp: get number of speakers for interpolation from argv */
	int GetNumInterp(int argc, char **argv_search);

private:
    HTS_Engine *engine;
    STable *wt, *ct;
    Table *pw, *pp, *ip;

    FILE *fp_log ;

};





#endif  // __INTER_LIB_H_
