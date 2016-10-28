#pragma once
#include "afx.h"

class CTTSDemoUtils : public CObject
{
	public:
		CTTSDemoUtils();
		~CTTSDemoUtils();
		static CString GetFileVersion(LPCTSTR filePath);
		static int ReadTextFile(CString filePath, CStringList& fileList);
		static CStringA UTF16_UTF8(const CStringW& utf16);
		static CStringW UTF8_UTF16(const CStringA& utf8);
};

