#include "stdafx.h"
#include "TTSDemoUtils.h"

#pragma comment(lib,"Version.lib")

#define MAX_LINE_LENGTH 4096

CTTSDemoUtils::CTTSDemoUtils()
{
}


CTTSDemoUtils::~CTTSDemoUtils()
{
}

CString CTTSDemoUtils::GetFileVersion(LPCTSTR filePath)
{
	CString ret(_T(""));
	DWORD dummy;
	DWORD dwSize = GetFileVersionInfoSize(filePath, &dummy);
	if (dwSize == 0) ret = _T("Error retriving version(Module Not Found)");
	else
	{
		BYTE* data = new BYTE[dwSize];
		if (!GetFileVersionInfo(filePath, NULL, dwSize, &data[0]))
			ret = _T("Error retriving version(Version Data Not Retrived)");
		else
		{
			UINT infoLen = 0;
			VS_FIXEDFILEINFO * pFixedInfo = NULL;
			if (!VerQueryValue(&data[0], _T("\\"), (LPVOID *)&pFixedInfo, &infoLen)) ret = _T("Error retriving version(Version Data Query Failed)");
			else
			{
				ret.Format(_T("%u.%u.%u(%u)"),
					HIWORD(pFixedInfo->dwFileVersionMS),
					LOWORD(pFixedInfo->dwFileVersionMS),
					HIWORD(pFixedInfo->dwFileVersionLS),
					LOWORD(pFixedInfo->dwFileVersionLS));
			}
		}
		delete[] data;
	}
	return ret;
}

int CTTSDemoUtils::ReadTextFile(CString filePath, CStringList& lines)
{
	lines.RemoveAll();

	FILE * fptr;
	char line[MAX_LINE_LENGTH];

	//fptr = fopen("C:\\Files\\SVN\\SVN_SpeechOcean\\TTSDemo\\TTSDemo\\sample\\test.txt", "r, ccs=UTF-8");
	errno_t e = _tfopen_s(&fptr, filePath, _T("rb, ccs=UTF-8"));
	if (fptr)
	{
		while (fgets(line, MAX_LINE_LENGTH, fptr))
		{
			CStringA lineA(line);
			CStringW lineW = UTF8_UTF16(lineA);
			lines.AddTail(lineW);
		}
		fclose(fptr);
		fptr = NULL;
	}

	return lines.GetCount();
}


CStringA CTTSDemoUtils::UTF16_UTF8(const CStringW& utf16)
{
	if (utf16.IsEmpty()) return "";
	CStringA utf8;
	int cc = 0;

	if ((cc = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, 0, 0) - 1) > 0)
	{
		char * buf = utf8.GetBuffer(cc);
		if (buf) WideCharToMultiByte(CP_UTF8, 0, utf16, -1, buf, cc, 0, 0);
		utf8.ReleaseBuffer();
	}
	return utf8;
}

CStringW CTTSDemoUtils::UTF8_UTF16(const CStringA& utf8)
{
	if (utf8.IsEmpty()) return L"";
	CStringW utf16;
	int cc = 0;
	
	if ((cc = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0) - 1) > 0)
	{
		wchar_t *buf = utf16.GetBuffer(cc);
		if (buf) MultiByteToWideChar(CP_UTF8, 0, utf8, -1, buf, cc);
		utf16.ReleaseBuffer();
	}

	return utf16;
}
