#include <windows.h>

#pragma pack(push)
#pragma pack(1)

// OFFSET              Count TYPE   Description
// 0000h                   4 char   ID='PK',03,04
// 0004h                   1 word   Version needed to extract archive
// 0006h                   1 word   General purpose bit field (bit mapped)
// 									  0 - file is encrypted
// 									  1 - 8K/4K sliding dictionary used
// 									  2 - 3/2 Shannon-Fano trees were used
// 									3-4 - unused
// 								   5-15 - used internally by ZIP
// 								 Note:  Bits 1 and 2 are undefined if the
// 										compression method is other than
// 										type 6 (Imploding).
// 0008h                   1 word   Compression method (see table 0010)
// 000Ah                   1 dword  Original DOS file date/time (see table 0009)
// 000Eh                   1 dword  32-bit CRC of file (inverse??)
// 0012h                   1 dword  Compressed file size
// 0016h                   1 dword  Uncompressed file size
// 001Ah                   1 word   Length of filename
// 								 ="LEN"
// 001Ch                   1 word   Length of extra field
// 								 ="XLN"
// 001Eh               "LEN" char   path/filename
// 001Eh               "XLN" char   extra field
// +"LEN"


// OFFSET              Count TYPE   Description
// 0000h                   4 char   ID='PK',01,02
// 0004h                   1 byte   Version made by
// 0005h                   1 byte   Host OS (see table 0011)
// 0006h                   1 byte   Minimum version needed to extract
// 0007h                   1 byte   Target OS
// 								 see above "Host OS"
// 0008h                   1 word   General purpose bit flag
// 								 see above "General purpose bit flag"
// 000Ah                   1 word   Compression method
// 								 see above "Compression method"
// 000Ch                   1 dword  DOS date / time of file
// 0010h                   1 dword  32-bit CRC of file (see table 0009)
// 0014h                   1 dword  Compressed size of file
// 0018h                   1 dword  Uncompressed size of file
// 001Ch                   1 word   Length of filename
// 								 ="LEN"
// 001Eh                   1 word   Length of extra field
// 								 ="XLN"
// 0020h                   1 word   Length of file comment
// 								 ="CMT"
// 0022h                   1 word   Disk number ??
// 0024h                   1 word   Internal file attributes (bit mapped)
// 									0 - file is apparently an ASCII/binary file
// 								 1-15 - unused
// 0026h                   1 dword  External file attributes (OS dependent)
// 002Ah                   1 dword  Relative offset of local header from the
// 								 start of the first disk this file appears on
// 002Eh               "LEN" char   Filename / path; should not contain a drive
// 								 or device letter, all slashes should be forward
// 								 slashes '/'.
// 002Eh+              "XLN" char   Extra field
// +"LEN"
// 002Eh               "CMT" char   File comment
// +"LEN"
// +"XLN"


// OFFSET              Count TYPE   Description
// 0000h                   4 char   ID='PK',05,06
// 0004h                   1 word   Number of this disk
// 0006h                   1 word   Number of disk with start of central directory
// 0008h                   1 word   Total number of file/path entries on this disk
// 000Ah                   1 word   Total number of entries in central dir
// 000Ch                   1 dword  Size of central directory
// 0010h                   1 dword  Offset of start of central directory relative
// 								 to starting disk number
// 0014h                   1 word   Archive comment length
// 								 ="CML"
// 0016h               "CML" char   Zip file comment
struct Open_XML_File_Header
{//文件头标记0x04034b50
	WORD ExtractVersion;//解压文件所需 pkware 版本
	WORD GeneralBit;//全局方式位标记
	WORD CompressMethod;//压缩方式
	DWORD LastModify;//最后修改文件日期时间
	DWORD CRC32Check;//CRC-32校验
	DWORD CompressedSize;//压缩后尺寸
	DWORD UncompressedSize;//未压缩尺寸
	WORD FilenameLen;//文件名长度
	WORD ExtraFieldLen;//扩展记录长度
	char FileName[0];//文件名
//	char ExtraField[0];//扩展字段
};

struct Open_XML_Dir_Header
{//目录中文件文件头标记0x02014b50
	BYTE ExtractVersion;//压缩使用的pkware 版本
	BYTE HostOS;//宿主系统
	BYTE UnExtractVersion;//解压文件所需 pkware 版本
	BYTE TargetOS;//目标系统
	WORD GeneralBit;////全局方式位标记
	WORD CompressMethod;//压缩方式
	DWORD LastModify;//最后修改文件日期时间
	DWORD CRC32Check;//CRC-32校验
	DWORD CompressedSize;//压缩后尺寸
	DWORD UnCompressedSize;//未压缩尺寸
	WORD FilenameLen;//文件名长度
	WORD ExtraFieldLen;//扩展记录长度
	WORD CommentLen;//文件注释长度
	WORD DiskNumber;//磁盘开始号
	WORD InternalAttrib;//内部文件属性
	DWORD ExternalAttrib;//外部文件属性
	DWORD RelativeOffset;//局部头部偏移量
	char FileName[0];//文件名
//	char ExtraField[0];//扩展字段
//	char Comment[0];//文件注释
};

