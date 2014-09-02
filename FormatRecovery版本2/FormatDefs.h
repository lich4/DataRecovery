#include "stdafx.h"
#include <afxtempl.h>

#ifndef FORMAT_RECOVERY
#define FORMAT_RECOVERY

//定义常量
#define FORMAT_API			WINAPI
#define MAX_FILESIGLEN		0x100
#define MAX_CHILDNUM		0x100
#define FORMAT_FAILURE		(DWORD)-1
#define FORMAT_INVALIDID	(DWORD)-1
#define TREE_ADDFAILURE		FALSE
#define TREE_ADDSUCCESS		TRUE
#define MAX_EXTLEN			0x100
#define MAX_DESCIPTION		0x1000

//结构清晰化~~
#define CLASS_BEGIN_DEFINETYPE//定义类结构
#define CLASS_END_DEFINETYPE
#define CLASS_BEGIN_DEFMEMBERVAR//定义类成员变量
#define CLASS_END_DEFMEMBERVAR
#define CLASS_BEGIN_DEFMEMFUNC//定义类成员函数
#define CLASS_END_DEFMEMFUNC

//定义一些意义更加明显的类型
typedef LPCTSTR				FORMAT_EXTENSION;	
typedef BYTE				FORMAT_DATA;				
typedef DWORD				FORMAT_SFILESIZE;//小于4G的文件大小
typedef LONGLONG			FORMAT_FILESIZE;//大于4G的文件大小
typedef LONGLONG			FORMAT_DISKSIZE;
typedef LONGLONG			FORMAT_FILEPOSITION;
typedef HMODULE				FORMAT_DLLHANDLE;
typedef HANDLE				FORMAT_DISKHANDLE;
typedef INT					FORMAT_DATATYPE;
typedef INT					FORMAT_FILESIGLEN;
typedef BOOL				FORMAT_ISSUCCESS;
typedef INT					FORMAT_FILESIGID;//唯一对应Dll的序数，唯一对应一个Sig
typedef INT					FORMAT_ERRORCODE;		

typedef BOOL (*FORMAT_DLLFUNC_GETFILESIGNATURE)(PVOID);
typedef FORMAT_ISSUCCESS (*FORMAT_GETFILESIG)(PVOID);
typedef VOID (*FORMAT_GETSUPPORTEXT)(CHAR Ext[MAX_EXTLEN]);
typedef BOOL (*FORMAT_GETCOMPLEXFILETYPE)(CONST FORMAT_DATA DiskInfo[MAX_FILESIGLEN]);

//签名树节点
class SigTreeNode
{
public:
	CLASS_BEGIN_DEFMEMFUNC
		SigTreeNode();
		SigTreeNode*& operator[](FORMAT_FILESIGID Id);//获取子节点
		FORMAT_FILESIGID& operator*();//获取Id
	CLASS_END_DEFMEMFUNC

private:
	CLASS_BEGIN_DEFMEMBERVAR
		SigTreeNode* Child[MAX_CHILDNUM];
		//文件签名每一位为0x00-0xFF 因此Child每个元素序号对应当前位的签名字节码，连接下一位签名字节码
		//例如D0CF11E0  根节点的第D0个子节点会存在，D0节点的第CF个子节点会存在，CF节点的第11个子节点会存在，
		//11节点的第E0个子节点会存在，而E0点的Id则设为Dll对应的序号

		FORMAT_FILESIGID Id;//当前枝干对应ID，如果不存在则为FORMAT_INVALIDID，表示没有dll支持当前长度的sig
	CLASS_END_DEFMEMBERVAR
};

//文件签名结构体
struct FORMAT_SIMPLETYPE
{
	FORMAT_DATA FileSig[MAX_FILESIGLEN];//文件签名内容
	FORMAT_FILESIGLEN FileSigLen;//文件签名长度                             AddTreeTrunk调用之前检测参数
};

//查找前对于简单文件签名先生成文件签名树，对于复杂文件签名获取其文件签名验证函数地址，查找时先搜索文件签名树，后搜索函数
class CFileSigTree
{
public:
	CLASS_BEGIN_DEFMEMFUNC
		CFileSigTree();
		virtual ~CFileSigTree();
		BOOL AddTreeTrunk(IN CONST FORMAT_SIMPLETYPE& ObjFileSig,IN FORMAT_FILESIGID Id);//加入新的文件签名
		FORMAT_FILESIGID SearchFileSig(IN CONST FORMAT_DATA DiskInfo[MAX_FILESIGLEN]) CONST;//搜索文件签名，如果没搜到返回FORMAT_INVALIDID
	CLASS_END_DEFMEMFUNC

private:
	CLASS_BEGIN_DEFMEMFUNC
		VOID DelTree(SigTreeNode*);//递归删除签名树
	CLASS_END_DEFMEMFUNC

