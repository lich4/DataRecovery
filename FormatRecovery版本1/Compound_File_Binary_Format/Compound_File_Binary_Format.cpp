// Compound_File_Binary_Format.cpp : Defines the entry point for the DLL application.
//

// D0 CF 11 E0 A1 B1 1A E1
// DOC, DOT, PPS, PPT, XLA, XLS,    Microsoft Office applications (Word, Powerpoint, Excel, Wizard)
//							WIZ		[See also Word, Powerpoint, and Excel "subheaders" at byte offset 512]
//									[Note the similarity between D0 CF 11 E0 and the word "docfile"!] 
// 							AC_		CaseWare Working Papers compressed client file 
// 							ADP		Access project file 
// 							APR		Lotus/IBM Approach 97 file 
// 							DB		MSWorks database file 
// 							MSC		Microsoft Common Console Document 
// 							MSI		Microsoft Installer package 
// 							MTW		Minitab data file 
// 							OPT		Developer Studio File Workspace Options file 
// 							PUB		MS Publisher file 
// 							QBM		QuickBooks Portable Company File 
// 							RVT		Revit Project file 
// 							SOU		Visual Studio Solution User Options file 
// 							SPO		SPSS output file 
// 							VSD		Visio file 
// 							WPS		MSWorks text document 

#include "stdafx.h"

#ifndef COMPOUND_FILE_BINARY_FORMAT_EXPORTS
#define COMPOUND_FILE_BINARY_FORMAT_EXPORTS
#endif

#include "Compound_File_Binary_Format.h"
#include "defs.h"
#include <stdlib.h>

//具体文件格式分析请自行用Winhex解决，看懂格式再看我的代码

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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


COMPOUND_FILE_BINARY_FORMAT_API BOOL CanGetFileLenImmediately=FALSE;//认为微软复合文档不能立刻求大小
COMPOUND_FILE_BINARY_FORMAT_API BOOL IsFileSigSimple=TRUE;//可以简单通过签名树查找

COMPOUND_FILE_BINARY_FORMAT_API FORMAT_SFILESIZE GetFileLenImmediately(FORMAT_DATA* pData)
{
	return 0;
}

COMPOUND_FILE_BINARY_FORMAT_API BOOL GetFileSig(PVOID pData)
{
	FORMAT_DATA sig[]={0xD0,0xCF,0x11,0xE0,0xA1,0xB1,0x1A,0xE1};
	FORMAT_SIMPLETYPE* data=(FORMAT_SIMPLETYPE*)pData;
	memcpy(data->FileSig,sig,sizeof(sig));
	data->FileSigLen=sizeof(sig);
	return TRUE;
}

#define IsInValid(X) (X<-3 || X>0x7FFFF)//最大簇不可能大于0x7FFFF,否则文件大小将大于0x7FFFF*0x200=255M

