#ifndef  __WORD_SEG_LIB_H_
#define  __WORD_SEG_LIB_H_

#include "NLPIR.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef OS_LINUX
#pragma comment(lib, "lib/NLPIR.lib")
#else
#include<pthread.h>
#endif

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <time.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef OS_LINUX
#include <io.h>
#include <process.h>
#include <direct.h>
#include <assert.h>
#include <conio.h>
#define makeDirectory _mkdir
#pragma warning(disable:4786)
#define PATH_DELEMETER  "\\"
#define LONG64U __int64
#define DATE_DELEMETER  "/"
//#include <windows.h>
#define SLEEP(x) Sleep(x*1000)
#else
#include <dirent.h>
#include <ctype.h>
#include <unistd.h> 
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#define makeDirectory(x) mkdir(x, 0777)    // 这步骤必须，否则文件夹只读
//#define stricmp strcasecmp
//#define strnicmp strncasecmp
#define Sleep(x) sleep(x/1000)
#define min(a,b) (((a)<(b)) ? (a) : (b))
#define _stricmp(X,Y) strcasecmp((X),(Y))
#define stricmp(X,Y) strcasecmp((X),(Y))
#define strnicmp(X,Y,Z) strncasecmp((X),(Y),(Z))
#define _fstat(X,Y)     fstat((X),(Y))
#define _fileno(X)     fileno((X))
#define _stat           stat
#define _getcwd         getcwd
#define _off_t          off_t
#define PATH_DELEMETER  "/"
#define DATE_DELEMETER  "/"
#define LONG64U long long
#include <unistd.h>
#define SLEEP(x) sleep(x)
#endif




int load_map_pos(const char *file);
int init_nlpir(const char *data_dir, const char *user_word_dict);
int SplitGBK(const char *sInput, char *sOutput, int size);
int delete_nlpir();




#endif