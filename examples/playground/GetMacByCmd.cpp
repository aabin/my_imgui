//////////////////////////////////////////////////////////////////////////
// ������ ͨ�������з�ʽ�õ�MAC��ַ
// ���ߣ� �����
// ���ڣ� 2005��7��1��

// email: guohongfeng@gmail.com
/////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <windows.h>
#include <string>
#include <iostream>
using namespace std;

//��������������С
const long MAX_COMMAND_SIZE = 10000;

//��ȡMAC������
char szFetCmd[] = "ipconfig /all";
//����MAC��ַ��ǰ����Ϣ
const string str4Search = "Physical Address";

//�������з�ʽ��ȡ����MAC��ַ
BOOL GetMacByCmd(char *lpszMac);

////////////////////////////////////////////////////////////////////////////
// �������� GetMacByCmd(char *lpszMac)
// ������
//      ���룺 void
//      ����� lpszMac,���ص�MAC��ַ��
// ����ֵ��
//      TRUE:  ���MAC��ַ��
//      FALSE: ��ȡMAC��ַʧ�ܡ�
// ���̣�
//      1. ����һ�������ܵ���
//      2. ����һ��IPCONFIG �Ľ��̣���������ض��򵽹ܵ���
//      3. �ӹܵ���ȡ�����з��ص�������Ϣ���뻺����lpszBuffer��
//      4. �ӻ�����lpszBuffer�л�ó�ȡ��MAC����
//
//  ��ʾ�����Է�����ɴ˳�����IP��ַ��������Ϣ��
//        ���������Ŀ���ͨ���������ʽ�õ�����Ϣֻ��ı�strFetCmd �� 
//        str4Search�����ݼ��ɡ�
///////////////////////////////////////////////////////////////////////////

BOOL GetMacByCmd(char *lpszMac)
{
	//��ʼ������MAC��ַ������
	BOOL bret; 

	SECURITY_ATTRIBUTES sa; 
	HANDLE hReadPipe,hWritePipe;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sa.lpSecurityDescriptor = NULL; 
	sa.bInheritHandle = TRUE; 
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if(!bret)
	{
		return FALSE;
	}

	STARTUPINFO si; 
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO); 
    GetStartupInfo(&si); 
    si.hStdError = hWritePipe; 
    si.hStdOutput = hWritePipe; 
    si.wShowWindow = SW_HIDE; 
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	bret = CreateProcess (NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, 
      NULL, &si, &pi ); 
 
    char szBuffer[MAX_COMMAND_SIZE+1]; 
	string strBuffer;

	 if (bret) 
	 { 
		  WaitForSingleObject (pi.hProcess, INFINITE); 
		  unsigned long count;

		  memset(szBuffer, 0x00, sizeof(szBuffer));
		  bret  =  ReadFile(hReadPipe,  szBuffer,  MAX_COMMAND_SIZE,  &count,  0);
		  if(!bret)
		  {
			   //�ر����еľ��
			   CloseHandle(hWritePipe);
			   CloseHandle(pi.hProcess); 
			   CloseHandle(pi.hThread); 
			   CloseHandle(hReadPipe);
			   return FALSE;
		  }
		  else
		  {
			   strBuffer = szBuffer;
			   long ipos;
			   ipos = strBuffer.find(str4Search);
			   strBuffer = strBuffer.substr(ipos+str4Search.length());
			   ipos = strBuffer.find(":");
			   strBuffer = strBuffer.substr(ipos+1);
			   ipos = strBuffer.find("\r\n");
			   strBuffer = strBuffer.substr(0, ipos);
		  }
  
	 }

 strcpy(lpszMac, strBuffer.c_str());

 //ȥ���м�ġ�00-50-EB-0F-27-82���м��'-'�õ�0050EB0F2782


 //�ر����еľ��
 CloseHandle(hWritePipe);
 CloseHandle(pi.hProcess); 
 CloseHandle(pi.hThread); 
 CloseHandle(hReadPipe);
 return TRUE;

}

