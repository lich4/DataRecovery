// Text_File_Format.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Text_File_Format.h"

TEXT_FILE_FORMAT_API BOOL CanGetFileLenImmediately=FALSE;
TEXT_FILE_FORMAT_API BOOL IsFileSigSimple=FALSE;

TEXT_FILE_FORMAT_API FORMAT_SFILESIZE GetFileLenImmediately(FORMAT_DATA* pData)
{
	return FALSE;
}

BOOL GetComplexFileType(CONST FORMAT_DATA DiskInfo[MAX_FILESIGLEN])
{
	if(*(WORD*)DiskInfo == 0xFEFF)//UNICODE_LITTLE_ENDIAN
	{
		WORD* ptr=(WORD*)DiskInfo;
		for(int i=0;i<16;i++)
		{
			if(ptr[i] == 0)
				return FALSE;
		}
		return TRUE;
	}
	else if(*(WORD*)DiskInfo == 0xFFFE)//UNICODE_BIG_ENDIAN
	{
		WORD* ptr=(WORD*)DiskInfo;
		for(int i=0;i<64;i++)
		{
			if(ptr[i] == 0)
				return FALSE;
		}
		return TRUE;
	}
	else if((*(DWORD*)DiskInfo & 0xFFFFFF) == 0xBFBBEF)//UTF_8
	{
		BYTE* ptr=(BYTE*)DiskInfo;
		for(int i=0;i<64;i++)
		{
			if(ptr[i] == 0)
				return FALSE;
		}
		return TRUE;
	}
	else
	{
		if(*(WORD*)DiskInfo == '\n\r' || *DiskInfo == '\t')//'\n\r' == \r\n
		{
			do 
			{
				DiskInfo++;
			} 
			while (*DiskInfo == '\n' || *DiskInfo == '\r' || *DiskInfo == '\t');
		}
		for(int i=0;i<128;i++)//如果4行都没有的话，恢复他干啥。。。
		{
			BYTE bit1=(DiskInfo[0]>>5),bit2=(DiskInfo[1]>>5);//单个32-127(1-3) 或者 两个128-255(4-7)
			//ascii可能字符组合
			if((bit1 >= 1 && bit1 < 4) || DiskInfo[0] == '\r' || DiskInfo[0] == '\n' || DiskInfo[0] == '\t')
			{
				DiskInfo++;
			}
			else if(bit1 >= 4 && bit2 >= 4)
			{
				DiskInfo += 2;
			}
			else
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

TEXT_FILE_FORMAT_API BOOL GetFileSig(PVOID pData)
{
	*(DWORD*)pData=(DWORD)GetComplexFileType;
	return TRUE;
}

TEXT_FILE_FORMAT_API BOOL GetRawFileInfo(FORMAT_DATAPOINTER& pData,FORMAT_FILEINFO* pfileinfo)
{
	pfileinfo->bitmap=0;
	pfileinfo->IsFileValid=FALSE;
	FORMAT_DATA* DiskInfo=(FORMAT_DATA*)pData;

	if(*(WORD*)DiskInfo == 0xFEFF || *(WORD*)DiskInfo == 0xFFFE)//UNICODE
	{
		WORD* ptr=(WORD*)DiskInfo;
		while(*ptr++);
		pfileinfo->bitmap |= 0x01;
		pfileinfo->filesize=(BYTE*)ptr-(BYTE*)DiskInfo-2;
		pfileinfo->bitmap |= 0x08;
		if(*(WORD*)DiskInfo == 0xFEFF)
			strcpy_s(pfileinfo->Description,"UNICODE_LITTLE_ENDIAN_TXT");
		else
			strcpy_s(pfileinfo->Description,"UNICODE_BIG_ENDIAN_TXT");
	}
	else
	{
		BYTE* ptr=(BYTE*)DiskInfo;
		while(*ptr++);
		pfileinfo->bitmap |= 0x01;
		pfileinfo->filesize=(BYTE*)ptr-(BYTE*)DiskInfo-1;
		pfileinfo->bitmap |= 0x08;
		if((*(DWORD*)DiskInfo & 0xFFFFFF) == 0xBFBBEF)
			strcpy_s(pfileinfo->Description,"UTF8_TXT");
		else
			strcpy_s(pfileinfo->Description,"ANSI_TXT");
	}

	pfileinfo->bitmap |= 0x04;
	pfileinfo->IsFileValid=TRUE;
	return TRUE;
}

TEXT_FILE_FORMAT_API VOID GetSupportExtension(CHAR Ext[MAX_EXTLEN])
{
	strcpy_s(Ext,MAX_EXTLEN,"txt");
}
