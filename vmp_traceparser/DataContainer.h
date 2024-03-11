#pragma once
#include <Windows.h>

#define DATACONT_SUCCESS 0
#define DATACONT_ER_FATAL -1


class DataContainer
{
private:
	void* pDataBuf;			// バッファポインタ
	int iDataSize;			// Allocした領域のサイズ格納用
	int iCurrentDataSize;	// 領域に入っている実データサイズ格納用

public:
	DataContainer();
	~DataContainer();

	// データオブジェクトのポインタを取得する。
	void* GetDataPointer();

	// データオブジェクトのサイズを取得する
	int GetDataSize();

	// データオブジェクトの有効サイズを設定する
	void SetCurrentDataSize(int iSize);

	// データオブジェクトの有効サイズを取得する
	int GetCurrentDataSize();

	// データオブジェクトの領域を確保する。
	int CreateDataObject(int iSize);

	// データオブジェクトの領域を解放する。
	int DeleteDataObject();

	// データオブジェクトを0クリアする。
	int ClearDataObject();

	// 直前のデータを残しつつ、領域を再確保する。
	int ReallocDataObject(int iSize);

	// データの取り込み(memcpy相当)
	int ImportData(void* pData, int iSize);

	// 文字列の取り込み(strcpy相当)
	int ImportString(const TCHAR *pData, int iSize);

	// 文字列結合(strcat相当)
	int AppendString(const TCHAR *pData, int iSize);
	int AppendStringA(const LPSTR pData, int iSize);

	// データのファイル出力（同名のファイルが存在する場合上書き）
	int OutputToFile(const TCHAR *pFileName);

	// ファイルからのデータ読み込み
	int InputFromFile(const TCHAR *pFileName);
};

