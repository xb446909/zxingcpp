// zxingcpp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

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

	auto bitmap = writer.encode(TextUtfEncoding::FromUtf8(text), width, height).toByteMatrix();
	memcpy(buffer, bitmap.data(), width * height);
	return 0;
}

