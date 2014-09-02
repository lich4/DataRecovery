//由于该项目由于vc6的bug，不得不在vs2010编译

//此格式化恢复工具分几步：
//①程序搜索ext文件夹下的dll，dll的GetSupportExtension提供该dll能识别的文件类型，主模块将所有支持的类型给用户显示。
//②程序列出所有支持的(介质)磁盘类型，目前支持本地硬盘、移动硬盘、u盘、以后可能支持内存文件恢复，列出给用户显示的介质是各个非隐藏分区和所有硬盘的设备链接符号。
//③用户选择需要搜索的文件类型和要恢复的介质，点击搜索开始搜索。
//④主模块将用户选择的文件类型加入搜索，其中对于简单文件签名直接加入搜索树中，对于复杂文件签名则把函数地址加入一个链表。
//以下为初步文件搜索：
//⑤主模块打开设备，从0扇区开始搜索，先搜索简单签名树，如果没有则在复杂文件签名列表中搜索，如果都没有直接进入下一个扇区。如果找到则跳转到⑥。
//⑥将搜索到的初步信息显示到列表控件，如果能快速获取文件大小则可以加速遍历，否则按簇遍历(每次向后0x1000)，记录每个找到的文件相关信息用于后面深度解析。
//以下为深度文件搜索：
//⑦对于搜到的每一项获取信息，定位到文件，把文件数据交给dll获取详细信息，这些信息包括精确后缀名，有效性，长度，和文件的特定信息。
//⑧深度文件搜索完成后，右键点击列表控件会弹出2个菜单，一个是恢复，点击就开始恢复所有选择的文件，文件名为序号，后缀为识别出来的后缀，目录为选择的目录。
//另一个菜单是查看文件信息，选择后可以查看dll在深度搜索时提供的特定文件信息

/*
Dll导出函数要求：

//告知主模块该dll对于给定buf(已验证过文件签名)是否能提供快速返回文件大小，和GetFileLenImmediately配套使用可以在初次文件格式识别中加快扇区遍历速度。
BOOL	CanGetFileLenImmediately;

//此函数对应于CanGetFileLenImmediately，如果CanGetFileLenImmediately设为TRUE则应该提供此函数入口快速计算文件大小。用于初步文件搜索。
FORMAT_SFILESIZE GetFileLenImmediately(VOID);

//告知注模块文件签名是否简单(可以通过判断连续几个字符确定)，此导出值需要和GetFileSig连用以获取文件签名信息
BOOL	IsFileSigSimple;

//此函数用于获取dll中的文件签名信息，如果文件签名可以简单对比出来就直接将签名的那些字节考入FORMAT_SIMPLETYPE，设置相关信息即可，
//如果该种文件签名不可以通过简单逐头字节对照比较出来，则应提供文件签名判断的入口函数，其类型为FORMAT_GETCOMPLEXFILETYPE
BOOL	GetFileSig(PVOID pData);//simplefile:pData=FORMAT_SIMPLETYPE* complexfile:pData=FORMAT_GETCOMPLEXFILETYPE*

//用于深度文件搜索，获取文件大小及扩展信息,获取唯一文件后缀
BOOL	GetRawFileInfo(CONST FORMAT_DATAPOINTER pData,FORMAT_FILEINFO* pfileinfo);

//提供该dll支持的文件类型，可以返回文件描述，也可以返回类型后缀，不要含有不能作为文件名的非法字符
VOID	GetSupportExtension(CHAR Ext[MAX_EXTLEN]);
*/

//我想到的优化法：①双缓冲区，充分利用io操作时间分给cpu并行处理内存数据
//②深度搜索查找占用时间太长可以利用列表项后一项位置为界，搜索超过此位置仍没结束(获取到大小和后缀)则强制结束

#include <windows.h>
#include <winioctl.h>
#include <assert.h>
#include <process.h>

#define IOCTL_DISK_GET_LENGTH_INFO CTL_CODE(IOCTL_DISK_BASE, 0x0017, METHOD_BUFFERED, FILE_READ_ACCESS)

// typedef struct _GET_LENGTH_INFORMATION 
// {
//     LARGE_INTEGER   Length;
// }GET_LENGTH_INFORMATION, *PGET_LENGTH_INFORMATION;

#ifndef FORMAT_COMMON	
#define FORMAT_COMMON

