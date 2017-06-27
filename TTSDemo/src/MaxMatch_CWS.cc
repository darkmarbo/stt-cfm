#include "stdafx.h"
#include "Dictionary.h"
#include "MaxMatch.h"
#include "UTF8Util.h"
#include "MaxMatch_Engine.h"


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


//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[])
{

    if (argc < 3)
    {
        printf("usage: %s input_file out_file\n",argv[0]);
        return 0;
    }

	MaxMatch_Engine maxMatch_engine;

	string input_path = argv[1];
	string output_path = argv[2];

	cout << "Begin ~" + input_path << endl;

	maxMatch_engine.handleFile(input_path, output_path);

	cout << "Complete!" + output_path << endl;

	return 0;

}

