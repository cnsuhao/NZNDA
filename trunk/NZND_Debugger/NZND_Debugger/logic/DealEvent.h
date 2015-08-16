#pragma once


bool Deal_CPEV();//进程创建事件
bool Deal_EPDE();//被调试者退出

//搜索指定地址是否存在于已记录ProtIndex的列表,存在则返回下标.-1没找到.
int SerarchProtIndex(LPVOID lpAddress);


DWORD OnExceptionDebugEvent(LPEXCEPTION_DEBUG_INFO pDbgInfo);




//解密指定索引块
bool	Decry(DWORD dwIndexBlock,BYTE bFirstKey);

//通过加密递归程序.
bool	EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock);


//流动Xor加解密
BYTE XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
void XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);

//读指定块内存
bool ReadDebuggedMemory(LPVOID lpAddress,DWORD dwGetNumber,_Out_ BYTE* wcGetValue);

//使指定块内存变成无权限
bool RepairProtNoAccess(DWORD dwIndex);
//使指定块内存变成可读可写可执行
bool RepairProtData(DWORD dwIndex);

struct _PROCMSG
{
	DWORD dwStact;            //一个标志
	DWORD dwImageBase;   //源程序的基址
	DWORD dwImportRVA;    //源程序导入表的位置
	DWORD dwEntryRVA;      //源程序入口点

	vector	<DWORD>		vdLength;		//长度
	vector	<bool>		vbEncryState;	//加密状态
	vector	<PVOID>		vpAddress;		//起始地址
};