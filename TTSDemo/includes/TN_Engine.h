#pragma once

//#include "TextNorm.h"
//#include "BasicNSWs.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class  __declspec(dllexport) TN_Engine
{
	string text;
	string rule_standard_str;
	string rule_norm_str;

	//BasicNSWs basicNSWs;

	//TextNorm textNorm;

public:
	TN_Engine(void);
	~TN_Engine(void);

	static void setLang(string lang);
	static string getLang();
	static void setModel_Dir(string model_dir);
	static string getModel_Dir();

	static int Init(string model_dir);
	string ParseText(string text);
	string NormText(string text);

	static string lang;	//zh, en, etc.  see lang_list.txt
	static string model_dir;

	static int load_conf_map();

	static map<string, vector<string>> conf_map;

	void ProcessFile(string file_path, string output_path);

	static string ProcessText(string text);

};


