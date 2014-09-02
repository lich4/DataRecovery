// #define REG_NONE						0//没有固定格式
// #define REG_SZ							1//固定长度字符串
// #define REG_EXPAND_SZ					2//可变长度字符串
// #define REG_BINARY						3//二进制数据
// #define REG_DWORD						4//32 位数字
// #define REG_DWORD_BIG_ENDIAN			5//32 位数字
// #define REG_LINK						6//Unicode 字符串
// #define REG_MULTI_SZ					7//多个字符串
// #define REG_RESOURCE_LIST				8//二进制数据
// #define REG_FULL_RESOURCE_DESCRIPTION	9//二进制数据
// #define REG_RESOURCE_REQUIREMENTS_LIST	10//二进制数据
// #define REG_QWORD						11//64 位数字

#pragma pack(push, curAlignment)
#pragma pack(1)

struct REGF//基本块
{
	char		Signature[4];	//"regf"
	DWORD		Sequence1;		//更新序列号
	DWORD		Sequence2;		//更新序列号
	LONGLONG	LastWriteTime;	//最后一次写操作发生时间
	DWORD		Version1;		//主版本号
	DWORD		Version2;		//次版本号
	DWORD		Type;
	DWORD		Format;
	DWORD		RootCell;		//第一个键记录的偏移
	DWORD		Lenght;			//数据块长度
	DWORD		Cluster;
	char		name[64];		//储巢文件名
	DWORD		Reserved1[99];
	DWORD		CheckSum;		//校验和
	DWORD		Reserved2[894];
	DWORD		BootType;
	DWORD		BootRecover;
};

struct HBIN//巢箱
{
	char		Signature[4];	//"hbin"
	DWORD		FileOffset;		//本巢箱相对第一个巢箱起始的偏移
	DWORD		Size;			//本巢箱的大小
	DWORD		Reserved[2];
	LONGLONG	LastWreiteTime;	//最后一次写操作发生时间
	DWORD		Spare;
};
//巢室可以容纳：键、值、安全描述符、一列子键、一列键值
struct  Key_Cell//键巢室
{
	DWORD		Size;				//100000000-Size为大小
	char		Signature[2];		//"nk"
	WORD		Flags;				//根键:0x2C 0xAC 其他:0x20
	LONGLONG	LastWriteTime;		//最后一次写操作发生时间
	DWORD		Spare;
	DWORD		Parent;				//父键的偏移
	DWORD		SubKeyCounts[2];	//[0]为子键的个数
	DWORD		SubKeyList[2];		//[0]为子键列表相对"首个"BIN的偏移
	DWORD		Count;				//包含值的个数
	DWORD		OffsetToList;		//到值列表的偏移量
	DWORD		OffsetToSecurity;	//安全描述符记录的偏移
	DWORD		Class;				//类名的偏移
	DWORD		Reserved[5];
	WORD		NameLength;			//键名长度
	WORD		ClassLength;		//类名长度
	//接键名称
};

struct	KeyNode//值巢室
{
	DWORD	Size;			//大小
	char	Signature[2];	//"vk"
	WORD	NameLength;		//值名称长度
	WORD	DataLength;		//值数据长度
	BYTE	Reserved;
	BYTE	DataType;		//数据类型
	DWORD	Data;			//导致数据的偏移或值数据  若DataLength高位为1为数据，DataLength&0x7FFFFFFF为数据长度
	DWORD	Type;			//值类型
	WORD	Flags;			//是否被命名字段,0x0:值记录不包括一个名称
	WORD	Spare;
	//接值名称或 {DWORD 大小;大小 值数据}
};

struct	KeyChildList//子键列表巢室
{
	DWORD	Size;			//大小
	char	Signature[2];		//ri,li,lf,lh
	WORD	Count;			//子项个数
};//lf,lh后接Count个InsideChild	  <==>   ri,li后接Count个InsideChild.offset

struct InsideChild
{
	DWORD	offset;			//到子项的偏移量
	DWORD	CheckCode;		//子项名称的前4个字符或者校验码
};

struct	KeySecurity//安全描述符巢室
{
	DWORD	Size;
	WORD	Signature;		//"sk"
	WORD	Reserved;
	DWORD	Pre;			//上一个"sk"记录的偏移
	DWORD	Next;			//下一个"sk"记录的偏移
	DWORD	ReferenceCount;	//引用计数
	DWORD	Length;			//数据大小
	SECURITY_DESCRIPTOR_RELATIVE Descriptor;//数据
};

#pragma pack(pop, curAlignment)
