#pragma once

#include <Windows.h>

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <conio.h>
#include <Windows.h>

using namespace std;

class  __declspec(dllexport) UTF8Util
{
public:
//	UTF8Util(void);
//	~UTF8Util(void);

	/**
	* Detect UTF8 BOM and skip it.
	*/
	static void SkipUtf8Bom(FILE* fp);

	/**
	* Return the length of the str
	*/
	static size_t StrCharLength(const char* str)
	{
		size_t length_count = 0;
		//size_t iter_i = 0;
		//size_t length = 0;

		const char* inbuff = str;
		const char* pbuff = FindNextInline(inbuff, '\t');
		//length = static_cast<size_t>(pbuff-inbuff);
		while(inbuff != pbuff)
		{
			//size_t charLength = UTF8Util.NextCharLength(inbuff);
			inbuff = NextChar(inbuff);
			length_count += 1;
		}
		return length_count;
	}

	static string NextNChars(const char* str, size_t length)
	{
		size_t length_count = 0;
		//size_t iter_i = 0;

		const char* tempStr = str;
		while(length_count < length)
		{
			length_count += 1;
			//cout<<"tempStr="<<tempStr<<endl;
			tempStr = NextChar(tempStr);
		}

		return FromSubstr(str, tempStr-str);
	}

	//
	static string PrevNChars(const char* str, size_t length)
	{
		size_t length_count = 0;
		
		const char* tempStr = str;
		while(length_count < length)
		{
			length_count += 1;
			tempStr = PrevChar(tempStr);
		}

		return tempStr;
	}

	/**
	* Return the length of the str
	*/
	static const char* SkipNChars(const char* str, size_t length)
	{
		size_t length_count = 0;

		const char* inbuff = str;
		const char* pbuff = FindNextInline(inbuff, '\t');
		while(inbuff!=pbuff && length_count<length)
		{
			inbuff = NextChar(inbuff);
			length_count += 1;

		}
		return inbuff;
	}

	/**
	* Returns the length in byte for the next UTF8 character.
	* On error returns 0.
	*/
	static size_t NextCharLengthNoException(const char* str) {
		char ch = *str;
		if ((ch & 0x80) == 0x00) {
			return 1;
		} else if ((ch & 0xE0) == 0xC0) {   // 110******
			return 2;
		} else if ((ch & 0xF0) == 0xE0) {
			return 3;
		} else if ((ch & 0xF8) == 0xF0) {
			return 4;
		} else if ((ch & 0xFC) == 0xF8) {
			return 5;
		} else if ((ch & 0xFE) == 0xFC) {
			return 6;
		}
		return 0;
	}
  
	/**
	* Returns the length in byte for the next UTF8 character.
	*/
	static size_t NextCharLength(const char* str) {
		size_t length = NextCharLengthNoException(str);
		if (length == 0) {
			//throw InvalidUTF8(str);
			//yqw 20150817 抛出异常，注释
		}
		return length;
	}

	/**
	* Returns the length in byte for the previous UTF8 character.
	*/
	static size_t PrevCharLength(const char* str) {
		for (size_t i = 1; i <= 6; i++) {
			str--;
			size_t length = NextCharLengthNoException(str);
			if (length == i) {
				return length;
			}
		}
		//throw InvalidUTF8(str);
		//yqw 20150817 抛出异常，注释
	}

	/**
	* Returns the char* pointer over the next UTF8 character.
	*/
	static const char* NextChar(const char* str) {
		return str + NextCharLength(str);
	}

	/**
	* Move the char* pointer before the previous UTF8 character.
	*/
	static const char* PrevChar(const char* str) {
		return str - PrevCharLength(str);
	}

	 /**
	 * Finds a character in the same line.
	* @param str The text to be searched in.
	* @param ch  The character to find.
	* @return    The pointer that points to the found chacter in str or EOL/EOF.
	*/
	static const char* FindNextInline(const char* str, const char ch) {
		while (!IsLineEndingOrFileEnding(*str) && *str != ch) {
			str = NextChar(str);
		}
		return str;
	}

