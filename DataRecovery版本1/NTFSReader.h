// NTFSReader.h: interface for the NTFSReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NTFSREADER_H__18B867A1_115A_44E1_9611_B861FB5549C2__INCLUDED_)
#define AFX_NTFSREADER_H__18B867A1_115A_44E1_9611_B861FB5549C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

#pragma pack(push, curAlignment)
#pragma pack(1)

struct NTFS//NTFS引导扇区
{
	char		chJumpInstruction[3];//跳转指令
	char		chOemID[4];//改行和下行表示的是文件系统，内容是"NTFS"。
	char		chDummy[4];//四个空格
	
	struct NTFS_BPB
	{
		WORD		wBytesPerSec;//每扇区字节数
		BYTE		uchSecPerClust;//每簇扇区数
		WORD		wReservedSec;//保留扇区
		BYTE		uchReserved[3];//总是0
		WORD		wUnused1;//未使用 为0
		BYTE		uchMediaDescriptor;//介质描述
		WORD		wUnused2;//未使用 为0
		WORD		wSecPerTrack;//每磁道扇区数目
		WORD		wNumberOfHeads;//每柱面磁头数
		DWORD		dwHiddenSec;//隐含扇区
		DWORD		dwUnused3;//未使用 为0
		DWORD		dwUnused4;//未使用 为80008000
		LONGLONG	n64TotalSec;//扇区总数
		LONGLONG	n64MFTLogicalClustNum;//$MFT的起始逻辑簇号
		LONGLONG	n64MFTMirrLogicalClustNum;//$MFTMirr的起始逻辑簇号
		int			nClustPerMFTRecord;//文件记录的大小描述
		int			nClustPerIndexRecord;//索引缓冲的大小描述
		LONGLONG	n64VolumeSerialNum;//分区的逻辑序列号，此处为格式化时随机产生
		DWORD		dwChecksum;//校验和
	} bpb;
	
	char		chBootstrapCode[426];//启动代码
	WORD		wSecMark;//引导扇区结束标志55AA
};

struct MFT_HEAD
{//MFT头信息
	char		szSignature[4];		//"FILE"
	WORD		wFixupOffset;		//更新序列号的偏移
	WORD		wFixupSize;			//更新序列号的大小与数组
	LONGLONG	n64LogSeqNumber;	//日志文件序列号
	WORD		wSequence;			//序列号
	WORD		wHardLinks;			//硬链接数
	WORD		wAttribOffset;		//第一个属性的偏移地址
	WORD		wFlags;				//标志位:00H文件被删除 01H记录正在使用 02H该记录为目录
	DWORD		dwRecLength;		//文件记录的实际长度
	DWORD		dwAllLength;		//总共分配给记录的长度
	LONGLONG	n64BaseMftRec;		//基本文件记录中的文件索引号
	WORD		wNextAttrID;		//下一属性ID
	WORD		wFixupPattern;		//边界
	DWORD		dwMFTRecNumber;		//MFT记录编号
	WORD		wFixup;				//更新序列号
	WORD		wFixupMatrix[3];	//更新序列数组
	//接着是10H-F0H属性:NTFS_ATTRIBUTE
};

struct NTFS_ATTRIBUTE	//四种属性头
{
	DWORD	dwType;					//属性类型			
	DWORD	dwFullLength;			//包括属性头在内的本属性长度
	BYTE	uchNonResFlag;			//0为常驻 其余为非常驻
	BYTE	uchNameLength;			//属性名长度
	WORD	wNameOffset;			//该属性开始的偏移
	WORD	wFlags;					//加密压缩稀疏标志
	WORD	wID;					//属性ID
	
	union ATTR
	{
		struct RESIDENT
		{//常驻
			DWORD	dwLength;		//该属性的长度
			WORD	wAttrOffset;	//该属性的偏移
			BYTE	uchIndexedTag;	//索引标志
			BYTE	uchPadding;		//无意义
		}Resident;//接着是(属性名+)属性内容:NTFS10H-NTFSF0H
		
		struct NONRESIDENT
		{//非常驻
			LONGLONG	n64StartVCN;		//最开始的虚拟簇号
			LONGLONG	n64EndVCN;			//最后的虚拟簇号
			WORD		wDatarunOffset;		//数据运行信息的偏移地址
			WORD		wCompressionSize;	//压缩单位大小
			BYTE		uchPadding[4];		//无意义
			LONGLONG	n64AllocSize;		//属性分配的大小
			LONGLONG	n64RealSize;		//属性真实的大小
			LONGLONG	n64StreamSize;		//最初的属性大小
			//接着是(属性名+)数据运行的内容
		}NonResident;
	}Attr;
};

struct STANDARD_INFORMATION
{//10H类型属性 $STANDARD_INFORMATION
	LONGLONG	n64Create;		//文件建立时间
	LONGLONG	n64Modify;		//文件最后修改时间
	LONGLONG	n64Modfil;		//MFT修改时间
	LONGLONG	n64Access;		//文件最后访问时间
	DWORD		dwFATAttributes;//传统文件属性
	DWORD		dwMaxversion;	//最大版本数
	DWORD		dwVersion;		//版本数
	DWORD		dwID;			//分类ID
	DWORD		dwOwnerID;		//所有者ID
	DWORD		dwSafeID;		//安全ID
	LONGLONG	dwQuota;		//配额管理
	LONGLONG	dwSerial;		//更新序列号
} ;

