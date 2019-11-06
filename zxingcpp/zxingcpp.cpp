// zxingcpp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "BarcodeFormat.h"
#include "MultiFormatWriter.h"
#include "BitMatrix.h"
#include "ByteMatrix.h"
#include "TextUtfEncoding.h"
#include "ZXStrConvWorkaround.h"
#include "ReadBarcode.h"

#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>

using namespace ZXing;

static void PrintUsage(const char* exePath)
{
	std::cout << "Usage: " << exePath << " [-size <width>x<height>] [-margin <margin>] [-encoding <encoding>] [-ecc <level>] <format> <text> <output>\n"
		<< "    -size      Size of generated image\n"
		<< "    -margin    Margin around barcode\n"
		<< "    -encoding  Encoding used to encode input text\n"
		<< "    -ecc       Error correction level, [0-8]\n"
		<< "\n"
		<< "Supported formats are:\n"
		<< "    AZTEC\n"
		<< "    CODABAR\n"
		<< "    CODE_39\n"
		<< "    CODE_93\n"
		<< "    CODE_128\n"
		<< "    DATA_MATRIX\n"
		<< "    EAN_8\n"
		<< "    EAN_13\n"
		<< "    ITF\n"
		<< "    PDF_417\n"
		<< "    QR_CODE\n"
		<< "    UPC_A\n"
		<< "    UPC_E\n"
		<< "Formats can be lowercase letters, with or without underscore.\n";
}


static std::string FormatClean(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](char c) { return (char)std::tolower(c); });
	str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
	return str;
}

static std::string ParseFormat(std::string str)
{
	str = FormatClean(str);
	for (int i = 0; i < (int)BarcodeFormat::FORMAT_COUNT; ++i) {
		auto standardForm = ToString((BarcodeFormat)i);
		if (str == FormatClean(standardForm))
			return standardForm;
	}
	return std::string();
}




std::wstring AnsiToUnicode(const std::string &strAnsi)
{
	//获取转换所需的接收缓冲区大小
	int  nUnicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		strAnsi.c_str(),
		-1,
		NULL,
		0);
	//分配指定大小的内存
	wchar_t* pUnicode = new wchar_t[nUnicodeLen + 1];
	memset((void*)pUnicode, 0, (nUnicodeLen + 1) * sizeof(wchar_t));

	//转换
	::MultiByteToWideChar(CP_ACP,
		0,
		strAnsi.c_str(),
		-1,
		(LPWSTR)pUnicode,
		nUnicodeLen);

	std::wstring  strUnicode;
	strUnicode = (wchar_t*)pUnicode;
	delete[]pUnicode;

	return strUnicode;
}

std::string UnicodeToUTF8(const std::wstring& strUnicode)
{
	int nUtf8Length = WideCharToMultiByte(CP_UTF8,
		0,
		strUnicode.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);

	char* pUtf8 = new char[nUtf8Length + 1];
	memset((void*)pUtf8, 0, sizeof(char) * (nUtf8Length + 1));

	::WideCharToMultiByte(CP_UTF8,
		0,
		strUnicode.c_str(),
		-1,
		pUtf8,
		nUtf8Length,
		NULL,
		NULL);

	std::string strUtf8;
	strUtf8 = pUtf8;
	delete[] pUtf8;

	return strUtf8;
}

std::wstring UTF8ToUnicode(const std::string& str)
{
	int nUnicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);

	wchar_t*  pUnicode;
	pUnicode = new wchar_t[nUnicodeLen + 1];
	memset((void*)pUnicode, 0, (nUnicodeLen + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		nUnicodeLen);

	std::wstring  strUnicode;
	strUnicode = (wchar_t*)pUnicode;
	delete[]pUnicode;

	return strUnicode;
}


std::string UnicodeToANSI(const std::wstring& strUnicode)
{
	int nAnsiLen = WideCharToMultiByte(CP_ACP,
		0,
		strUnicode.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);

	char *pAnsi = new char[nAnsiLen + 1];
	memset((void*)pAnsi, 0, (nAnsiLen + 1) * sizeof(char));

	::WideCharToMultiByte(CP_ACP,
		0,
		strUnicode.c_str(),
		-1,
		pAnsi,
		nAnsiLen,
		NULL,
		NULL);

	std::string strAnsi;
	strAnsi = pAnsi;
	delete[]pAnsi;

	return strAnsi;
}

std::string AnsiToUtf8(const std::string &strAnsi)
{
	std::wstring strUnicode = AnsiToUnicode(strAnsi);
	return UnicodeToUTF8(strUnicode);
}

std::string Utf8ToAnsi(const std::string &strUtf8)
{
	std::wstring strUnicode = UTF8ToUnicode(strUtf8);
	return UnicodeToANSI(strUnicode);
}

int __stdcall GenerateBarcode(int width, int height, int margin, int eccLevel, const char* format, const char* text, unsigned char* buffer)
{
	auto barcodeFormat = BarcodeFormatFromString(format);
	if (barcodeFormat == BarcodeFormat::FORMAT_COUNT)
		return -1;
	MultiFormatWriter writer(barcodeFormat);
	if (margin >= 0)
		writer.setMargin(margin);
	if (eccLevel >= 0)
		writer.setEccLevel(eccLevel);
	writer.setEncoding(CharacterSet::UTF8);


	auto utf8_str = AnsiToUtf8(text);
	auto bitmap = writer.encode(TextUtfEncoding::FromUtf8(utf8_str), width, height).toByteMatrix();
	memcpy(buffer, bitmap.data(), width * height);

	return 0;
}

std::string WCharToMbs(std::wstring str)
{
	DWORD num = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, 0);

	char *pChar = NULL;
	pChar = (char*)malloc(num * sizeof(char));
	if (pChar == NULL)
	{
		free(pChar);
	}
	memset(pChar, 0, num * sizeof(char));

	WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, pChar, num, NULL, 0);
	std::string mbs(pChar);
	free(pChar);
	return mbs;
}

bool __stdcall ScanBarcode(int width, int height, unsigned char* buffer, char* text, bool tryRotate, bool fastMode)
{
	auto result = ReadBarcode(width, height, buffer, width * 3, 3, 0, 1, 2, { BarcodeFormatFromString("") }, tryRotate, !fastMode);
	if (result.isValid())
	{
		strcpy(text, WCharToMbs(result.text()).c_str());
	}
	return result.isValid();
}
