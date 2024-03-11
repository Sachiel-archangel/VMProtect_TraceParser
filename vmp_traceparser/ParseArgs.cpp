#include "ParseArgs.h"
#include <string.h>
#include <stdlib.h>

#define DATAFILENAME 2
#define OUTFILENAME 3

ParseArgs::ParseArgs()
{
	strDataFileName = NULL;
	strOutFileName = NULL;
}

ParseArgs::~ParseArgs()
{
	if (strDataFileName != NULL)
	{
		delete strDataFileName;
		strDataFileName = NULL;
	}
	if (strOutFileName != NULL)
	{
		delete strOutFileName;
		strOutFileName = NULL;
	}
}

int ParseArgs::Parse(int argc, wchar_t* argv[])
{
	int iRet = 0;
	int i;
	int flag = 0;

	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			// -hの場合：リターン値をマイナスとして、コメントを出すよう誘導する。
			if (wcscmp(argv[i], L"-h") == 0) {
				return -1;
			}

			// -dの場合：次の文字列をデータファイルとするフラグを設定。
			if (wcscmp(argv[i], L"-d") == 0) {
				flag = DATAFILENAME;
				continue;
			}

			// -oの場合：次の文字列をデータファイルとするフラグを設定。
			if (wcscmp(argv[i], L"-o") == 0) {
				flag = OUTFILENAME;
				continue;
			}

			// 注：ここまで来た場合、参照しているデータは -h などのスイッチではない。

			if (flag == DATAFILENAME) {
				int tmp = (int)wcslen(argv[i]);
				strDataFileName = new wchar_t[(tmp + 1)];
				memset(strDataFileName, 0x00, (tmp + 1) * 2);
				wcscpy_s(strDataFileName, tmp + 1, argv[i]);

				iRet++;

				flag = 0;
				continue;
			}

			if (flag == OUTFILENAME) {
				int tmp = (int)wcslen(argv[i]);
				strOutFileName = new wchar_t[(tmp + 1)];
				memset(strOutFileName, 0x00, (tmp + 1) * 2);
				wcscpy_s(strOutFileName, tmp + 1, argv[i]);

				iRet++;

				flag = 0;
				continue;
			}
		}

	}

	// データファイル名が設定されていなかった場合、デフォルトで実行フォルダの data.log を参照するようパラメータ設定。
	if (strDataFileName == NULL)
	{
		strDataFileName = new wchar_t[32];
		memset(strDataFileName, 0x00, 32 * 2);
		wcscpy_s(strDataFileName, 32, L"data.log");
	}

	// 出力ファイル名が設定されていなかった場合、デフォルトで実行フォルダの output.csv を参照するようパラメータ設定。
	if (strOutFileName == NULL)
	{
		strOutFileName = new wchar_t[32];
		memset(strOutFileName, 0x00, 32 * 2);
		wcscpy_s(strOutFileName, 32, L"output.txt");
	}

	return iRet;
}

wchar_t* ParseArgs::GetDataFileName()
{
	return strDataFileName;
}

wchar_t* ParseArgs::GetOutFileName()
{
	return strOutFileName;
}
