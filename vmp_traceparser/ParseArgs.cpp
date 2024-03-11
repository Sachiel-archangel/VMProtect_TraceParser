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
			// -h�̏ꍇ�F���^�[���l���}�C�i�X�Ƃ��āA�R�����g���o���悤�U������B
			if (wcscmp(argv[i], L"-h") == 0) {
				return -1;
			}

			// -d�̏ꍇ�F���̕�������f�[�^�t�@�C���Ƃ���t���O��ݒ�B
			if (wcscmp(argv[i], L"-d") == 0) {
				flag = DATAFILENAME;
				continue;
			}

			// -o�̏ꍇ�F���̕�������f�[�^�t�@�C���Ƃ���t���O��ݒ�B
			if (wcscmp(argv[i], L"-o") == 0) {
				flag = OUTFILENAME;
				continue;
			}

			// ���F�����܂ŗ����ꍇ�A�Q�Ƃ��Ă���f�[�^�� -h �Ȃǂ̃X�C�b�`�ł͂Ȃ��B

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

	// �f�[�^�t�@�C�������ݒ肳��Ă��Ȃ������ꍇ�A�f�t�H���g�Ŏ��s�t�H���_�� data.log ���Q�Ƃ���悤�p�����[�^�ݒ�B
	if (strDataFileName == NULL)
	{
		strDataFileName = new wchar_t[32];
		memset(strDataFileName, 0x00, 32 * 2);
		wcscpy_s(strDataFileName, 32, L"data.log");
	}

	// �o�̓t�@�C�������ݒ肳��Ă��Ȃ������ꍇ�A�f�t�H���g�Ŏ��s�t�H���_�� output.csv ���Q�Ƃ���悤�p�����[�^�ݒ�B
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