	CLASS_BEGIN_DEFMEMBERVAR
		SigTreeNode* Root;//签名树根节点，维系整个签名树的命脉
	CLASS_END_DEFMEMBERVAR
};

//磁盘内存映射指针，本想实现类似于内存文件映射，有待改进
typedef class CDiskMappingPointer
{//安全起见访问范围最大为m_dwBlockBytes/2
public:
	CLASS_BEGIN_DEFMEMFUNC
		CDiskMappingPointer(FORMAT_DISKHANDLE hDisk,FORMAT_FILEPOSITION Base=0);//需要提供要搜索的设备句柄
		CDiskMappingPointer(CONST CDiskMappingPointer& ObjPtr);//自动重载=()
		virtual ~CDiskMappingPointer();
		FORMAT_FILEPOSITION GetBase(VOID) CONST;//获取基址
		FORMAT_DISKSIZE GetDiskSize(VOID) CONST;//获取该设备磁盘大小
		FORMAT_FILEPOSITION GetOffset(VOID) CONST;//获取偏移   基址和偏移共同确定当前的内存位置
		int GetProgress(VOID);//计算当前访问位置占整个磁盘的进度
		BOOL IsEndOfDisk(VOID) CONST;//是否是磁盘结尾
		BOOL SetDiskPointer(FORMAT_FILEPOSITION pos);//读取磁盘特定位置
		VOID SwapBuf(VOID);
		FORMAT_DATA operator[](FORMAT_SFILESIZE delta) CONST;
		VOID operator+=(FORMAT_SFILESIZE delta);//移动指针
		FORMAT_DATA operator*(VOID) CONST;
		operator FORMAT_DATA*(VOID) CONST;
		operator DWORD*(VOID) CONST;//获取相应缓冲区
		operator int*(VOID) CONST;//获取相应缓冲区
		operator WORD*(VOID) CONST;//获取相应缓冲区
		//以下五个成员函数用于快速改变文件指针读取操作
		VOID BeginBatchRead(VOID);
		VOID EndBatchRead(VOID);
		VOID BeforeSeek();//记录当前位置以便还原位置
		VOID SeekFromOrigin(FORMAT_SFILESIZE delta);//从记录位置为开始移动内存指针，读取数据
		VOID AfterSeek();//还原到记录位置的数据
	CLASS_END_DEFMEMFUNC

	CLASS_BEGIN_DEFMEMBERVAR
		FORMAT_SFILESIZE m_dwBlockBytes;
	CLASS_END_DEFMEMBERVAR

private:
	CLASS_BEGIN_DEFMEMBERVAR
		FORMAT_DATA* m_Data;
		FORMAT_FILEPOSITION m_Base;
		FORMAT_FILEPOSITION m_CurrentPos;//m_CurrentPos < m_dwBlockBytes
		CONST FORMAT_DISKHANDLE m_DiskHandle;
		FORMAT_DISKSIZE m_DiskSize;
		BOOL m_IsCurrentBufBuf1;
		//以下三个成员变量用于快速改变文件指针读取操作
		FORMAT_FILEPOSITION m_oBase;
		FORMAT_FILEPOSITION m_oCurrentPos;
		FORMAT_DATA* m_oData;
	CLASS_END_DEFMEMBERVAR
}FORMAT_DATAPOINTER;

typedef CONST FORMAT_SFILESIZE (*FORMAT_DLLFUNC_GETFILELENIMMEDATELY)(FORMAT_DATA*);

struct FORMAT_FILEINFO
{
	BYTE cbSize;
	BYTE bitmap;//指定哪些成员变量有效，有效位在成员后标示
	FORMAT_FILESIZE filesize;//0x01
	CHAR fileextDeepscan[MAX_EXTLEN];//0x02
	BOOL IsFileValid;//0x04
	CHAR Description[MAX_DESCIPTION];//0x08
	PVOID reserved;//用于扩展
};

typedef BOOL (*FORMAT_DLLFUNC_GETRAWFILEINFO)(FORMAT_DATAPOINTER&,FORMAT_FILEINFO*);
struct FORMAT_FILETYPEINFO
{//从dll中获取信息，各个类型已在common.h中讨论过
	INT cbSize;
	BOOL IsFileSigSimple;
	FORMAT_DLLHANDLE DllHandle;
	BOOL CanGetFileLenImmediately;
	FORMAT_GETSUPPORTEXT GetSupportExtension;
	FORMAT_DLLFUNC_GETRAWFILEINFO GetRawFileInfo;
	FORMAT_DLLFUNC_GETFILELENIMMEDATELY GetFileLenFast;//必须返回小文件长度
	PVOID reserved;
};

struct FORMAT_COMPLEXFILESIG
{//复杂文件签名
	FORMAT_FILESIGID Id;
	FORMAT_GETCOMPLEXFILETYPE GetComplexFileType;
};

