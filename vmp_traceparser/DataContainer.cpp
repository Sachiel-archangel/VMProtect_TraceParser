#include "DataContainer.h"
#include <strsafe.h>
#include <shlwapi.h>

DataContainer::DataContainer()
{
	pDataBuf = NULL;
	iDataSize = 0;
	iCurrentDataSize = 0;
}


DataContainer::~DataContainer()
{
	if (pDataBuf)
		DeleteDataObject();
}

// データオブジェクトのポインタを取得する。
void* DataContainer::GetDataPointer()
{
	return pDataBuf;
}


// データオブジェクトのサイズを取得する
int DataContainer::GetDataSize()
{
	return iDataSize;
}


// データオブジェクトの有効サイズを設定する
void DataContainer::SetCurrentDataSize(int iSize)
{
	iCurrentDataSize = iSize;
}

// データオブジェクトの有効サイズを取得する
int DataContainer::GetCurrentDataSize()
{
	return iCurrentDataSize;
}


// データオブジェクトの領域を確保する。
int DataContainer::CreateDataObject(int iSize)
{
	// 引数チェック
	if (iSize <= 0)
		return DATACONT_ER_FATAL;

	// オブジェクトが残っている場合は一度削除
	if (pDataBuf)
		DeleteDataObject();

	// 領域確保
	pDataBuf = VirtualAlloc(NULL, iSize, MEM_COMMIT, PAGE_READWRITE);
	if (pDataBuf == NULL)
		return DATACONT_ER_FATAL;

	iDataSize = iSize;

	ClearDataObject();

	return DATACONT_SUCCESS;
}


// データオブジェクトの領域を解放する。
int DataContainer::DeleteDataObject()
{
	if (!pDataBuf)
		return DATACONT_SUCCESS;

	ClearDataObject();

	bool bRetCode;

	bRetCode = VirtualFree(pDataBuf, 0, MEM_RELEASE);

	pDataBuf = NULL;
	iDataSize = 0;

	if (bRetCode == false)
		return DATACONT_ER_FATAL;

	return DATACONT_SUCCESS;
}


// データオブジェクトを0クリアする。
int DataContainer::ClearDataObject()
{
	if (pDataBuf)
		memset(pDataBuf, 0x00, iDataSize);

	iCurrentDataSize = 0;

	return DATACONT_SUCCESS;
}


// 直前のデータを残しつつ、領域を再確保する。
int DataContainer::ReallocDataObject(int iSize)
{
	void *pTmp;
	int iTmpSize;
	int iTmpCurrentSize;

	// 引数チェック
	if (iSize <= 0)
		return DATACONT_ER_FATAL;

	// 再確保サイズが小さい場合、不要なため行わない。
	if (iDataSize >= iSize)
		return DATACONT_SUCCESS;

	// 初期のデータ領域が無い場合、単純に領域を確保して終了
	if (pDataBuf == NULL) {
		return CreateDataObject(iSize);
	}

	// 領域のポインタを退避し、領域を再確保
	pTmp = pDataBuf;
	iTmpSize = iDataSize;
	iTmpCurrentSize = iCurrentDataSize;
	pDataBuf = NULL;
	if (CreateDataObject(iSize) < 0)
		return DATACONT_ER_FATAL;

	// 確保した領域に元データをコピー
	memcpy(pDataBuf, pTmp, iTmpSize);
	iCurrentDataSize = iTmpCurrentSize;

	// 旧領域を解放する。
	bool bRetCode;

	bRetCode = VirtualFree(pTmp, 0, MEM_RELEASE);

	if (bRetCode == false)
		return DATACONT_ER_FATAL;

	return DATACONT_SUCCESS;
}

// 外部のデータの取り込み
int DataContainer::ImportData(void* pData, int iSize)
{
	// 引数チェック
	if (iSize <= 0 || pData == NULL)
		return DATACONT_ER_FATAL;

	if (iSize > iDataSize)
	{
		if (CreateDataObject(iSize) != DATACONT_SUCCESS)
		{
			return DATACONT_ER_FATAL;
		}
	}

	memcpy(pDataBuf, pData, iSize);
	iCurrentDataSize = iSize;

	return DATACONT_SUCCESS;
}

// 文字列の取り込み
int DataContainer::ImportString(const TCHAR *pData, int iSize)
{
	int iStrLength = 0;
	int iStrSize = 0;

	// 引数チェック
	if (iSize <= 0 || pData == NULL)
		return DATACONT_ER_FATAL;

	// 文字列長の取得とバッファサイズの確認
	iStrLength = lstrlen(pData);
	iStrSize = iStrLength * (int)sizeof(TCHAR);
	if(iStrSize > iSize)
		return DATACONT_ER_FATAL;

	// 文字列格納領域確保(NULL止め分プラス)
	if (iStrSize + (int)sizeof(TCHAR) > iDataSize)
	{
		if (CreateDataObject(iStrSize + (int)sizeof(TCHAR)) != DATACONT_SUCCESS)
		{
			return DATACONT_ER_FATAL;
		}
	}

	// データのコピー
	memcpy(pDataBuf, pData, iStrSize);

	iCurrentDataSize = iStrSize;

	return DATACONT_SUCCESS;
}

