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

// �f�[�^�I�u�W�F�N�g�̃|�C���^���擾����B
void* DataContainer::GetDataPointer()
{
	return pDataBuf;
}


// �f�[�^�I�u�W�F�N�g�̃T�C�Y���擾����
int DataContainer::GetDataSize()
{
	return iDataSize;
}


// �f�[�^�I�u�W�F�N�g�̗L���T�C�Y��ݒ肷��
void DataContainer::SetCurrentDataSize(int iSize)
{
	iCurrentDataSize = iSize;
}

// �f�[�^�I�u�W�F�N�g�̗L���T�C�Y���擾����
int DataContainer::GetCurrentDataSize()
{
	return iCurrentDataSize;
}


// �f�[�^�I�u�W�F�N�g�̗̈���m�ۂ���B
int DataContainer::CreateDataObject(int iSize)
{
	// �����`�F�b�N
	if (iSize <= 0)
		return DATACONT_ER_FATAL;

	// �I�u�W�F�N�g���c���Ă���ꍇ�͈�x�폜
	if (pDataBuf)
		DeleteDataObject();

	// �̈�m��
	pDataBuf = VirtualAlloc(NULL, iSize, MEM_COMMIT, PAGE_READWRITE);
	if (pDataBuf == NULL)
		return DATACONT_ER_FATAL;

	iDataSize = iSize;

	ClearDataObject();

	return DATACONT_SUCCESS;
}


// �f�[�^�I�u�W�F�N�g�̗̈���������B
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


// �f�[�^�I�u�W�F�N�g��0�N���A����B
int DataContainer::ClearDataObject()
{
	if (pDataBuf)
		memset(pDataBuf, 0x00, iDataSize);

	iCurrentDataSize = 0;

	return DATACONT_SUCCESS;
}


// ���O�̃f�[�^���c���A�̈���Ċm�ۂ���B
int DataContainer::ReallocDataObject(int iSize)
{
	void *pTmp;
	int iTmpSize;
	int iTmpCurrentSize;

	// �����`�F�b�N
	if (iSize <= 0)
		return DATACONT_ER_FATAL;

	// �Ċm�ۃT�C�Y���������ꍇ�A�s�v�Ȃ��ߍs��Ȃ��B
	if (iDataSize >= iSize)
		return DATACONT_SUCCESS;

	// �����̃f�[�^�̈悪�����ꍇ�A�P���ɗ̈���m�ۂ��ďI��
	if (pDataBuf == NULL) {
		return CreateDataObject(iSize);
	}

	// �̈�̃|�C���^��ޔ����A�̈���Ċm��
	pTmp = pDataBuf;
	iTmpSize = iDataSize;
	iTmpCurrentSize = iCurrentDataSize;
	pDataBuf = NULL;
	if (CreateDataObject(iSize) < 0)
		return DATACONT_ER_FATAL;

	// �m�ۂ����̈�Ɍ��f�[�^���R�s�[
	memcpy(pDataBuf, pTmp, iTmpSize);
	iCurrentDataSize = iTmpCurrentSize;

	// ���̈���������B
	bool bRetCode;

	bRetCode = VirtualFree(pTmp, 0, MEM_RELEASE);

	if (bRetCode == false)
		return DATACONT_ER_FATAL;

	return DATACONT_SUCCESS;
}

