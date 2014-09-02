#include "..\common.h"

#ifndef COMPOUND_FILE_BINARY_FORMAT
#define COMPOUND_FILE_BINARY_FORMAT
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the COMPOUND_FILE_BINARY_FORMAT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// COMPOUND_FILE_BINARY_FORMAT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.



#ifdef COMPOUND_FILE_BINARY_FORMAT_EXPORTS
#define COMPOUND_FILE_BINARY_FORMAT_API __declspec(dllexport)
#else
#define COMPOUND_FILE_BINARY_FORMAT_API __declspec(dllimport)
#endif

extern "C"
{
	extern COMPOUND_FILE_BINARY_FORMAT_API BOOL CanGetFileLenImmediately;
	extern COMPOUND_FILE_BINARY_FORMAT_API BOOL IsFileSigSimple;
	
	COMPOUND_FILE_BINARY_FORMAT_API FORMAT_SFILESIZE GetFileLenImmediately(FORMAT_DATA* pData);
	COMPOUND_FILE_BINARY_FORMAT_API BOOL GetFileSig(PVOID pData);
	COMPOUND_FILE_BINARY_FORMAT_API BOOL GetRawFileInfo(FORMAT_DATAPOINTER& pData,FORMAT_FILEINFO* pfileinfo);
	COMPOUND_FILE_BINARY_FORMAT_API VOID GetSupportExtension(CHAR Ext[MAX_EXTLEN]);
};

#endif