struct FILE_NAME
{//30H类型属性 $FILE_NAME 
	LONGLONG	dwMftParentDir; //父目录的文件参考号
	LONGLONG	n64Create;      //文件创建时间
	LONGLONG	n64Modify;      //文件修改时间
	LONGLONG	n64Modfil;      //MFT修改时间
	LONGLONG	n64Access;      //文件最后访问时间
	LONGLONG	n64Allocated;   //文件分配大小
	LONGLONG	n64RealSize;    //文件实际大小
	DWORD		dwFlags;		//标志位
	DWORD		dwEAsReparsTag;	//$EA $REPARSE_POINT
	BYTE		chFileNameLength;//文件名长度
	BYTE		chFileNameType;  //文件名命名空间	
};

struct FAT32//NTFS引导扇区
{
	char		chJumpInstruction[3];//跳转指令
	char		chOemID[8];//改行和下行表示的是文件系统，内容是"NTFS"。
	
	struct FAT32_BPB
	{
		WORD		wBytesPerSec;//每扇区字节数
		BYTE		uchSecPerClust;//每簇扇区数
		WORD		wReservedSec;//保留扇区数
		BYTE		wFATNum;//FAT数
		WORD		wRootEntries;//根目录项数
		WORD		wSmallSector;//小扇区数
		BYTE		uchMediaDescriptor;//介质描述
		WORD		FATPerSec;//每FAT扇区数
		WORD		wSecPerTrack;//每磁道扇区数目
		WORD		wNumberOfHeads;//每柱面磁头数
		DWORD		dwHiddenSec;//隐藏扇区
		DWORD		dwTotalSec;//总扇区数
		DWORD		dwSecPerFat;//每FAT扇区数
		WORD		dwExtFlag;//扩展标志
		WORD		dwFSVersion;//文件系统版本
		DWORD		dwRootClustNum;//根目录簇号
		WORD		dwSectorNumber;//文件系统信息扇区号
		WORD		dwBackUpSec;//备份引导扇区
		BYTE		Reserved[12];
	} bpb;
	
	struct FAT32_EXTBPB
	{
		BYTE		DriveNum;//物理驱动器号
		BYTE		Reserved;//保留
		BYTE		ExtBootSign;//扩展引导标签
		DWORD		VolSerialNum;//分区序号
		char		VolLabel[11];//卷标
		char		SysID[8];//系统ID
	} extbpb;
	char		chBootstrapCode[420];//启动代码
	WORD		wSecMark;//引导扇区结束标志55AA
};

union FAT32_DIR
{
	struct SHORTDIR//短文件名目录
	{
		char	dwFileName[8];//文件的文件名
		char	dwFileExt[3];//文件的扩展名
		BYTE	dwFileAttrib;//文件属性
		WORD	Reserved;//保留未用
		WORD	dwCreateTime;//文件创建时间
		WORD	dwLastAccessDate;//文件最后访问日期
		WORD	dwCreateDate;//文件创建日期
		WORD	dwHigh16;//文件起始簇号的高16位
		WORD	dwSysLastModifyTime;//系统最后修改时间
		WORD	dwSysLastModifyDate;//系统最后修改的日期
		WORD	dwLow16;//文件起始簇号的低16位
		DWORD	dwFileSize;//文件的长度(Byte)
	} shortdir;

	struct LONGDIR//短文件名目录
	{
		BYTE	dwFlag;//标志位
		char	namepart1[10];//文件名第一部分
		WORD	flag1;//长文件名为OFH
		BYTE	Serial;//目录项标号
		char	namepart2[12];//文件名第二部分
		WORD	Flag;//总为0
		char	namepart3[4];//文件名的第三部分
	} longdir;
};

struct PartitionTable
{
	BYTE	BootIndicator;
	BYTE	StartingHead;
	WORD	StartData;
	BYTE	PartitionID;
	BYTE	EndingHead;
	WORD	EndData;
	DWORD	RelativeSectors;
	DWORD	TotalSectors;
};

#pragma pack(pop, curAlignment)

struct TableList
{
	PartitionTable	Table;
	TableList*		Next;
	BOOL			IsPartValid;
	TableList()
	{
		Next=NULL;
	}
};

struct FILESTRUCT
{
	CString		name;
	CString		DATA[14];
	int			MFT;
	int			FATHERMFT;
	HTREEITEM	treeitem;

	FILESTRUCT()
	{
		treeitem=NULL;
	}
	virtual ~FILESTRUCT()
	{
		if(DATA[13]!="")
		{
			int RunNum;
			sscanf(DATA[13],"%d",&RunNum);
			delete (CString*)RunNum;
		}
	}
};

struct DirTreeNode
{
	CString			name;
	DirTreeNode*	nextchild;//第一个兄弟节点
	HTREEITEM		m_TreeItem;
	int				selfMFT;
	int				serial;

	DirTreeNode() 
	{
		nextchild=NULL;
		name="";
		m_TreeItem=NULL;
		selfMFT=-1;
		serial=-1;
	}
};

struct LostFile
{
	CString		name;
	int			FatherMFT;
	int			SelfMFT;
	LostFile*	next;
	int			serial;

	LostFile()
	{
		name="";
		FatherMFT=-1;
		SelfMFT=-1;
		next=NULL;
		serial=-1;
	}
};

class DirTree
{//散列树
	private:
		DirTreeNode*	root;
		LostFile*		stillnotfind;//若发现文件MFT超过散列树范围则加入此树
		CTreeCtrl*		m_TreeCtrl;
		CListCtrl*		m_ListCtrl;
		int				TotalNum;
	public:
		DirTree(int num);
		virtual ~DirTree();
		void AddFileToTree(int self,int father,int serial,CString& name);
		void AddToCtrl(CTreeCtrl* Tree,CListCtrl* List);//将当前子树加入CTreeCtrl
	private:
		void AddToCtrlEx(HTREEITEM cur,int serial);
};

class NTFSReader
{
	NTFSReader();
	virtual ~NTFSReader();
};
