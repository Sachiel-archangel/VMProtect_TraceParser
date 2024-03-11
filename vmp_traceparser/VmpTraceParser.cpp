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

	// �t�@�C�����[�h�ݒ�
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

	// �t�@�C���̃I�[�v��
	hHandle = CreateFile(strFileName, dwDesiredAccess, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

	return hHandle;
}

int VmpTraceParser::ReadData(HANDLE hHandle, DataContainer* objBuf, DataContainer* objBefore)
{
	DWORD dwReadSize;
	unsigned char* pBuf = (unsigned char*)objBuf->GetDataPointer();
	int intSize = 0;
	intSize = MAX_BUFFERSIZE - objBefore->GetCurrentDataSize();

	// �O�f�[�^�̃R�s�[
	objBuf->ImportData(objBefore->GetDataPointer(), objBefore->GetDataSize());
	pBuf = pBuf + objBefore->GetDataSize();

	// �f�[�^�̓ǂݍ��݁B
	// �ǂݍ��ݐ�͑O�f�[�^�̃R�s�[��̖���
	// �ǂݍ��݃T�C�Y�́A�o�b�t�@�T�C�Y����O�f�[�^�T�C�Y���������B
	if (!ReadFile(hHandle, pBuf, objBuf->GetDataSize(), &dwReadSize, NULL)) {
		return 0;
	}

	// �ǂݍ��݌��ʃT�C�Y��0
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

	// �ǂݍ��݌��ʃT�C�Y��0
	if (dwWriteSize == 0)
		return 0;

	return 1;
}


int VmpTraceParser::SearchStringsOffset(DataContainer* objBufferAddr, int* intOffset, const char* strSearchStrings)
{
	unsigned char* pBuf;
	unsigned char cSearchHead = 0x00;
	int intRet = 0;

	// �T�[�`�J�n�ʒu
	pBuf = (unsigned char*)objBufferAddr->GetDataPointer();
	
	// �擪��1�o�C�g�ŃT�[�`����B
	// *�������̂��߂�1�o�C�g�ŃT�[�`����B
	// *32�r�b�g�łȂ�4�o�C�g�A64�r�b�g�łȂ�8�o�C�g�ŃT�[�`����Ȃǂ̂ق��������ɂ͑����Ƃ͎v���B
	// *�������A�T�[�`�Ώە����񂪂������Z�����Ƃ�����̂ŁA�����܂ł̍l���ƃR�[�f�B���O�͍���͌�����B
	cSearchHead = strSearchStrings[0];

	for (;;) {

		// �擪�̕����̃q�b�g�`�F�b�N
		if (pBuf[*intOffset] == cSearchHead) {
			// ������`�F�b�N�̑O�ɁA�o�b�t�@�I�[�o�[�������`�F�b�N
			if (objBufferAddr->GetDataSize() - *intOffset < strlen(strSearchStrings))
				break;

			// ������Ŕ�r
			if (!memcmp((const char*)&pBuf[*intOffset], strSearchStrings, strlen(strSearchStrings))) {
				intRet = 1;
				break;
			}
		}

		// ���̃A�h���X�ցi�I�t�Z�b�g���o�b�t�@�͈͂ɂ��邩���`�F�b�N����j
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

	// �T�[�`�J�n�ʒu
	pBuf = (unsigned char*)objBufferAddr->GetDataPointer();

	// �擪��1�o�C�g�ŃT�[�`����B
	// *�������̂��߂�1�o�C�g�ŃT�[�`����B
	// *32�r�b�g�łȂ�4�o�C�g�A64�r�b�g�łȂ�8�o�C�g�ŃT�[�`����Ȃǂ̂ق��������ɂ͑����Ƃ͎v���B
	// *�������A�T�[�`�Ώە����񂪂������Z�����Ƃ�����̂ŁA�����܂ł̍l���ƃR�[�f�B���O�͍���͌�����B
	cSearchHead = strSearchStrings[0];

	i = intOffset;
	for (;;) {
		// ���s�����������ꍇ�A�m�[�q�b�g����B
		if (pBuf[i] == '\n')
			return 0;

		// �擪�̕����̃q�b�g�`�F�b�N
		if (pBuf[i] == cSearchHead) {
			// ������`�F�b�N�̑O�ɁA�o�b�t�@�I�[�o�[�������`�F�b�N
			if (objBufferAddr->GetDataSize() - i < strlen(strSearchStrings))
				break;

			// ������Ŕ�r
			if (!memcmp((const char*)&pBuf[i], strSearchStrings, strlen(strSearchStrings))) {
				intRet = 1;
				break;
			}
		}

		// ���̃A�h���X�ցi�I�t�Z�b�g���o�b�t�@�͈͂ɂ��邩���`�F�b�N����j
		i++;
		if (objBufferAddr->GetCurrentDataSize() <= i)
			break;
	}

	return intRet;
}

int VmpTraceParser::HexCharCheck(unsigned char cChar)
{
	// �P�U�i���̃L�����N�^�[�i0�`9�AA�`F�܂���a�`f�j���ǂ������`�F�b�N
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

			// �u+�v�����O��16�i�l�̕�����̐擪�I�t�Z�b�g��T���B
			objCopy.ImportData(objData->GetDataPointer(), objData->GetDataSize());
			pstrCopy = (unsigned char*)objCopy.GetDataPointer();
			for (j = i - 1; j >= 0; j--) {
				if (!HexCharCheck(pstrCopy[j])) {
					j++;
					break;
				}
			}
			intHead = j;

			// 16�i����������o�C�i���l�ɕϊ�����B
			llParam = HexToParam(&objCopy, &j);

			// �u+�v�������16�i�l�̕�����̐擪�I�t�Z�b�g��T��
			for (; j <= objCopy.GetCurrentDataSize(); j++) {
				if (HexCharCheck(pstrCopy[j])) {
					break;
				}
			}

			// �u+�v�������16�i�l�̕���������Z����B
			llParam += HexToParam(&objCopy, &j);

			// ������̑O�����R�s�[
			objData->ClearDataObject();
			for (k = 0; k < intHead; k++) {
				pstrAddress[k] = pstrCopy[k];
			}

			// �v�Z��̃A�h���X���o��
			sprintf_s((char*)&pstrAddress[k], objData->GetDataSize(), "%llX", llParam);

			// �A�h���X�������R�s�[����B
			objData->AppendStringA((LPSTR) &pstrCopy[j], objCopy.GetCurrentDataSize() - j + 1);

			// �I�t�Z�b�g�̒���
			i = j;
		}
	}

	return intRet;
}

