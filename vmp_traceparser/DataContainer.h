#pragma once
#include <Windows.h>

#define DATACONT_SUCCESS 0
#define DATACONT_ER_FATAL -1


class DataContainer
{
private:
	void* pDataBuf;			// �o�b�t�@�|�C���^
	int iDataSize;			// Alloc�����̈�̃T�C�Y�i�[�p
	int iCurrentDataSize;	// �̈�ɓ����Ă�����f�[�^�T�C�Y�i�[�p

public:
	DataContainer();
	~DataContainer();

	// �f�[�^�I�u�W�F�N�g�̃|�C���^���擾����B
	void* GetDataPointer();

	// �f�[�^�I�u�W�F�N�g�̃T�C�Y���擾����
	int GetDataSize();

	// �f�[�^�I�u�W�F�N�g�̗L���T�C�Y��ݒ肷��
	void SetCurrentDataSize(int iSize);

	// �f�[�^�I�u�W�F�N�g�̗L���T�C�Y���擾����
	int GetCurrentDataSize();

	// �f�[�^�I�u�W�F�N�g�̗̈���m�ۂ���B
	int CreateDataObject(int iSize);

	// �f�[�^�I�u�W�F�N�g�̗̈���������B
	int DeleteDataObject();

	// �f�[�^�I�u�W�F�N�g��0�N���A����B
	int ClearDataObject();

	// ���O�̃f�[�^���c���A�̈���Ċm�ۂ���B
	int ReallocDataObject(int iSize);

	// �f�[�^�̎�荞��(memcpy����)
	int ImportData(void* pData, int iSize);

	// ������̎�荞��(strcpy����)
	int ImportString(const TCHAR *pData, int iSize);

	// �����񌋍�(strcat����)
	int AppendString(const TCHAR *pData, int iSize);
	int AppendStringA(const LPSTR pData, int iSize);

	// �f�[�^�̃t�@�C���o�́i�����̃t�@�C�������݂���ꍇ�㏑���j
	int OutputToFile(const TCHAR *pFileName);

	// �t�@�C������̃f�[�^�ǂݍ���
	int InputFromFile(const TCHAR *pFileName);
};

