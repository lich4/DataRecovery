#pragma pack(push, curAlignment)
#pragma pack(1)

struct FILEMAP_ENTRY
{
	char dwSig[4];	//条目标识
	DWORD nBlocks;	//条目占用多少个块(128字节)
};

struct URL_LEAK:FILEMAP_ENTRY
{
    LONGLONG	LastModifiedTime;		//最后修改时间
    LONGLONG	LastAccessedTime;		//最后访问时间
    DWORD		dostExpireTime;			//到期时间
    DWORD		dostPostCheckTime;		
    DWORD		dwFileSize;				//硬盘缓存中的文件的大小
    DWORD		dwRedirHashItemOffset;	// ask DanpoZ
    DWORD		dwGroupOffset;
    DWORD		data;
    DWORD    CopySize;					// 好像总是0x60
    DWORD    UrlNameOffset;				// URL名称偏移。基地址是本URL条目的开始地址
    BYTE     DirIndex;					// 属于的子文件夹索引
    BYTE     bSyncState;				// automatic sync mode state
    BYTE     bVerCreate;				// 建立本ENTRY的CACHE的版本
    BYTE     bVerUpdate;				// 升级本ENTRY的CACHE的版本
	DWORD    InternalFileNameOffset;	//硬盘上文件名（不包括目录）字符串的偏移，基地址是本URL条目的开始地址。
    DWORD    CacheEntryType;			//缓存类型
    DWORD    HeaderInfoOffset;			//从WEB服务器中取本文件时的返回的HTTP头部信息
    DWORD    HeaderInfoSize;			//和大小（注意包括最后的回车换行的）
    DWORD    FileExtensionOffset;		// should be WORD
    DWORD    dostLastSyncTime;       
    DWORD    NumAccessed;				// 存取次数（点击率）
    DWORD    NumReferences;				// 引用次数
    DWORD    dostFileCreationTime;		// 好像是ULONG？
};

struct REDIR:FILEMAP_ENTRY
{
	DWORD dwItemOffset;					// offset to hash table item of destination URL
	DWORD dwHashValue;					// destination URL hash value (BUGBUG: collisions?)
};

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

#pragma pack(pop, curAlignment)