struct Open_XML_Dir_End
{//目录结束标记0x06054b50
	WORD CurDiskNumber;//当前磁盘编号
	WORD DirBeginDiskNumber;//目录区开始磁盘编号
	WORD TotalRecOnDisk;//本磁盘上纪录总数
	WORD TotalRecInDir;//目录区中纪录总数
	DWORD DirFieldSize;//目录区尺寸大小
	DWORD DirOffsetToFirstDisk;//目录区对第一张磁盘的偏移量
	WORD CommentLen;//ZIP 文件注释长度
	char Comment[0];//ZIP 文件注释
};

struct PKZip_Open_XML
{
	DWORD Sig;
	union
	{
		Open_XML_File_Header PKZipFileHeader;
		Open_XML_Dir_Header PKZipCentralDirectoryHeader;
		Open_XML_Dir_End PKZipEndCentralDirectory;
	}Header;
};



#pragma pack(pop)

// 一个 ZIP 文件的普通格式
// 　　----------------------
// 　　 一个 ZIP 文件由三个部分组成：
// 　　 
// 　　 压缩源文件数据区+压缩源文件目录区+压缩源文件目录结束标志
// 　　
// 　　 1、压缩源文件数据区
// 　　 在这个数据区中每一个压缩的源文件/目录都是一条记录，记录的格式如下：
// 　　 
// 　　 [文件头+ 文件数据 + 数据描述符]
// 　　 a、文件头结构
// 　　 组成 长度
// 　　 文件头标记 4 bytes (0x04034b50)
// 　　 解压文件所需 pkware 版本 2 bytes
// 　　 全局方式位标记 2 bytes
// 　　 压缩方式 2 bytes
// 　　 最后修改文件时间 2 bytes
// 　　 最后修改文件日期 2 bytes
// 　　 CRC-32校验 4 bytes
// 　　 压缩后尺寸 4 bytes
// 　　 未压缩尺寸 4 bytes
// 　　 文件名长度 2 bytes
// 　　 扩展记录长度 2 bytes
// 　　 文件名 （不定长度）
// 　　 扩展字段 （不定长度）
// 　　 
// 　　 b、文件数据
// 　　 
// 　　 c、数据描述符
// 　　组成 长度
// 　　 CRC-32校验 4 bytes
// 　　 压缩后尺寸 4 bytes
// 　　 未压缩尺寸 4 bytes
// 　　 这个数据描述符只在全局方式位标记的第３位设为１时才存在（见后详解），紧接在压缩数据的最后一个字节后。这个数据描述符只用在不能对输出的 ZIP 文件进行检索时使用。例如：在一个不能检索的驱动器（如：磁带机上）上的 ZIP 文件中。如果是磁盘上的ZIP文件一般没有这个数据描述符。
// 　　
// 　　 2、压缩源文件目录区
// 　　 在这个数据区中每一条纪录对应在压缩源文件数据区中的一条数据
// 　　 组成 长度
// 　　 目录中文件文件头标记 4 bytes (0x02014b50)
// 　　 压缩使用的pkware 版本 2 bytes
// 　　 解压文件所需 pkware 版本 2 bytes
// 　　 全局方式位标记 2 bytes
// 　　 压缩方式 2 bytes
// 　　 最后修改文件时间 2 bytes
// 　　 最后修改文件日期 2 bytes
// 　　 ＣＲＣ－３２校验 4 bytes
// 　　 压缩后尺寸 4 bytes
// 　　 未压缩尺寸 4 bytes
// 　　 文件名长度 2 bytes
// 　　 扩展字段长度 2 bytes
// 　　 文件注释长度 2 bytes
// 　　 磁盘开始号 2 bytes
// 　　 内部文件属性 2 bytes
// 　　 外部文件属性 4 bytes
// 　　 局部头部偏移量 4 bytes
// 　　 文件名 （不定长度）
// 　　 扩展字段 （不定长度）
// 　　 文件注释 （不定长度）
// 　　
// 　　 3、压缩源文件目录结束标志
// 　　 组成 长度
// 　　 目录结束标记 4 bytes (0x02014b50)
// 　　 当前磁盘编号 2 bytes
// 　　 目录区开始磁盘编号 2 bytes
// 　　 本磁盘上纪录总数 2 bytes
// 　　 目录区中纪录总数 2 bytes
// 　　 目录区尺寸大小 4 bytes
// 　　 目录区对第一张磁盘的偏移量 4 bytes
// 　　 ZIP 文件注释长度 2 bytes
// 　　 ZIP 文件注释 （不定长度）