// �O���̃f�[�^�̎�荞��
int DataContainer::ImportData(void* pData, int iSize)
{
	// �����`�F�b�N
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

// ������̎�荞��
int DataContainer::ImportString(const TCHAR *pData, int iSize)
{
	int iStrLength = 0;
	int iStrSize = 0;

	// �����`�F�b�N
	if (iSize <= 0 || pData == NULL)
		return DATACONT_ER_FATAL;

	// �����񒷂̎擾�ƃo�b�t�@�T�C�Y�̊m�F
	iStrLength = lstrlen(pData);
	iStrSize = iStrLength * (int)sizeof(TCHAR);
	if(iStrSize > iSize)
		return DATACONT_ER_FATAL;

	// ������i�[�̈�m��(NULL�~�ߕ��v���X)
	if (iStrSize + (int)sizeof(TCHAR) > iDataSize)
	{
		if (CreateDataObject(iStrSize + (int)sizeof(TCHAR)) != DATACONT_SUCCESS)
		{
			return DATACONT_ER_FATAL;
		}
	}

	// �f�[�^�̃R�s�[
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

	// �����`�F�b�N
	if (iSize <= 0 || pData == NULL)
		return DATACONT_ER_FATAL;

	// �����񒷂̎擾�ƃo�b�t�@�T�C�Y�̊m�F
	iStrLength = lstrlen(pData);
	iStrSize = iStrLength * (int)sizeof(TCHAR);
	if (iStrSize > iSize)
		return DATACONT_ER_FATAL;

	// ���݂̕�����̃T�C�Y���擾
	iSrcLength = lstrlen((TCHAR *)pDataBuf);
	iSrcSize = iSrcLength * (int)sizeof(TCHAR);

	// �̈悪�s�����Ă���ꍇ�A�̈�̍Ċm��
	if (iSrcSize + iStrSize + (int)sizeof(TCHAR) > iDataSize)
	{
		if (ReallocDataObject(iSrcSize + iStrSize + (int)sizeof(TCHAR)) != DATACONT_SUCCESS)
		{
			return DATACONT_ER_FATAL;
		}
	}

	// ������̌���
	if(StringCchCat((TCHAR *)pDataBuf, iDataSize, pData) != S_OK)
		return DATACONT_ER_FATAL;

	// �T�C�Y�̊i�[
	iCurrentDataSize = lstrlen((TCHAR *)pDataBuf) * (int)sizeof(TCHAR);

	return DATACONT_SUCCESS;
}

int DataContainer::AppendStringA(const LPSTR pData, int iSize)
{
	int iStrLength = 0;
	int iStrSize = 0;
	int iSrcLength = 0;
	int iSrcSize = 0;

	// �����`�F�b�N
	if (iSize <= 0 || pData == NULL)
		return DATACONT_ER_FATAL;

	// �����񒷂̎擾�ƃo�b�t�@�T�C�Y�̊m�F
	iStrLength = lstrlenA(pData);
	iStrSize = iStrLength * (int)sizeof(CHAR);
	if (iStrSize > iSize)
		return DATACONT_ER_FATAL;

	// ���݂̕�����̃T�C�Y���擾
	iSrcLength = lstrlenA((LPSTR)pDataBuf);
	iSrcSize = iSrcLength * (int)sizeof(CHAR);

	// �̈悪�s�����Ă���ꍇ�A�̈�̍Ċm��
	if (iSrcSize + iStrSize + (int)sizeof(CHAR) > iDataSize)
	{
		if (ReallocDataObject(iSrcSize + iStrSize + (int)sizeof(CHAR)) != DATACONT_SUCCESS)
		{
			return DATACONT_ER_FATAL;
		}
	}

	// ������̌���
	if (StringCchCatA((LPSTR)pDataBuf, iDataSize, pData) != S_OK)
		return DATACONT_ER_FATAL;

	// �T�C�Y�̊i�[
	iCurrentDataSize = lstrlenA((LPSTR)pDataBuf) * (int)sizeof(CHAR);

	return DATACONT_SUCCESS;
}

// �f�[�^�̃t�@�C���o�́i�����̃t�@�C�������݂���ꍇ�㏑���j
int DataContainer::OutputToFile(const TCHAR *pFileName)
{
	HANDLE hFile = NULL;
	DWORD dwWritten = 0;
	int iRetCode = DATACONT_ER_FATAL;

	// �����`�F�b�N
	if (pFileName == NULL)
		return DATACONT_ER_FATAL;

	// �t�@�C�����J��
	hFile = CreateFile(pFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return DATACONT_ER_FATAL;

	// �t�@�C���ɏo��
	if (WriteFile(hFile, pDataBuf, iCurrentDataSize, &dwWritten, NULL) == TRUE)
	{
		iRetCode = DATACONT_SUCCESS;
	}
	
	CloseHandle(hFile);

	return iRetCode;
}

// �t�@�C������̃f�[�^�ǂݍ���
int DataContainer::InputFromFile(const TCHAR *pFileName)
{
	HANDLE hFile = NULL;
	int iRetCode = DATACONT_ER_FATAL;
	DWORD dwRead = 0;
	LARGE_INTEGER objFileSize;

	// �����`�F�b�N
	if (pFileName == NULL)
		return DATACONT_ER_FATAL;

	// �t�@�C�����J��
	hFile = CreateFile(pFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return DATACONT_ER_FATAL;

	// �T�C�Y���擾
	if (GetFileSizeEx(hFile, &objFileSize) == false)
	{
		CloseHandle(hFile);
		return DATACONT_ER_FATAL;
	}

	// �T�C�Y�`�F�b�N
	// �Ή��ł��Ȃ��T�C�Y�̓G���[
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

	// �t�@�C���f�[�^�̊i�[�̈�擾
	// �e�L�X�g�f�[�^�������ꍇ��NULL�~�ߗ̈悪�K�v�Ȃ��߁A�̈�͂��̕��ǉ����Ċm�ۂ���B
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