int DataContainer::AppendString(const TCHAR *pData, int iSize)
{
	int iStrLength = 0;
	int iStrSize = 0;
	int iSrcLength = 0;
	int iSrcSize = 0;

	// 引数チェック
	if (iSize <= 0 || pData == NULL)
		return DATACONT_ER_FATAL;

	// 文字列長の取得とバッファサイズの確認
	iStrLength = lstrlen(pData);
	iStrSize = iStrLength * (int)sizeof(TCHAR);
	if (iStrSize > iSize)
		return DATACONT_ER_FATAL;

	// 現在の文字列のサイズを取得
	iSrcLength = lstrlen((TCHAR *)pDataBuf);
	iSrcSize = iSrcLength * (int)sizeof(TCHAR);

	// 領域が不足している場合、領域の再確保
	if (iSrcSize + iStrSize + (int)sizeof(TCHAR) > iDataSize)
	{
		if (ReallocDataObject(iSrcSize + iStrSize + (int)sizeof(TCHAR)) != DATACONT_SUCCESS)
		{
			return DATACONT_ER_FATAL;
		}
	}

	// 文字列の結合
	if(StringCchCat((TCHAR *)pDataBuf, iDataSize, pData) != S_OK)
		return DATACONT_ER_FATAL;

	// サイズの格納
	iCurrentDataSize = lstrlen((TCHAR *)pDataBuf) * (int)sizeof(TCHAR);

	return DATACONT_SUCCESS;
}

int DataContainer::AppendStringA(const LPSTR pData, int iSize)
{
	int iStrLength = 0;
	int iStrSize = 0;
	int iSrcLength = 0;
	int iSrcSize = 0;

	// 引数チェック
	if (iSize <= 0 || pData == NULL)
		return DATACONT_ER_FATAL;

	// 文字列長の取得とバッファサイズの確認
	iStrLength = lstrlenA(pData);
	iStrSize = iStrLength * (int)sizeof(CHAR);
	if (iStrSize > iSize)
		return DATACONT_ER_FATAL;

	// 現在の文字列のサイズを取得
	iSrcLength = lstrlenA((LPSTR)pDataBuf);
	iSrcSize = iSrcLength * (int)sizeof(CHAR);

	// 領域が不足している場合、領域の再確保
	if (iSrcSize + iStrSize + (int)sizeof(CHAR) > iDataSize)
	{
		if (ReallocDataObject(iSrcSize + iStrSize + (int)sizeof(CHAR)) != DATACONT_SUCCESS)
		{
			return DATACONT_ER_FATAL;
		}
	}

	// 文字列の結合
	if (StringCchCatA((LPSTR)pDataBuf, iDataSize, pData) != S_OK)
		return DATACONT_ER_FATAL;

	// サイズの格納
	iCurrentDataSize = lstrlenA((LPSTR)pDataBuf) * (int)sizeof(CHAR);

	return DATACONT_SUCCESS;
}

// データのファイル出力（同名のファイルが存在する場合上書き）
int DataContainer::OutputToFile(const TCHAR *pFileName)
{
	HANDLE hFile = NULL;
	DWORD dwWritten = 0;
	int iRetCode = DATACONT_ER_FATAL;

	// 引数チェック
	if (pFileName == NULL)
		return DATACONT_ER_FATAL;

	// ファイルを開く
	hFile = CreateFile(pFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return DATACONT_ER_FATAL;

	// ファイルに出力
	if (WriteFile(hFile, pDataBuf, iCurrentDataSize, &dwWritten, NULL) == TRUE)
	{
		iRetCode = DATACONT_SUCCESS;
	}
	
	CloseHandle(hFile);

	return iRetCode;
}

// ファイルからのデータ読み込み
int DataContainer::InputFromFile(const TCHAR *pFileName)
{
	HANDLE hFile = NULL;
	int iRetCode = DATACONT_ER_FATAL;
	DWORD dwRead = 0;
	LARGE_INTEGER objFileSize;

	// 引数チェック
	if (pFileName == NULL)
		return DATACONT_ER_FATAL;

	// ファイルを開く
	hFile = CreateFile(pFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return DATACONT_ER_FATAL;

	// サイズを取得
	if (GetFileSizeEx(hFile, &objFileSize) == false)
	{
		CloseHandle(hFile);
		return DATACONT_ER_FATAL;
	}

	// サイズチェック
	// 対応できないサイズはエラー
	if(objFileSize.HighPart > 0)
	{
		CloseHandle(hFile);
		return DATACONT_ER_FATAL;
	}

	if (objFileSize.LowPart > 0x7fffffff - sizeof(TCHAR))
	{
		CloseHandle(hFile);
		return DATACONT_ER_FATAL;
	}

	// ファイルデータの格納領域取得
	// テキストデータだった場合にNULL止め領域が必要なため、領域はその分追加して確保する。
	if(CreateDataObject((int)objFileSize.LowPart + sizeof(TCHAR)) != DATACONT_SUCCESS)
	{
		CloseHandle(hFile);
		return DATACONT_ER_FATAL;
	}

	if(ReadFile(hFile, pDataBuf, objFileSize.LowPart, &dwRead, NULL) == TRUE)
	{
		iRetCode = DATACONT_SUCCESS;
	}

	CloseHandle(hFile);

	return iRetCode;
}