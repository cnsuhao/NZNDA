#pragma once


bool Deal_CPEV();//���̴����¼�
bool Deal_EPDE();//���������˳�

//����ָ����ַ�Ƿ�������Ѽ�¼ProtIndex���б�,�����򷵻��±�.-1û�ҵ�.
int SerarchProtIndex(LPVOID lpAddress);


DWORD OnExceptionDebugEvent(LPEXCEPTION_DEBUG_INFO pDbgInfo);




//����ָ��������
bool	Decry(DWORD dwIndexBlock,BYTE bFirstKey);

//ͨ�����ܵݹ����.
bool	EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock);


//����Xor�ӽ���
BYTE XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
void XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);

//��ָ�����ڴ�
bool ReadDebuggedMemory(LPVOID lpAddress,DWORD dwGetNumber,_Out_ BYTE* wcGetValue);

//ʹָ�����ڴ�����Ȩ��
bool RepairProtNoAccess(DWORD dwIndex);
//ʹָ�����ڴ��ɿɶ���д��ִ��
bool RepairProtData(DWORD dwIndex);

struct _PROCMSG
{
	DWORD dwStact;            //һ����־
	DWORD dwImageBase;   //Դ����Ļ�ַ
	DWORD dwImportRVA;    //Դ��������λ��
	DWORD dwEntryRVA;      //Դ������ڵ�

	vector	<DWORD>		vdLength;		//����
	vector	<bool>		vbEncryState;	//����״̬
	vector	<PVOID>		vpAddress;		//��ʼ��ַ
};