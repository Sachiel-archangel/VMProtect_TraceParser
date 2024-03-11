// vmp_traceparser.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include "VmpTraceParser.h"
#include "ParseArgs.h"

int wmain(int argc, wchar_t* argv[])
{
	ParseArgs* objParseArgs = NULL;
	VmpTraceParser* objVmpTraceParser = NULL;
	int intRet = 0;

	objParseArgs = new ParseArgs;
	// オプションの分解
	if (objParseArgs->Parse(argc, argv) < 0) {
		printf("vmp_traceparser\r\n\r\n");
		printf("Option:\r\n");
		printf("  -d:input file name (default:data.log)\r\n");
		printf("  -o:output file name (default:output.txt)\r\n");
		return 0;
	}

	printf("vmp_traceparser process start.\r\n");

	// 関数の実行
	intRet = objVmpTraceParser->ParseFunctionLog(objParseArgs->GetDataFileName(), objParseArgs->GetOutFileName());

	printf("vmp_traceparser process end. result code = %d\r\n", intRet);

	// 終了
	if (objParseArgs) {
		delete objParseArgs;
		objParseArgs = NULL;
	}

	if (objVmpTraceParser) {
		delete objVmpTraceParser;
		objVmpTraceParser = NULL;
	}

	return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