//CListCtrl显示标题： 0:序号(文件名) 1:文件类型 2:文件大小 3:是否有效
struct StorageData
{//记录数据用于深度搜索和数据恢复
	FORMAT_FILESIGID Id;
	FORMAT_FILEPOSITION Base;
	FORMAT_FILEPOSITION Offset;
	FORMAT_FILESIZE FileSize;
	CHAR Desciption[MAX_DESCIPTION];
	BOOL IsValid;
};

class CFormatRecovery
{//一个类绑定一个Disk HANDLE
public:
	CLASS_BEGIN_DEFMEMFUNC
		CFormatRecovery(FORMAT_DISKHANDLE,CListCtrl&,CProgressCtrl&);
		virtual ~CFormatRecovery();
		BOOL	AnalysisDisk(VOID);//分析磁盘，开始遍历磁盘
		BOOL	DoGetInfo(CString& result) CONST;//分析结束后获取dll返回的文件扩展信息
		BOOL	DoRecovery(CONST CString& RecoverDir) CONST;//恢复选中文件
		FORMAT_FILESIGID FastGetSpecFileExt(IN CONST FORMAT_DATA DiskInfo[MAX_FILESIGLEN],OUT CString&) CONST;//通过签名树和复杂签名函数得到初步检测结果
		FORMAT_ERRORCODE GetErrorCode(VOID) CONST;//预留差错控制接口
		VOID	GetSupportFileExtension(OUT CList<CString,CString&>&) CONST;//获取当前软件支持的扩展类全集
		VOID	Initialize(IN CList<CString,CString&>&);//参数为需要查找的扩展名链表
		VOID	Stop(VOID);//停止初步文件搜索
		BOOL	IsSearching(VOID);
	CLASS_END_DEFMEMFUNC

	CLASS_BEGIN_DEFMEMBERVAR
		CList<FORMAT_FILETYPEINFO,FORMAT_FILETYPEINFO&> m_FileInfo;
		CListCtrl& m_ListCtrl;//SetItemData存放大小和位置用于恢复
	CLASS_END_DEFMEMBERVAR
private:
	CLASS_BEGIN_DEFMEMFUNC	
		BOOL DeepAnalysisFile(IN FORMAT_DATAPOINTER&,FORMAT_FILESIGID,OUT FORMAT_FILEINFO*) CONST;//通过Dll GetRawFileInfo得到深度检测结果
		VOID SetErrorCode(FORMAT_ERRORCODE);//预留差错控制接口
	CLASS_END_DEFMEMFUNC

	CLASS_BEGIN_DEFMEMBERVAR
		BOOL m_Stop;
		BOOL m_IsSearching;
		INT m_DllNum;
		CFileSigTree m_SigTree;
		CProgressCtrl& m_ProgressCtrl;
		FORMAT_ERRORCODE m_ErrorCode;
		FORMAT_DATATYPE m_CurrentType;
		CONST FORMAT_DISKHANDLE m_DiskHandle;
		CList<FORMAT_COMPLEXFILESIG,FORMAT_COMPLEXFILESIG&> m_ComplexFileSig;
	CLASS_END_DEFMEMBERVAR
};
#endif

//用于排列列表控件的通用宏
#define MakeCompareFunc(Class,ListCtrl) \
	namespace Class##SPACE	\
	{	\
		int CALLBACK CompareFuncUp(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)	\
		{	\
			Class* pdlg=(Class*)lParamSort;	\
			if(pdlg->col == 0)	\
				return atoi(pdlg->ListCtrl.GetItemText(lParam1,pdlg->col)) < atoi(pdlg->ListCtrl.GetItemText(lParam2,pdlg->col));\
			else if(pdlg->col == 2)	\
				return _atoi64(pdlg->ListCtrl.GetItemText(lParam1,pdlg->col)) < _atoi64(pdlg->ListCtrl.GetItemText(lParam2,pdlg->col));\
			else	\
				return pdlg->ListCtrl.GetItemText(lParam1,pdlg->col) < pdlg->ListCtrl.GetItemText(lParam2,pdlg->col);	\
		}	\
		int CALLBACK CompareFuncDown(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)	\
		{	\
			Class* pdlg=(Class*)lParamSort;	\
			if(pdlg->col == 0)	\
				return atoi(pdlg->ListCtrl.GetItemText(lParam1,pdlg->col)) > atoi(pdlg->ListCtrl.GetItemText(lParam2,pdlg->col));\
			else if(pdlg->col == 2)	\
				return _atoi64(pdlg->ListCtrl.GetItemText(lParam1,pdlg->col)) > _atoi64(pdlg->ListCtrl.GetItemText(lParam2,pdlg->col));\
			else	\
				return pdlg->ListCtrl.GetItemText(lParam1,pdlg->col) > pdlg->ListCtrl.GetItemText(lParam2,pdlg->col);	\
		}	\
	};\
	using namespace Class##SPACE;
