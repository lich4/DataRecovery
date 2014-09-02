#include "..\common.h"

#ifndef TEXT_FILE_FORMAT
#define TEXT_FILE_FORMAT

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 TEXT_FILE_FORMAT_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// TEXT_FILE_FORMAT_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef TEXT_FILE_FORMAT_EXPORTS
#define TEXT_FILE_FORMAT_API __declspec(dllexport)
#else
#define TEXT_FILE_FORMAT_API __declspec(dllimport)
#endif

extern "C"
{
	extern TEXT_FILE_FORMAT_API BOOL CanGetFileLenImmediately;
	extern TEXT_FILE_FORMAT_API BOOL IsFileSigSimple;

	TEXT_FILE_FORMAT_API FORMAT_SFILESIZE GetFileLenImmediately(FORMAT_DATA* pData);
	TEXT_FILE_FORMAT_API BOOL GetFileSig(PVOID pData);
	TEXT_FILE_FORMAT_API BOOL GetRawFileInfo(FORMAT_DATAPOINTER& pData,FORMAT_FILEINFO* pfileinfo);
	TEXT_FILE_FORMAT_API VOID GetSupportExtension(CHAR Ext[MAX_EXTLEN]);
};

#endif
