/*********************************************************************************
  *FileName:  TcpServer
  *Author:  ��ѧ��
  *Version:  1.0
  *Date:  2016-11-11
  *Description:  ���������ҪĿ��Ϊ��Ϊ����ͨ�ŵķ������ͻ��˽�������֮��
				 ���տͻ��˷������ַ�����ʾ����
  *Others:  
  *Function List:  //��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
     1.Serial_write������д����
     2.Serial_read�����ڶ�����
	 3.Serial_open���򿪴���
  *History:  //�޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ��
     1.Date:
       Author:
       Modification:
     2.
**********************************************************************************/
#include "stdafx.h"

#include <WinSock2.h>
#include <iostream>
#include <string>
using namespace std;
#pragma comment(lib,"WS2_32.LIB")

#include <windows.h>
#include <winbase.h>
#include <tchar.h>
#include <SDKDDKVer.h>
HANDLE hcom;//���ھ����ȫ��

#define BUF_SIZE    64      // ��������С
 
int main()
{
    WSADATA wsadata;
    SOCKET listenSocket;
    SOCKET  sClient;						// �ͻ����׽��֣�����ʵ����ͻ��˵�ͨ��
	int     retVal;							// ���ø���Socket�����ķ���ֵ 

    SOCKADDR_IN serverAddr,clientAddr;
 
    int len;

    WSAStartup(MAKEWORD(2,2),&wsadata);
 
    listenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 
    if (listenSocket == INVALID_SOCKET)
    {
        cout<<"create listenSocket failured"<<endl;
	}else
	{
		printf("create listenSocket success...\n");
	}
 

	

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port= htons(5150);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iRet = bind(listenSocket,(SOCKADDR*)&serverAddr,sizeof(serverAddr));

    if (SOCKET_ERROR == iRet)
    {
        cout<<"bind failed"<<endl;
	}else
	{
		printf("bind success ...\n");
	}
 
    iRet = listen(listenSocket,5);
    if (SOCKET_ERROR == iRet )
    {
        cout<<"listen failed"<<endl;
	}else
	{
		cout<<"listen success ..."<<endl;
	}

    len = sizeof(clientAddr);

    sClient = accept(listenSocket,(SOCKADDR*)&clientAddr,&len);
    if (INVALID_SOCKET == sClient)
    {
        cout<<"accept new socket error"<<endl;
	}else
	{
		printf("TCP Server start success!!!\n");
		cout<<"**********************************************"<<endl;
	}
 
    char buf[100];
    while(1)
    {
        memset(buf,0,100);
        int n = recv(sClient,buf,100,0);


		// ��ȡ��ǰϵͳʱ��
		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDateTime[30];
		sprintf(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		// ��ӡ�������Ϣ
		printf("%s, Recv From Client [%s:%d] :%s\n", sDateTime, inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, buf);   
		// ����ͻ��˷���quit�ַ�������������˳�		
		
		
		if(strcmp(buf, "quit") == 0)
		{
			retVal = send(sClient,"quit",strlen("quit"),0);   
			break;
		}
		else		// ������ͻ��˷��ͻ����ַ���
		{
			char    msg[BUF_SIZE];  
			sprintf(msg, "Server received Message is: %s", buf); 
			retVal = send(sClient, msg, strlen(msg),0);   
			if(SOCKET_ERROR == retVal)   
			{   
				printf("send failed !\n");   
				closesocket(listenSocket);   
				closesocket(sClient);   
				WSACleanup();   
				return -1;   
			}   
		}       
    }
    closesocket(sClient);
    closesocket(listenSocket);
    WSACleanup();
    system("pause");
    return 0;
}