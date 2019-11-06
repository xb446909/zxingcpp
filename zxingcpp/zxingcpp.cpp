// zxingcpp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "TextDecoder.h"
#include "BarcodeFormat.h"
#include "MultiFormatWriter.h"
#include "BitMatrix.h"
#include "ByteMatrix.h"
#include "TextUtfEncoding.h"
#include "ZXStrConvWorkaround.h"

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

std::string AnsiToUtf8(const std::string &strAnsi)
{
	std::wstring strUnicode = AnsiToUnicode(strAnsi);
	return UnicodeToUTF8(strUnicode);
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