//以下是我定义的，详细说明在FormatDefs.cpp和FormatDefs.h，这里只是挑出这2个文件里必备的声明和定义用于编写windows dll，
//由于和windows和mfc不兼容，没有直接用上面2个文件
#define FORMAT_API			WINAPI
#define MAX_FILESIGLEN		0x100
#define MAX_CHILDNUM		0x100
#define FORMAT_FAILURE		(DWORD)-1
#define FORMAT_INVALIDID	(DWORD)-1
#define TREE_ADDFAILURE		FALSE
#define TREE_ADDSUCCESS		TRUE
#define MAX_EXTLEN			0x100
#define MAX_DESCIPTION		0x1000

#define CLASS_BEGIN_DEFINETYPE//定义类结构
#define CLASS_END_DEFINETYPE
#define CLASS_BEGIN_DEFMEMBERVAR//定义类成员变量
#define CLASS_END_DEFMEMBERVAR
#define CLASS_BEGIN_DEFMEMFUNC//定义类成员函数
#define CLASS_END_DEFMEMFUNC

typedef LPCTSTR				FORMAT_EXTENSION;	
typedef BYTE				FORMAT_DATA;				
typedef DWORD				FORMAT_SFILESIZE;
typedef LONGLONG			FORMAT_FILESIZE;
typedef LONGLONG			FORMAT_DISKSIZE;
typedef LONGLONG			FORMAT_FILEPOSITION;
typedef HMODULE				FORMAT_DLLHANDLE;
typedef HANDLE				FORMAT_DISKHANDLE;
typedef INT					FORMAT_DATATYPE;
typedef INT					FORMAT_FILESIGLEN;
typedef BOOL				FORMAT_ISSUCCESS;
typedef INT					FORMAT_FILESIGID;//唯一对应Dll的序数，唯一对应一个Sig
typedef INT					FORMAT_ERRORCODE;

struct FORMAT_FILEINFO
{
	BYTE cbSize;
	BYTE bitmap;//获取了哪些其他成员变量
	FORMAT_FILESIZE filesize;//0x01
	CHAR fileextDeepscan[MAX_EXTLEN];//0x02
	BOOL IsFileValid;//0x04
	CHAR Description[MAX_DESCIPTION];//0x08
	PVOID reserved;
};

struct FORMAT_SIMPLETYPE
{
	FORMAT_DATA FileSig[MAX_FILESIGLEN];
	FORMAT_FILESIGLEN FileSigLen;//AddTreeTrunk调用之前检测参数
};
typedef BOOL (*FORMAT_GETCOMPLEXFILETYPE)(CONST FORMAT_DATA DiskInfo[MAX_FILESIGLEN]);

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

CDiskMappingPointer::CDiskMappingPointer(FORMAT_DISKHANDLE hDisk,FORMAT_FILEPOSITION Base):m_DiskHandle(hDisk),m_Base(((Base)>>12)<<12)
{
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	m_dwBlockBytes=1000*dwGran;
	m_Data=new FORMAT_DATA[m_dwBlockBytes*2];//双缓冲区
	if(m_Data == NULL)
	{
		MessageBox(NULL,"内存不够","内存不够",MB_OK);
		exit(-1);
	}
	LARGE_INTEGER li;
	li.QuadPart=m_Base;

	//获取指定头部数据
	SetFilePointer(m_DiskHandle,li.LowPart,&li.HighPart,FILE_BEGIN);
	DWORD retlen;
	ReadFile(m_DiskHandle,m_Data,m_dwBlockBytes,&retlen,NULL);//填充第一块缓冲区
	m_IsCurrentBufBuf1=TRUE;
	//获取磁盘大小
	m_DiskSize=0;
	GET_LENGTH_INFORMATION getlen;
	if(DeviceIoControl(hDisk,IOCTL_DISK_GET_LENGTH_INFO,NULL,0,(LPVOID)&getlen,sizeof(getlen),&retlen,NULL))
		m_DiskSize=getlen.Length.QuadPart;
	m_CurrentPos=(Base&0xFFF);
}

CDiskMappingPointer::CDiskMappingPointer(CONST CDiskMappingPointer& ObjPtr):m_Base(ObjPtr.m_Base),m_DiskHandle(ObjPtr.m_DiskHandle)
{//深拷贝对象
	m_dwBlockBytes=ObjPtr.m_dwBlockBytes;
	m_CurrentPos=ObjPtr.m_CurrentPos;
	m_DiskSize=ObjPtr.m_DiskSize;
	m_Data=new FORMAT_DATA[m_dwBlockBytes];
	memcpy(m_Data,ObjPtr.m_Data,m_dwBlockBytes);
}

