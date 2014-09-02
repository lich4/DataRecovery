#include "..\common.h"

#ifndef PORTABLE_DOCUMENT_FORMAT
#define PORTABLE_DOCUMENT_FORMAT
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PORTABLE_DOCUMENT_FORMAT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PORTABLE_DOCUMENT_FORMAT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
	
#ifdef PORTABLE_DOCUMENT_FORMAT_EXPORTS
#define PORTABLE_DOCUMENT_FORMAT_API __declspec(dllexport)
#else
#define PORTABLE_DOCUMENT_FORMAT_API __declspec(dllimport)
#endif
	
extern "C"
{
	extern PORTABLE_DOCUMENT_FORMAT_API BOOL CanGetFileLenImmediately;
	extern PORTABLE_DOCUMENT_FORMAT_API BOOL IsFileSigSimple;
	
	PORTABLE_DOCUMENT_FORMAT_API FORMAT_SFILESIZE GetFileLenImmediately(FORMAT_DATA* pData);
	PORTABLE_DOCUMENT_FORMAT_API BOOL GetFileSig(PVOID pData);
	PORTABLE_DOCUMENT_FORMAT_API BOOL GetRawFileInfo(FORMAT_DATAPOINTER& pData,FORMAT_FILEINFO* pfileinfo);
	PORTABLE_DOCUMENT_FORMAT_API VOID GetSupportExtension(CHAR Ext[MAX_EXTLEN]);
};


#endif