	/**
	* Returns ture if the character is a line ending or end of file.
	*/
	static bool IsLineEndingOrFileEnding(const char ch) {
		return ch == '\0' || ch == '\n' || ch == '\r';
	}

	/**
	* Copies a substring with given length to a new std::string.
	*/
	static string FromSubstr(const char* str, size_t length) {
		string newStr;
		newStr.resize(length);
		strncpy(const_cast<char*>(newStr.c_str()), str, length);
		return newStr;
	}

	/**
	 *
	 */
	static string Substr(const char* str, size_t start, size_t end)
	{
		if(start < 0)
		{
			start = 0;
		}
		size_t length = StrCharLength(str);
		if(start >= length)
		{
			return "";
		}
		if(end > length)
		{
			end = length;
		}

		string newStr;
		size_t dstLength = SkipNChars(str, end)-SkipNChars(str, start);
		newStr.resize(dstLength);
		strncpy(const_cast<char*>(newStr.c_str()), SkipNChars(str, start), dstLength);
		return newStr;
	}

	/**
	* Returns true if the given string is longer or as long as the given length.
	*/
	static bool NotShorterThan(const char* str, size_t length) {
		while (length > 0) {
			if (*str == '\0') {
				return false;
			}
			length--;
			str++;
		}
		return true;
	}

	/**
	* Truncates a string with a maximal length.
	* No UTF8 character will be broken.
	*/
	static string TruncateUTF8(const char* str, size_t maxLength) {
		string wordTrunc;
		if (NotShorterThan(str, maxLength)) {
			size_t len = 0;
			const char* pStr = str;
			while (len < maxLength) {
				size_t nextLen = NextCharLength(pStr);
				pStr += nextLen;
				len += nextLen;
			}
			wordTrunc = FromSubstr(str, len);
		} else {
			wordTrunc = str;
		}
		return wordTrunc;
	}

	/**
	* Replaces all patterns in a string in place.
	*/
	static void ReplaceAll(string& str, const char* from, const char* to) {
		string::size_type pos = 0;
		string::size_type fromLen = strlen(from);
		string::size_type toLen = strlen(to);
		while ((pos = str.find(from, pos)) != string::npos) {
			str.replace(pos, fromLen, to);
			pos += toLen;
		}
	}

	/**
	 *
	 */
	static void ReplaceFirst(string& str, const char* from, const char* to)
	{
		string::size_type pos = 0;
		string::size_type fromLen = strlen(from);
		string::size_type toLen = strlen(to);
		if((pos = str.find(from, pos)) != string::npos)
		{
			str.replace(pos, fromLen, to);
			pos += toLen;
		}
	}

	/**
	* Joins a string vector in to a string with a separator.
	*/
	static string Join(const vector<string>& strings, const string& separator) {
		std::ostringstream buffer;
		bool first = true;
		//yqw 20150817
		for(int index=0; index<strings.size(); ++index)
		{
			string str = strings.at(index);
			if(!first)
			{
				buffer << separator;
			}
			buffer << str;
			first = false;
		}
		return buffer.str();

//		for (const &str : strings) {
//			if (!first) {
//				buffer << separator;
//			}
//			buffer << str;
//			first = false;
//		}
//		return buffer.str();
	}

	/**
	* Joins a string vector in to a string.
	*/
	static string Join(const vector<string>& strings) {
		std::ostringstream buffer;
		for(int index=0; index<strings.size(); ++index)
		{
			string str = strings.at(index);
			buffer << str;
		}
		return buffer.str();

//		for (const auto& str : strings) {
//			buffer << str;
//		}
//		return buffer.str();

	}

	static string UTF8ToGBK(const string& strUTF8) 
	{ 
		int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
		unsigned short * wszGBK = new unsigned short[len + 1]; memset(wszGBK, 0, len * 2 + 2); 
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

		len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL); 
		char *szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1); 
		WideCharToMultiByte(CP_ACP,0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL); //strUTF8 = szGBK; 
		std::string strTemp(szGBK); 
		delete[]szGBK; 
		delete[]wszGBK; 
		return strTemp; 
	}

};