// IDA�� Function Log �𕪐͂��鏈���B
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

	// �t�@�C�����J��
	hDataFile = this->OpenFile(strDataFileName, READFILE);
	hOutFile = this->OpenFile(strOutFileName, WRITEFILE);

	if (hDataFile == (HANDLE)HFILE_ERROR || hOutFile == (HANDLE)HFILE_ERROR) {
		if (hDataFile != (HANDLE)HFILE_ERROR)
			CloseHandle(hDataFile);
		if (hOutFile != (HANDLE)HFILE_ERROR)
			CloseHandle(hOutFile);

		return VMPTRACEPARSER_FILEOPENERROR;
	}

	// �ǂݍ��݃T�C�Y���擾
	if (GetFileSizeEx(hDataFile, &objFileSize) == false)
	{
		CloseHandle(hDataFile);
		return VMPTRACEPARSER_FATALERROR;
	}

	// �̈�̊m��
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

	// �f�[�^�̃p�[�X
	for (;;)
	{
		// �f�[�^�̓ǂݍ���
		if (!ReadData(hDataFile, objBufferAddr, objBeforeData))
			break;

		objBeforeData->DeleteDataObject();
		intOffset = 0;

		// �ǂݍ��񂾃f�[�^�̔���
		for (;;)
		{
			// 1: �Z�O�����g��.vmp0�̍s�ɐ؂�ւ��s��T���B
			// 1.1: .text�Z�O�����g�̃L�[���[�h��T���B
			if ((intFlag & FLAG_HIT_TEXT) == 0) {
				if (!SearchStringsOffset(objBufferAddr, &intOffset, ".text")) {
					// �q�b�g�����ɖ����܂ōs�����ꍇ�A�c���ׂ��f�[�^��objBeforeData�Ɋi�[����Break;
					objBeforeData->CreateDataObject(objBufferAddr->GetCurrentDataSize() - intOffset);
					objBeforeData->ImportData((unsigned char*)objBufferAddr->GetDataPointer() + intOffset, objBufferAddr->GetCurrentDataSize() - intOffset);
					objBeforeData->SetCurrentDataSize(objBufferAddr->GetCurrentDataSize() - intOffset);

					break;
				}
			}
			intFlag |= FLAG_HIT_TEXT;

			// 1.2: .vmp0�Z�O�����g�̃L�[���[�h��T���B
			// �Ƃ肠�����A����Ƀq�b�g�����Ƃ��ɒ��O��.text�Z�O�����g�ł��邱�Ƃ̃`�F�b�N�܂ł͂��̒i�K�ł͍l�����Ȃ��B

			if ((intFlag & FLAG_HIT_VMP0) == 0) {

				// .vmp0�Z�O�����g�̃L�[���[�h��T���B
				if (!SearchStringsOffset(objBufferAddr, &intOffset, ".vmp0")) {
					// �q�b�g�����ɖ����܂ōs�����ꍇ�A�c���ׂ��f�[�^��objBeforeData�Ɋi�[����Break;
					// �����̏ꍇ�A����ɓǂݍ��񂾐擪�̃f�[�^�ł��̏����Ƀq�b�g�����ꍇ�A���O�̍s�̃f�[�^���Q�Ƃ��邱�ƂɂȂ邪�A
					// ���������Ȃ��悤1�s���̒��O�f�[�^���ێ�����K�v�����邱�Ƃɗ��ӁB

					// �܂��A1�s�O�̃f�[�^�̐擪�ʒu��T���B
					int intPrevHead = 0;
					unsigned char* pPrevHead = (unsigned char*)objBufferAddr->GetDataPointer();

					// ���O�̍s�̐擪�ɓ��B����ɂ́A2��\n�Ƀq�b�g����͂��B
					// (��������\r\n�̔���܂ł͂��Ȃ����̂Ƃ���B)
					//   �܂��A.vmp0�����������s�̐擪��T���B
					for (intPrevHead = intOffset; intPrevHead >= 0; intPrevHead--) {
						if (pPrevHead[intPrevHead] == '\n')
							break;
					}

					//   ����ɑO�̍s�̐擪��T���B
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

				// �q�b�g�����s��.text�Z�O�����g�̏ꍇ�A�T���Ȃ����B

			}
			intFlag |= FLAG_HIT_VMP0;


			// 2: ��������.vmp0�̍s�̒��O��.text�̍s����Acall�֐��̏��𒊏o����
			if ((intFlag & FLAG_GET_TEXT_CALLFUNC) == 0) {
				int intPrevHead = 0;
				unsigned char* pPrevHead = (unsigned char*)objBufferAddr->GetDataPointer();
				int intTextFlg = 0;

				// ���O�̍s�̐擪�ɓ��B����ɂ́A2��\n�Ƀq�b�g����͂��B
				// (��������\r\n�̔���܂ł͂��Ȃ����̂Ƃ���B)
				for (intPrevHead = intOffset; intPrevHead >= 0; intPrevHead--) {
					if (pPrevHead[intPrevHead] == '\n')
						break;
				}
				// .vmp0�����������s�ɁA.text�����邩�ǂ������`�F�b�N����B
				// �s�S�̂ł͂Ȃ��A.vmp0�̌��������s��.vmp0�����񂪂݂������ꏊ���O�̃f�[�^�ŏ\���Ȃ̂ŁA����ŏ����B
				// �ȉ��̂悤�ȃP�[�X�̓n�Y���̂��߁A�e�����߂̏����B
				// �u.text:00007FF60AD0AE10  	call    near ptr qword_7FF60ADFA9C0+1DEh	call .vmp0:qword_7FF60ADFA9C0+1DE�v
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

				// ���̍s�̊Ԃ�".text"�������邩���`�F�b�N
				for (; intPrevHead < intOffset - 4; intPrevHead++) {
					if (!memcmp((const char*) & pPrevHead[intPrevHead], ".text", strlen(".text"))) {
						intTextFlg = 1;
						break;
					}
				}

				// ���̎���.vmp0�̒��O��.text�Z�O�����g�łȂ��ꍇ�A�G���[�Ƃ��Ď��̃��R�[�h��T���B
				if (!intTextFlg) {
					intFlag = 0;
					continue;
				}

				// �q�b�g�����ʒu����1�s���̃f�[�^���R�s�[
				int i;
				for (i = intPrevHead; i < objBufferAddr->GetCurrentDataSize() - 1; i++) {
					if (pPrevHead[i] == '\n') {
						break;
					}
				}
				i++;

				// ��s�̃f�[�^�����ʂɏo�͂���B
				DataContainer objImportData;
				objImportData.CreateDataObject(i - intPrevHead + 1);
				memcpy(objImportData.GetDataPointer(), &pPrevHead[intPrevHead], i - intPrevHead);
				objImportData.SetCurrentDataSize(i - intPrevHead + 1);

				// �f�[�^���ŁA�A�h���X���u+�v�t���ɂȂ��Ă���ӏ����v�Z����B
				// ��F.text:sub_7FF60AC9BCE8+2D
				// ��.text:sub_7FF60AC9BD15
				CalAddress(&objImportData);

//				objResultData->AppendString((const TCHAR*)objImportData.GetDataPointer(), objImportData.GetDataSize());
				objResultData->AppendStringA((LPSTR)objImportData.GetDataPointer(), objImportData.GetCurrentDataSize());
				objImportData.DeleteDataObject();

			}

			intFlag |= FLAG_GET_TEXT_CALLFUNC;


			// 3: .vmp0����ʂ̃Z�O�����g�ɐ؂�ւ��Ō�̍s��T���AAPI�̖��̓��𒊏o����B
			// 3.1: .vmp0�Z�O�����g�̃L�[���[�h�������Ȃ�s��T���B
			// 3.2: .vmp0�Z�O�����g�̃L�[���[�h�������Ȃ�s�̑O�̍s�𒊏o���邱�ƂōŌ�̍s�𓾂�B
			// 3.3: ���������s����call���Ă���API�̖��̓��𒊏o����B
			if ((intFlag & FLAG_GET_APINAME) == 0) {
				unsigned char* pBuf = (unsigned char*)objBufferAddr->GetDataPointer();
				DataContainer objRowData;
				int flag;

				objRowData.CreateDataObject(128);

				for (;;) {
					int i;
					int intOffsetBackup;

					intOffsetBackup = intOffset;
					// 1�s���̃f�[�^�𒊏o
					for (i = intOffset; i < objBufferAddr->GetCurrentDataSize() - 1; i++) {
						if (pBuf[i] == '\n') {
							break;
						}
					}
					i++;

					objRowData.ClearDataObject();
					objRowData.ImportData(&pBuf[intOffset], i - intOffset);
					intOffset = i;

					// ���̍s��".vmp0"�ł͂Ȃ����Ƃ��`�F�b�N
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

					// �`�F�b�N���Ƀf�[�^�̏I�[�������ꍇ�A�K�v�f�[�^��ێ����Ď��̃f�[�^�ǂݍ��݂ɗU��
					if ((flag & 2) == 0) {
						objRowData.DeleteDataObject();
						objBeforeData->CreateDataObject(objBufferAddr->GetCurrentDataSize() - intOffsetBackup);
						objBeforeData->ImportData((unsigned char*)objBufferAddr->GetDataPointer() + intOffsetBackup, objBufferAddr->GetCurrentDataSize() - intOffsetBackup);
						objBeforeData->SetCurrentDataSize(objBufferAddr->GetCurrentDataSize() - intOffsetBackup);

						break;
					}

					// .vmp0�������ꍇ�A�����ɍ��v���Ȃ����ߎ��̍s��
					if (flag == 3)
						continue;

					// �����܂ł����ꍇ�A���̍s��.vmp0�ł͂Ȃ����s�̖����i\n�j���������Ă���B
					// (���̂Ƃ��AintOffset�͎��̍s�̐擪�A�h���X�ɂȂ��Ă���͂��B���̂��߁A�O�̍s�̃f�[�^��objRowData���g�p�B)

					// �܂��A��������ŏ��ɂ݂���u:�v��T���B
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

				// ���̃f�[�^�̓ǂݍ��݂֍s���ꍇ�̃u���[�N
				if ((flag & 2) == 0) {
					break;
				}

			}

			intFlag |= FLAG_GET_APINAME;



			// �t���O���Z�b�g���A���̃f�[�^�̃T�[�`�B
			intFlag = 0;
		}

		// ��������
		// �����Ȃ�p�[�X�����I��
		if (objBufferAddr->GetCurrentDataSize() < MAX_BUFFERSIZE)
			break;

		// �����ł͂Ȃ��ꍇ�A�c��f�[�^���o�b�t�@�ɃR�s�[���A����Ƀf�[�^��ǂݍ��ށB
		objBufferAddr->ClearDataObject();
	}

	// �f�[�^���t�@�C���ɏo��
	WriteData(hOutFile, objResultData);


	// �I�u�W�F�N�g�̔j��
	if (objBufferAddr != NULL)
		delete (objBufferAddr);
	if (objBeforeData != NULL)
		delete(objBeforeData);
	if (objResultData != NULL)
		delete(objResultData);

	// �t�@�C�������
	CloseHandle(hDataFile);
	CloseHandle(hOutFile);

	return intErrorFlg;
}
