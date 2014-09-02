// Portable_Document_Format.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#ifndef PORTABLE_DOCUMENT_FORMAT_EXPORTS
#define PORTABLE_DOCUMENT_FORMAT_EXPORTS
#endif

#include "Portable_Document_Format.h"

//具体文件格式分析请自行用Winhex解决，看懂格式再看我的代码

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call,  LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


PORTABLE_DOCUMENT_FORMAT_API BOOL CanGetFileLenImmediately=FALSE;//暂时没时间研究pdf格式，所以没有快速算法计算大小
PORTABLE_DOCUMENT_FORMAT_API BOOL IsFileSigSimple=TRUE;

PORTABLE_DOCUMENT_FORMAT_API FORMAT_SFILESIZE GetFileLenImmediately(FORMAT_DATA* pData)
{
	return 0;
}

PORTABLE_DOCUMENT_FORMAT_API BOOL GetFileSig(PVOID pData)
{
 	FORMAT_DATA sig[]={0x25,0x50,0x44,0x46};
 	FORMAT_SIMPLETYPE* data=(FORMAT_SIMPLETYPE*)pData;
 	memcpy(data->FileSig,sig,sizeof(sig));
 	data->FileSigLen=sizeof(sig);
	return TRUE;
}

PORTABLE_DOCUMENT_FORMAT_API BOOL GetRawFileInfo(FORMAT_DATAPOINTER& pData,FORMAT_FILEINFO* pfileinfo)
{
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	DWORD maxaccess=0x100*dwGran;
	FORMAT_FILESIZE filesize=0;
	int loopnum=0;
	int loopmax=0x3FFFFFFF/maxaccess;//最大支持1G文件

 	pfileinfo->bitmap=0;
 	pfileinfo->IsFileValid=FALSE;

	FORMAT_DATA* p=(FORMAT_DATA*)pData+5,*endv;
	for(endv=p;*endv != 0x0A && *endv != 0x0D && *endv != '%';endv++);
	*endv='\0';
	pfileinfo->bitmap |= 0x08;
	strcpy_s(pfileinfo->Description,"版本号:");
	strncat_s(pfileinfo->Description,(CHAR*)p,16);

// 	try
// 	{
		while(loopnum<loopmax && !pData.IsEndOfDisk())
		{
			p=(FORMAT_DATA*)pData;
			for(int i=0;i<(int)maxaccess;i++)
			{
				if(*(DWORD*)(p+i) == 'FOE%')//'%EOF'为结束标志
				{
					if(*(p+i+4) != 0 && *(p+i+5) != 0 && *(p+i+6) != 0)//伪结束符
						continue;
					pfileinfo->bitmap |= 0x01;
					pfileinfo->filesize=loopnum;
					pfileinfo->filesize *= maxaccess;
					pfileinfo->filesize += i+strlen((char*)p+i);

					pfileinfo->bitmap |= 0x04;
					pfileinfo->IsFileValid=TRUE;

					pfileinfo->bitmap |= 0x02;
					strcpy_s(pfileinfo->fileextDeepscan,"pdf");

					goto END;
				}
			}
			loopnum++;
			pData+=maxaccess;
		}
// 	}
// 	catch(...)
// 	{
// 		;
// 	}

END:

	return TRUE;
}

PORTABLE_DOCUMENT_FORMAT_API VOID GetSupportExtension(CHAR Ext[MAX_EXTLEN])
{
	strcpy_s(Ext,MAX_EXTLEN,"pdf");
}