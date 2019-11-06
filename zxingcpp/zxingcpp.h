#pragma once

/*
Supported format:

AZTEC
CODABAR
CODE_39
CODE_93
CODE_128
DATA_MATRIX
EAN_8
EAN_13
ITF
MAXICODE
PDF_417
QR_CODE
RSS_14
RSS_EXPANDED
UPC_A
UPC_E
UPC_EAN_EXTENSION

*/

int __stdcall GenerateBarcode(int width, int height, int margin, int eccLevel, const char* format, const char* text, unsigned char* buffer);

//ÊäÈëÍ¼Ïñ±ØÐëÎª24bppRGBÍ¼Ïñ
bool __stdcall ScanBarcode(int width, int height, unsigned char* buffer, char* text, bool tryRotate, bool fastMode);