CDiskMappingPointer::~CDiskMappingPointer()
{
	delete []m_Data;
}

FORMAT_DATA CDiskMappingPointer::operator [](FORMAT_SFILESIZE delta) CONST//调用者需要保证参数合法
{
	return m_Data[m_CurrentPos+delta];
}

VOID CDiskMappingPointer::operator+=(FORMAT_SFILESIZE delta)//调用者需要保证参数合法
{//尝试修改相对位置指针
	SetDiskPointer(m_Base+m_CurrentPos+delta);
}

FORMAT_DATA CDiskMappingPointer::operator*() CONST
{
	return m_Data[m_CurrentPos];
}

CDiskMappingPointer::operator FORMAT_DATA*() CONST
{//强制转换为FORMAT_DATA*
	return (m_Data+m_CurrentPos);
}

CDiskMappingPointer::operator DWORD*() CONST
{//强制转换为DWORD*
	return (DWORD*)(m_Data+m_CurrentPos);
}

CDiskMappingPointer::operator int*() CONST
{//强制转换为int*
	return (int*)(m_Data+m_CurrentPos);
}

CDiskMappingPointer::operator WORD*() CONST
{//强制转换为WORD*
	return (WORD*)(m_Data+m_CurrentPos);
}

VOID CDiskMappingPointer::SwapBuf()
{
	if(!m_IsCurrentBufBuf1)
		m_Data = m_oData;//若当前为第二个缓冲区就交换到第一个缓冲区
	else
		m_Data = m_oData+m_dwBlockBytes;//若当前为第一个缓冲区就交换到第二个缓冲区
	m_Base += m_dwBlockBytes;
	DWORD retlen;
	ReadFile(m_DiskHandle,m_Data,m_dwBlockBytes,&retlen,NULL);
	m_IsCurrentBufBuf1 = !m_IsCurrentBufBuf1;
}

VOID CDiskMappingPointer::BeginBatchRead()
{
	m_IsCurrentBufBuf1=TRUE;
	m_oData=m_Data;
}

VOID CDiskMappingPointer::EndBatchRead()
{
	m_IsCurrentBufBuf1=TRUE;
	m_Data=m_oData;
}

VOID CDiskMappingPointer::BeforeSeek()
{
	m_oBase=m_Base;
	m_oCurrentPos=m_CurrentPos;
}

VOID CDiskMappingPointer::SeekFromOrigin(FORMAT_SFILESIZE delta)
{
	if(m_oCurrentPos+delta < m_dwBlockBytes/2 && m_Base == m_oBase)
	{
	}
	else//如果指针发生过移动
	{//移回原位置并读取数据
		SetDiskPointer(m_oBase+m_oCurrentPos+delta);
	}
}

VOID CDiskMappingPointer::AfterSeek()
{//恢复指针及数据
	if(m_Base != m_oBase)
	{
		SetDiskPointer(m_oBase+m_oCurrentPos);
	}
	m_Base=m_oBase;
	m_CurrentPos=m_oCurrentPos;
}

int CDiskMappingPointer::GetProgress()
{
	return int(1000*(float)(m_CurrentPos+m_Base)/m_DiskSize);
}

BOOL CDiskMappingPointer::IsEndOfDisk() CONST
{
	return (m_Base+m_CurrentPos) > m_DiskSize;
}

BOOL CDiskMappingPointer::SetDiskPointer(FORMAT_FILEPOSITION pos)
{//设置绝对位置指针
	if(pos>m_DiskSize)
	{
		m_CurrentPos=pos-m_Base;
		return FALSE;
	}

	FORMAT_FILEPOSITION newpos=((pos>>12)<<12);//按簇为单位计算基址
	if(m_Base <= newpos && newpos <= m_Base+m_dwBlockBytes/2-0x1000 && m_CurrentPos < m_dwBlockBytes/2)
	{
		m_CurrentPos=pos-m_Base;
	}
	else
	{
		m_Base=((pos>>12)<<12);
		m_CurrentPos=(pos&0xFFF);//计算偏移，也就是去掉了基址的那部分
		LARGE_INTEGER li;
		li.QuadPart=m_Base;
		SetFilePointer(m_DiskHandle,li.LowPart,&li.HighPart,FILE_BEGIN);
		DWORD retlen;
		ReadFile(m_DiskHandle,m_Data,m_dwBlockBytes,&retlen,NULL);
	}

	return TRUE;
}

FORMAT_DISKSIZE CDiskMappingPointer::GetDiskSize() CONST
{
	return m_DiskSize;
}

#endif