COMPOUND_FILE_BINARY_FORMAT_API BOOL GetRawFileInfo(FORMAT_DATAPOINTER& pData,FORMAT_FILEINFO* pfileinfo)
{//通篇考虑磁盘数据合法性
	//以下算法为doc ppt xls文件大小最精确计算法，是我发明的，世界首创，版权归lichao890427所有，其精度甚至比显示的文件大小更加精确
	pfileinfo->bitmap=0;
	pfileinfo->IsFileValid=FALSE;
	StructuredStorageHeader* head=(StructuredStorageHeader*)LocalAlloc(LMEM_ZEROINIT | LMEM_FIXED,0x200);
	memcpy(head,(FORMAT_DATA*)pData,0x200);//此处必须复制，否则pData一旦改变将会破坏head数据

#define MAXMSATNUM 0x1000
	int MSAT[MAXMSATNUM];
	int index=0,i,maxsec,dirbegin;
	memset(MSAT,-1,sizeof(MSAT));
	int sectorsize=(1<<head->_uSectorShift);
	if(head->_uSectorShift>0x10)
		return FALSE;
	int* MIndex=NULL;
	pData.BeforeSeek();//保存指针位置

// 	try
// 	{
		//MSAT首扇区
		for(i=0;i<(sectorsize-sizeof(StructuredStorageHeader))/4 && head->_sectFat[i] >= 0;i++)
		{
			if(IsInValid(head->_sectFat[i]) || pData.IsEndOfDisk())
				goto END;
			MSAT[index++]=head->_sectFat[i];
		}

		if(head->_sectDifStart != ENDOFCHAIN)
		{
			if(IsInValid(head->_sectDifStart) )
 				goto END;
			pData.SeekFromOrigin(sectorsize*(head->_sectDifStart+1));
			for(i=0;i<head->_csectDif-1;i++)
			{
				memcpy(MSAT+index,(FORMAT_DATA*)pData,sectorsize-4);
				index+=(sectorsize-4)/4;
				if(IsInValid(((int*)pData)[(sectorsize-4)/4]))
 					goto END;
				pData.SeekFromOrigin(sectorsize*(((int*)pData)[(sectorsize-4)/4]+1));//此函数产生的目的就是为了防止反复ReadFile
				if(IsInValid(((int*)pData)[(sectorsize-4)/4]) || pData.IsEndOfDisk())//验证数据合法
					goto END;
			}

			//MSAT最后扇区
			for(i=0;i<sectorsize/4 && ((int*)pData)[i]>0;i++)
			{
				MSAT[index++]=((int*)pData)[i];
			}
		}

		maxsec=head->_sectDirStart;
		while(index-- && MSAT[index]<0);
		index++;

		MIndex=(int*)LocalAlloc(LMEM_ZEROINIT | LMEM_FIXED,index*sectorsize);
		for(i=0;i<index;i++)
		{
			if(MSAT[i]<0)
				break;
			if(MSAT[i]>maxsec)
				maxsec=MSAT[i];
			if(IsInValid(MSAT[i]) || pData.IsEndOfDisk())
				goto END;
			pData.SeekFromOrigin(sectorsize*(MSAT[i]+1));
			memcpy(MIndex+i*sectorsize/4,(FORMAT_DATA*)pData,sectorsize);
		}
		for(i=sectorsize/4-1;MIndex[i] == FREESECT;i--);
		maxsec=max(i,maxsec);

		//查找目录项
		strcpy_s(pfileinfo->Description,"目录项:\r\n");
		pfileinfo->bitmap |= 0x08;
		dirbegin=head->_sectDirStart;
		BOOL hasfind=FALSE;
		int maxdir=0x100;
		while(dirbegin >= 0)
		{
			if( --maxdir == 0 || IsInValid(dirbegin) || dirbegin >=  index*sectorsize/4 || IsInValid(maxsec) || pData.IsEndOfDisk())
				goto END;
			pData.SeekFromOrigin(sectorsize*(dirbegin+1));
			StructuredStorageDirectoryEntry* dir=(StructuredStorageDirectoryEntry*)(FORMAT_DATA*)pData;
			for(i=0;i<sectorsize/sizeof(StructuredStorageDirectoryEntry) && *(WCHAR*)dir[i]._ab != L'\0';i++)
			{
				if(!hasfind)
				{
					CHAR chDir[32];
					memset(chDir,0,sizeof(chDir));
					wcstombs(chDir,(wchar_t*)dir[i]._ab,32);

					if(!strcmp(chDir,"WordDocument"))
					{
						pfileinfo->bitmap |= 0x02;
						strcpy_s(pfileinfo->fileextDeepscan,"doc");
						hasfind=TRUE;
					}
					else if(!strcmp(chDir,"PowerPoint Document") || !strcmp(chDir,"Pictures"))
					{
						pfileinfo->bitmap |= 0x02;
						strcpy_s(pfileinfo->fileextDeepscan,"ppt");
						hasfind=TRUE;
					}
					else if(!strcmp(chDir,"Workbook") || !strcmp(chDir,"Book"))
					{
						pfileinfo->bitmap |= 0x02;
						strcpy_s(pfileinfo->fileextDeepscan,"xls");
						hasfind=TRUE;
					}

					strncat_s(pfileinfo->Description,chDir,32);
					strcat_s(pfileinfo->Description,"\r\n");
					if(strlen(pfileinfo->Description)>0x100)
						hasfind=TRUE;//未定类型，未定是否有效
				}

				if(dir[i]._mse != STGTY_STREAM && dir[i]._mse != STGTY_ROOT)//流和根仓库有效
					continue;
				if(dir[i]._mse == STGTY_STREAM && dir[i]._ulSize < 0x1000)//如果可以存放于短流,小于4096说明0x74处为短流扇区号
					continue;
				maxsec=max(dir[i]._sectStart,maxsec);
			}
			dirbegin=MIndex[dirbegin];
		}
// 	}
// 	catch(...)
// 	{
// 		;
// 	}

	pfileinfo->bitmap |= 0x01;
	pfileinfo->filesize=maxsec+2;
	pfileinfo->filesize *= sectorsize;
	
	pfileinfo->bitmap |= 0x04;
	pfileinfo->IsFileValid=TRUE;
END:
	pData.AfterSeek();
	if(MIndex)
		LocalFree((HLOCAL)MIndex);
	LocalFree((HLOCAL)head);
	return TRUE;
}

COMPOUND_FILE_BINARY_FORMAT_API VOID GetSupportExtension(CHAR Ext[MAX_EXTLEN])
{
	strcpy_s(Ext,MAX_EXTLEN,"doc or ppt or xls");
}
