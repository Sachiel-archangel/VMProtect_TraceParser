#include "VmpTraceParser.h"
#include <stdio.h>

#define READFILE 1
#define WRITEFILE 2

#define MAX_BUFFERSIZE 100 * 1024 * 1024

#define FLAG_HIT_TEXT 1
#define FLAG_HIT_VMP0 2
#define FLAG_GET_TEXT_CALLFUNC 4
#define FLAG_GET_APINAME 8


VmpTraceParser::VmpTraceParser()
{
}

VmpTraceParser::~VmpTraceParser()
{
}

HANDLE VmpTraceParser::OpenFile(wchar_t* strFileName, int flag)
{
	DWORD dwCreationDisposition = 0;
	DWORD dwDesiredAccess = 0;
	HANDLE hHandle = NULL;

	// ファイルモード設定
	if (flag == READFILE) {
		dwCreationDisposition = OPEN_EXISTING;
		dwDesiredAccess = GENERIC_READ;
	}
	else if (flag == WRITEFILE) {
		dwCreationDisposition = CREATE_ALWAYS;
		dwDesiredAccess = GENERIC_WRITE;
	}
	else
		return NULL;

	// ファイルのオープン
	hHandle = CreateFile(strFileName, dwDesiredAccess, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

	return hHandle;
}

int VmpTraceParser::ReadData(HANDLE hHandle, DataContainer* objBuf, DataContainer* objBefore)
{
	DWORD dwReadSize;
	unsigned char* pBuf = (unsigned char*)objBuf->GetDataPointer();
	int intSize = 0;
	intSize = MAX_BUFFERSIZE - objBefore->GetCurrentDataSize();

	// 前データのコピー
	objBuf->ImportData(objBefore->GetDataPointer(), objBefore->GetDataSize());
	pBuf = pBuf + objBefore->GetDataSize();

	// データの読み込み。
	// 読み込み先は前データのコピー先の末尾
	// 読み込みサイズは、バッファサイズから前データサイズ分を引く。
	if (!ReadFile(hHandle, pBuf, objBuf->GetDataSize(), &dwReadSize, NULL)) {
		return 0;
	}

	// 読み込み結果サイズが0
	if (dwReadSize == 0)
		return 0;

	objBuf->SetCurrentDataSize(dwReadSize);

	return 1;
}

int VmpTraceParser::WriteData(HANDLE hHandle, DataContainer* objBuf)
{
	DWORD dwWriteSize;

	if (!WriteFile(hHandle, objBuf->GetDataPointer(), objBuf->GetCurrentDataSize(), &dwWriteSize, NULL)) {
		return 0;
	}

	// 読み込み結果サイズが0
	if (dwWriteSize == 0)
		return 0;

	return 1;
}


int VmpTraceParser::SearchStringsOffset(DataContainer* objBufferAddr, int* intOffset, const char* strSearchStrings)
{
	unsigned char* pBuf;
	unsigned char cSearchHead = 0x00;
	int intRet = 0;

	// サーチ開始位置
	pBuf = (unsigned char*)objBufferAddr->GetDataPointer();
	
	// 先頭の1バイトでサーチする。
	// *高速化のために1バイトでサーチする。
	// *32ビット版なら4バイト、64ビット版なら8バイトでサーチするなどのほうが厳密には早いとは思う。
	// *ただし、サーチ対象文字列がそれらより短いこともあるので、そこまでの考慮とコーディングは今回は見送り。
	cSearchHead = strSearchStrings[0];

	for (;;) {

		// 先頭の文字のヒットチェック
		if (pBuf[*intOffset] == cSearchHead) {
			// 文字列チェックの前に、バッファオーバーランをチェック
			if (objBufferAddr->GetDataSize() - *intOffset < strlen(strSearchStrings))
				break;

			// 文字列で比較
			if (!memcmp((const char*)&pBuf[*intOffset], strSearchStrings, strlen(strSearchStrings))) {
				intRet = 1;
				break;
			}
		}

		// 次のアドレスへ（オフセットがバッファ範囲にあるかをチェックする）
		(*intOffset)++;
		if (objBufferAddr->GetCurrentDataSize() <= *intOffset)
			break;
	}

	return intRet;
}

int VmpTraceParser::SearchStringsInRow(DataContainer* objBufferAddr, int intOffset, const char* strSearchStrings)
{
	unsigned char* pBuf;
	unsigned char cSearchHead = 0x00;
	int intRet = 0;
	int i;

	// サーチ開始位置
	pBuf = (unsigned char*)objBufferAddr->GetDataPointer();

	// 先頭の1バイトでサーチする。
	// *高速化のために1バイトでサーチする。
	// *32ビット版なら4バイト、64ビット版なら8バイトでサーチするなどのほうが厳密には早いとは思う。
	// *ただし、サーチ対象文字列がそれらより短いこともあるので、そこまでの考慮とコーディングは今回は見送り。
	cSearchHead = strSearchStrings[0];

	i = intOffset;
	for (;;) {
		// 改行が見つかった場合、ノーヒット判定。
		if (pBuf[i] == '\n')
			return 0;

		// 先頭の文字のヒットチェック
		if (pBuf[i] == cSearchHead) {
			// 文字列チェックの前に、バッファオーバーランをチェック
			if (objBufferAddr->GetDataSize() - i < strlen(strSearchStrings))
				break;

			// 文字列で比較
			if (!memcmp((const char*)&pBuf[i], strSearchStrings, strlen(strSearchStrings))) {
				intRet = 1;
				break;
			}
		}

		// 次のアドレスへ（オフセットがバッファ範囲にあるかをチェックする）
		i++;
		if (objBufferAddr->GetCurrentDataSize() <= i)
			break;
	}

	return intRet;
}

int VmpTraceParser::HexCharCheck(unsigned char cChar)
{
	// １６進数のキャラクター（0～9、A～Fまたはa～f）かどうかをチェック
	if (cChar >= '0' && cChar <= '9')
		return 1;
	if (cChar >= 'A' && cChar <= 'F')
		return 1;
	if (cChar >= 'a' && cChar <= 'f')
		return 1;

	return 0;
}

long long VmpTraceParser::HexCharToParam(unsigned char cChar)
{
	if (cChar >= '0' && cChar <= '9')
		return cChar - '0';

	if (cChar == 'A' || cChar == 'a')
		return 10;

	if (cChar == 'B' || cChar == 'b')
		return 11;

	if (cChar == 'C' || cChar == 'c')
		return 12;

	if (cChar == 'D' || cChar == 'd')
		return 13;

	if (cChar == 'E' || cChar == 'e')
		return 14;

	if (cChar == 'F' || cChar == 'f')
		return 15;

	return 0;
}

long long VmpTraceParser::HexToParam(DataContainer* objData, int *intOffset)
{
	int i;
	unsigned char* pstrAddress = (unsigned char*)objData->GetDataPointer();
	long long llRet = 0;

	for (i = *intOffset; i< objData->GetCurrentDataSize(); i++) {
		if (HexCharCheck(pstrAddress[i])) {
			llRet = llRet << 4;
			llRet += HexCharToParam(pstrAddress[i]);
		}
		else {
			break;
		}
	}

	*intOffset = i;

	return llRet;
}

int VmpTraceParser::CalAddress(DataContainer* objData)
{
	int intRet = 0;
	int intSize = objData->GetDataSize();
	int i;
	unsigned char* pstrAddress = (unsigned char*)objData->GetDataPointer();

	for (i = 0; i < intSize; i++) {
		if (pstrAddress[i] == '+') {
			int j, k, intHead;
			DataContainer objCopy;
			unsigned char* pstrCopy = NULL;
			unsigned long long llParam = 0;

			// 「+」よりも前の16進値の文字列の先頭オフセットを探す。
			objCopy.ImportData(objData->GetDataPointer(), objData->GetDataSize());
			pstrCopy = (unsigned char*)objCopy.GetDataPointer();
			for (j = i - 1; j >= 0; j--) {
				if (!HexCharCheck(pstrCopy[j])) {
					j++;
					break;
				}
			}
			intHead = j;

			// 16進数文字列をバイナリ値に変換する。
			llParam = HexToParam(&objCopy, &j);

			// 「+」よりも後の16進値の文字列の先頭オフセットを探す
			for (; j <= objCopy.GetCurrentDataSize(); j++) {
				if (HexCharCheck(pstrCopy[j])) {
					break;
				}
			}

			// 「+」よりも後の16進値の文字列を加算する。
			llParam += HexToParam(&objCopy, &j);

			// 文字列の前半をコピー
			objData->ClearDataObject();
			for (k = 0; k < intHead; k++) {
				pstrAddress[k] = pstrCopy[k];
			}

			// 計算後のアドレスを出力
			sprintf_s((char*)&pstrAddress[k], objData->GetDataSize(), "%llX", llParam);

			// アドレスより後ろをコピーする。
			objData->AppendStringA((LPSTR) &pstrCopy[j], objCopy.GetCurrentDataSize() - j + 1);

			// オフセットの調整
			i = j;
		}
	}

	return intRet;
}

// IDAの Function Log を分析する処理。
int VmpTraceParser::ParseFunctionLog(wchar_t* strDataFileName, wchar_t* strOutFileName)
{
	int intErrorFlg = 0;
	int intOffset = 0;
	HANDLE hDataFile = NULL;
	HANDLE hOutFile = NULL;
	DataContainer* objBufferAddr = 0;
	DataContainer* objBeforeData = 0;
	DataContainer* objResultData = 0;
	LARGE_INTEGER objFileSize;
	unsigned int intFlag = 0;

	// ファイルを開く
	hDataFile = this->OpenFile(strDataFileName, READFILE);
	hOutFile = this->OpenFile(strOutFileName, WRITEFILE);

	if (hDataFile == (HANDLE)HFILE_ERROR || hOutFile == (HANDLE)HFILE_ERROR) {
		if (hDataFile != (HANDLE)HFILE_ERROR)
			CloseHandle(hDataFile);
		if (hOutFile != (HANDLE)HFILE_ERROR)
			CloseHandle(hOutFile);

		return VMPTRACEPARSER_FILEOPENERROR;
	}

	// 読み込みサイズを取得
	if (GetFileSizeEx(hDataFile, &objFileSize) == false)
	{
		CloseHandle(hDataFile);
		return VMPTRACEPARSER_FATALERROR;
	}

	// 領域の確保
	objBufferAddr = new DataContainer;
	objBeforeData = new DataContainer;
	objResultData = new DataContainer;
	objResultData->CreateDataObject(1024 * 1024);

	if (objBufferAddr == NULL || objBeforeData == NULL || objResultData == NULL) {
		if (objBufferAddr != NULL)
			delete (hDataFile);
		if (objBeforeData != NULL)
			delete(objBeforeData);
		if (objResultData != NULL)
			delete(objResultData);

		return VMPTRACEPARSER_ALLOCATEERROR;
	}

	if (objFileSize.HighPart == 0 && objFileSize.LowPart < MAX_BUFFERSIZE) {
		objBufferAddr->CreateDataObject(objFileSize.LowPart);
	}
	else {
		objBufferAddr->CreateDataObject(MAX_BUFFERSIZE);
	}

	// データのパース
	for (;;)
	{
		// データの読み込み
		if (!ReadData(hDataFile, objBufferAddr, objBeforeData))
			break;

		objBeforeData->DeleteDataObject();
		intOffset = 0;

		// 読み込んだデータの判定
		for (;;)
		{
			// 1: セグメントが.vmp0の行に切り替わる行を探す。
			// 1.1: .textセグメントのキーワードを探す。
			if ((intFlag & FLAG_HIT_TEXT) == 0) {
				if (!SearchStringsOffset(objBufferAddr, &intOffset, ".text")) {
					// ヒットせずに末尾まで行った場合、残すべきデータをobjBeforeDataに格納しつつBreak;
					objBeforeData->CreateDataObject(objBufferAddr->GetCurrentDataSize() - intOffset);
					objBeforeData->ImportData((unsigned char*)objBufferAddr->GetDataPointer() + intOffset, objBufferAddr->GetCurrentDataSize() - intOffset);
					objBeforeData->SetCurrentDataSize(objBufferAddr->GetCurrentDataSize() - intOffset);

					break;
				}
			}
			intFlag |= FLAG_HIT_TEXT;

			// 1.2: .vmp0セグメントのキーワードを探す。
			// とりあえず、これにヒットしたときに直前が.textセグメントであることのチェックまではこの段階では考慮しない。

			if ((intFlag & FLAG_HIT_VMP0) == 0) {

				// .vmp0セグメントのキーワードを探す。
				if (!SearchStringsOffset(objBufferAddr, &intOffset, ".vmp0")) {
					// ヒットせずに末尾まで行った場合、残すべきデータをobjBeforeDataに格納しつつBreak;
					// ここの場合、直後に読み込んだ先頭のデータでこの条件にヒットした場合、直前の行のデータを参照することになるが、
					// それを失わないよう1行分の直前データも保持する必要があることに留意。

					// まず、1行前のデータの先頭位置を探す。
					int intPrevHead = 0;
					unsigned char* pPrevHead = (unsigned char*)objBufferAddr->GetDataPointer();

					// 直前の行の先頭に到達するには、2回\nにヒットするはず。
					// (いちいち\r\nの判定まではしないものとする。)
					//   まず、.vmp0が見つかった行の先頭を探す。
					for (intPrevHead = intOffset; intPrevHead >= 0; intPrevHead--) {
						if (pPrevHead[intPrevHead] == '\n')
							break;
					}

					//   さらに前の行の先頭を探す。
					for (; intPrevHead >= 0; intPrevHead--) {
						if (pPrevHead[intPrevHead] == '\n')
							break;
					}
					intPrevHead++;

					objBeforeData->CreateDataObject(objBufferAddr->GetCurrentDataSize() - intPrevHead);
					objBeforeData->ImportData((unsigned char*)objBufferAddr->GetDataPointer() + intPrevHead, objBufferAddr->GetCurrentDataSize() - intPrevHead);
					objBeforeData->SetCurrentDataSize(objBufferAddr->GetCurrentDataSize() - intPrevHead);

					break;
				}

				// ヒットした行が.textセグメントの場合、探しなおす。

			}
			intFlag |= FLAG_HIT_VMP0;


			// 2: 見つかった.vmp0の行の直前の.textの行から、call関数の情報を抽出する
			if ((intFlag & FLAG_GET_TEXT_CALLFUNC) == 0) {
				int intPrevHead = 0;
				unsigned char* pPrevHead = (unsigned char*)objBufferAddr->GetDataPointer();
				int intTextFlg = 0;

				// 直前の行の先頭に到達するには、2回\nにヒットするはず。
				// (いちいち\r\nの判定まではしないものとする。)
				for (intPrevHead = intOffset; intPrevHead >= 0; intPrevHead--) {
					if (pPrevHead[intPrevHead] == '\n')
						break;
				}
				// .vmp0が見つかった行に、.textがあるかどうかをチェックする。
				// 行全体ではなく、.vmp0の見つかった行の.vmp0文字列がみつかった場所より前のデータで十分なので、それで処理。
				// 以下のようなケースはハズレのため、弾くための処理。
				// 「.text:00007FF60AD0AE10  	call    near ptr qword_7FF60ADFA9C0+1DEh	call .vmp0:qword_7FF60ADFA9C0+1DE」
				DataContainer objRowData;
				int intHead = intPrevHead + 1;
				int intDummy = 0;
				objRowData.CreateDataObject(intOffset - intPrevHead + 1);
				memcpy(objRowData.GetDataPointer(), &pPrevHead[intHead], intOffset - intPrevHead);
				objRowData.SetCurrentDataSize(intOffset - intPrevHead);
				if(SearchStringsOffset(&objRowData, &intDummy, ".text")) {
					intOffset++;
					continue;
				}



				intPrevHead--;

				for (; intPrevHead >= 0; intPrevHead--) {
					if (pPrevHead[intPrevHead] == '\n')
						break;
				}
				intPrevHead++;

				// その行の間に".text"が見つかるかをチェック
				for (; intPrevHead < intOffset - 4; intPrevHead++) {
					if (!memcmp((const char*) & pPrevHead[intPrevHead], ".text", strlen(".text"))) {
						intTextFlg = 1;
						break;
					}
				}

				// この時に.vmp0の直前が.textセグメントでない場合、エラーとして次のレコードを探す。
				if (!intTextFlg) {
					intFlag = 0;
					continue;
				}

				// ヒットした位置から1行分のデータをコピー
				int i;
				for (i = intPrevHead; i < objBufferAddr->GetCurrentDataSize() - 1; i++) {
					if (pPrevHead[i] == '\n') {
						break;
					}
				}
				i++;

				// 一行のデータを結果に出力する。
				DataContainer objImportData;
				objImportData.CreateDataObject(i - intPrevHead + 1);
				memcpy(objImportData.GetDataPointer(), &pPrevHead[intPrevHead], i - intPrevHead);
				objImportData.SetCurrentDataSize(i - intPrevHead + 1);

				// データ内で、アドレスが「+」付きになっている箇所を計算する。
				// 例：.text:sub_7FF60AC9BCE8+2D
				// →.text:sub_7FF60AC9BD15
				CalAddress(&objImportData);

//				objResultData->AppendString((const TCHAR*)objImportData.GetDataPointer(), objImportData.GetDataSize());
				objResultData->AppendStringA((LPSTR)objImportData.GetDataPointer(), objImportData.GetCurrentDataSize());
				objImportData.DeleteDataObject();

			}

			intFlag |= FLAG_GET_TEXT_CALLFUNC;


			// 3: .vmp0から別のセグメントに切り替わる最後の行を探し、APIの名称等を抽出する。
			// 3.1: .vmp0セグメントのキーワードが無くなる行を探す。
			// 3.2: .vmp0セグメントのキーワードが無くなる行の前の行を抽出することで最後の行を得る。
			// 3.3: 見つかった行からcallしているAPIの名称等を抽出する。
			if ((intFlag & FLAG_GET_APINAME) == 0) {
				unsigned char* pBuf = (unsigned char*)objBufferAddr->GetDataPointer();
				DataContainer objRowData;
				int flag;

				objRowData.CreateDataObject(128);

				for (;;) {
					int i;
					int intOffsetBackup;

					intOffsetBackup = intOffset;
					// 1行分のデータを抽出
					for (i = intOffset; i < objBufferAddr->GetCurrentDataSize() - 1; i++) {
						if (pBuf[i] == '\n') {
							break;
						}
					}
					i++;

					objRowData.ClearDataObject();
					objRowData.ImportData(&pBuf[intOffset], i - intOffset);
					intOffset = i;

					// 次の行が".vmp0"ではないことをチェック
					flag = 0;
					for (i = intOffset; i < objBufferAddr->GetCurrentDataSize() - 4; i++) {
						if (!memcmp((const char *) & pBuf[i], ".vmp0", strlen(".vmp0"))) {
							flag |= 1;
						}
						if (pBuf[i] == '\n') {
							flag |= 2;
							break;
						}
					}

					// チェック中にデータの終端が来た場合、必要データを保持して次のデータ読み込みに誘導
					if ((flag & 2) == 0) {
						objRowData.DeleteDataObject();
						objBeforeData->CreateDataObject(objBufferAddr->GetCurrentDataSize() - intOffsetBackup);
						objBeforeData->ImportData((unsigned char*)objBufferAddr->GetDataPointer() + intOffsetBackup, objBufferAddr->GetCurrentDataSize() - intOffsetBackup);
						objBeforeData->SetCurrentDataSize(objBufferAddr->GetCurrentDataSize() - intOffsetBackup);

						break;
					}

					// .vmp0だった場合、条件に合致しないため次の行へ
					if (flag == 3)
						continue;

					// ここまできた場合、次の行は.vmp0ではないかつ行の末尾（\n）が見つかっている。
					// (このとき、intOffsetは次の行の先頭アドレスになっているはず。このため、前の行のデータはobjRowDataを使用。)

					// まず、末尾から最初にみつかる「:」を探す。
					unsigned char* pRowBuf = (unsigned char*)objRowData.GetDataPointer();
					for (i = objRowData.GetCurrentDataSize() - 1; i >= 0; i--) {
						if (pRowBuf[i] == ':') {
							i++;
							break;
						}
					}

					DataContainer objImportData;
//					objResultData->AppendString((const TCHAR*)"\t", 2);
					objResultData->AppendStringA((LPSTR)"\t", 2);
					objImportData.CreateDataObject(objRowData.GetCurrentDataSize() -i + 1);
					memcpy(objImportData.GetDataPointer(), &pRowBuf[i], objRowData.GetCurrentDataSize() - i);

//					objResultData->AppendString((const TCHAR*)objImportData.GetDataPointer(), objImportData.GetDataSize());
					objResultData->AppendStringA((LPSTR)objImportData.GetDataPointer(), objImportData.GetDataSize());
					objImportData.DeleteDataObject();

					break;
				}

				// 次のデータの読み込みへ行く場合のブレーク
				if ((flag & 2) == 0) {
					break;
				}

			}

			intFlag |= FLAG_GET_APINAME;



			// フラグリセットし、次のデータのサーチ。
			intFlag = 0;
		}

		// 末尾判定
		// 末尾ならパース処理終了
		if (objBufferAddr->GetCurrentDataSize() < MAX_BUFFERSIZE)
			break;

		// 末尾ではない場合、残りデータをバッファにコピーし、さらにデータを読み込む。
		objBufferAddr->ClearDataObject();
	}

	// データをファイルに出力
	WriteData(hOutFile, objResultData);


	// オブジェクトの破棄
	if (objBufferAddr != NULL)
		delete (objBufferAddr);
	if (objBeforeData != NULL)
		delete(objBeforeData);
	if (objResultData != NULL)
		delete(objResultData);

	// ファイルを閉じる
	CloseHandle(hDataFile);
	CloseHandle(hOutFile);

	return intErrorFlg;
}
