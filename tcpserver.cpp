/*********************************************************************************
  *FileName:  TcpServer
  *Author:  ��ѧ��
  *Version:  1.0
  *Date:  2016-11-11
  *Description:  ���������ҪĿ��Ϊ��Ϊ����ͨ�ŵķ������ͻ��˽�������֮��
				 ���տͻ��˷������ַ���������ͷ��ת
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
 
int Serial_write(const void *Buf,int size)  
{  
    DWORD dw;  
    WriteFile(hcom,Buf,size,&dw,NULL);  
	    //if(dw==size)
		//printf("���ݷ��ͳɹ���\n");
    return dw;  
}  
int Serial_read(void *OutBuf,int size)  
{  
    DWORD cnt=0;  
    ReadFile(hcom,OutBuf,size,&cnt,0);  
    return cnt;  
  
}  
int Serial_open()
{
	hcom = CreateFile(_T("COM4"),GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
    if(hcom==(HANDLE)-1)
    {
        printf("�򿪴���ʧ�ܣ�\n");
        return FALSE;
    }
	else
		printf("�򿪴��ڳɹ���\n");

	//���ô��ڲ��� 2400-n-8-1
	DCB dcb;
	GetCommState(hcom,&dcb);
	dcb.BaudRate = 2400;
	dcb.ByteSize = 8;
	//dcb.fBinary = 1;
	dcb.fParity = NOPARITY; 
	dcb.StopBits = ONESTOPBIT;
	if (!SetCommState(hcom,&dcb))
	{
		DWORD dwError = GetLastError();
		printf("���ô���ʧ�ܣ�\n");
		return FALSE;
	}
	else
		printf("�������óɹ���\n");
	if( !PurgeComm( hcom, PURGE_RXCLEAR ) )  
	{
		printf("IO����ʧ�ܣ�\n");
		return FALSE; 
	}
	else 
		printf("IO����ɹ���\n");
		
	SetupComm(hcom,100,100);

	return TRUE;
}
void Serial_close(void)  
{  
    CloseHandle(hcom);  
}

int main()
{
    WSADATA wsadata;
    SOCKET listenSocket;
    SOCKET  sClient;						// �ͻ����׽��֣�����ʵ����ͻ��˵�ͨ��
	int     retVal;							// ���ø���Socket�����ķ���ֵ 

    SOCKADDR_IN serverAddr,clientAddr;
 
    int len;
 
	unsigned char ActionUp[7]={255,1,0,16,0,63,80};      //0xFF,0x01,0x00,0x10,0x00,0x3F,0x50
	unsigned char ActionDown[7]={255,1,0,8,0,63,72};     //0xFF,0x01,0x00,0x08,0x00,0x3F,0x48
	unsigned char ActionLeft[7]={255,1 ,0,2,63,0,66};    //0xFF,0x01,0x00,0x02,0x3F,0x00,0x42
	unsigned char ActionRight[7]={255,1,0,4,63,0,68};    //0xFF,0x01,0x00,0x04,0x3F,0x00,0x44
	unsigned char ActionStop[7]={255,1,0,0,0,0,1};       //0xFF,0x01,0x00,0x00,0x00,0x00,0x01
	if(Serial_open()==FALSE)
	{
		return FALSE;
	}

    WSAStartup(MAKEWORD(2,2),&wsadata);
 
    listenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 
    if (listenSocket == INVALID_SOCKET)
    {
        cout<<"create listenSocket failured"<<endl;
    }
 

	printf("TCP Server 20%% start ...\n");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port= htons(5150);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
	printf("TCP Server 40%% start ...\n");

    int iRet = bind(listenSocket,(SOCKADDR*)&serverAddr,sizeof(serverAddr));

	printf("TCP Server 60%% start ...\n");

    if (SOCKET_ERROR == iRet)
    {
        cout<<"bind failed"<<endl;
    }
 
    iRet = listen(listenSocket,5);
    if (SOCKET_ERROR == iRet )
    {
        cout<<"listen failed"<<endl;
    }
 
	printf("TCP Server 80%% start ...\n");

    len = sizeof(clientAddr);

    sClient = accept(listenSocket,(SOCKADDR*)&clientAddr,&len);
    if (INVALID_SOCKET == sClient)
    {
        cout<<"accept new socket error"<<endl;
	}else
	{
		printf("TCP Server start success!!!\n");
	}
 

	


	//char cmd=NULL;
    char buf[100];
    while(1)
    {
        memset(buf,0,100);
        int n = recv(sClient,buf,100,0);
        //if (n > 0)
        //{
        //    cout<<buf<<endl;
        //}

		// ��ȡ��ǰϵͳʱ��
		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDateTime[30];
		sprintf(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		// ��ӡ�������Ϣ
		printf("%s, Recv From Client [%s:%d] :%s\n", sDateTime, inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, buf);   
		// ����ͻ��˷���quit�ַ�������������˳�
		
		
		//strcmp(buf, "w") == 0;
		if (strcmp(buf, "w") == 0)    
		{
			Serial_write(ActionUp,7);
			printf("��������ת...\n");	
		}
		else if (strcmp(buf, "x") == 0) 
		{
			Serial_write(ActionDown,7);
			printf("��������ת...\n");			
		}
		else if(strcmp(buf, "a") == 0) 
		{
			Serial_write(ActionLeft,7);
			printf("��������ת...\n");
		}
		else if (strcmp(buf, "d") == 0)
		{
			Serial_write(ActionRight,7);
			printf("��������ת...\n");	
		}
		else if (strcmp(buf, "s") == 0)
		{
			Serial_write(ActionStop,7);
			printf("��ֹͣ...\n");	
		}
		/*else if (strcmp(buf, "q") == 0)
		{
           break;
		}
		else
		{
			printf("ָ�������������������\n");
		}*/
		
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






//// TcpServer.cpp : �������̨Ӧ�ó������ڵ㡣
////
//
//#include "stdafx.h"
//#include <WINSOCK2.H>   
//#include <iostream>
//  
//using namespace std;
//
//#pragma comment(lib,"WS2_32.lib")   
//#define BUF_SIZE    64      // ��������С
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	WSADATA wsd;					// WSADATA���������ڳ�ʼ��Windows Socket   
//    SOCKET  sServer;					// �������׽��֣����ڼ����ͻ�������
//    SOCKET  sClient;					// �ͻ����׽��֣�����ʵ����ͻ��˵�ͨ��   
//    int     retVal;							// ���ø���Socket�����ķ���ֵ   
//    char    buf[BUF_SIZE];			// ���ڽ��ܿͻ������ݵĻ�����   
//  
//    // ��ʼ���׽��ֶ�̬��   
//    if(WSAStartup(MAKEWORD(2,2),&wsd) != 0)   
//    {   
//        printf("WSAStartup failed !\n");   
//        return 1;   
//    }     
//    // �������ڼ������׽���   
//    sServer = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);   
//    if(INVALID_SOCKET == sServer)   
//    {   
//        printf("socket failed !\n");   
//        WSACleanup();   
//        return -1;   
//    }     
//	// �����׽���Ϊ������ģʽ
//	int iMode = 1;
//	retVal = ioctlsocket(sServer, FIONBIO, (u_long FAR*) &iMode);
//	if(retVal == SOCKET_ERROR)
//	{
//		printf("ioctlsocket failed !\n");
//        WSACleanup();   
//        return -1;   
//	}
//	
//    // ���÷������׽��ֵ�ַ   
//    SOCKADDR_IN addrServ;   
//    addrServ.sin_family = AF_INET;   
//    addrServ.sin_port = htons(9990);		// �����˿�Ϊ9990
//    addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    
//    // ���׽���sServer�����ص�ַ���˿�9990  
//    retVal = bind(sServer,(const struct sockaddr*)&addrServ,sizeof(SOCKADDR_IN));   
//    if(SOCKET_ERROR == retVal)   
//    {   
//        printf("bind failed !\n");   
//        closesocket(sServer);   
//        WSACleanup();   
//        return -1;   
//    }     
//    // �����׽���   
//    retVal = listen(sServer,1);   
//    if(SOCKET_ERROR == retVal)   
//    {   
//        printf("listen failed !\n");   
//        closesocket(sServer);   
//        WSACleanup();   
//        return -1;   
//    }     
//    // ���ܿͻ�����   
//    printf("TCP Server start...\n");
//    sockaddr_in addrClient;							// �ͻ��˵�ַ
//    int addrClientlen = sizeof(addrClient); 
//
//	printf("addrClientlen==%d!!\n",addrClientlen);
//
//	sClient = accept(sServer,(sockaddr FAR*)&addrClient,&addrClientlen); 
//    if(INVALID_SOCKET == sClient)   
//    {   
//        printf("accept failed !\n");   
//        closesocket(sServer);   
//        WSACleanup();   
//        return -1;   
//    }   
//    // ѭ�����տͻ��˵����ݣ�ֱ�ӿͻ��˷���quit������˳���  
//	while(true)
//	{
//		ZeroMemory(buf,BUF_SIZE);						// ��ս������ݵĻ�����
//		retVal = recv(sClient,buf,BUFSIZ,0);				// 
//		if(SOCKET_ERROR == retVal)   
//		{   
//			printf("recv failed !\n");   
//			closesocket(sServer);   
//			closesocket(sClient);   
//			WSACleanup();   
//			return -1;   
//		}   
//		// ��ȡ��ǰϵͳʱ��
//		SYSTEMTIME st;
//		GetLocalTime(&st);
//		char sDateTime[30];
//		sprintf(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
//		// ��ӡ�������Ϣ
//		printf("%s, Recv From Client [%s:%d] :%s\n", sDateTime, inet_ntoa(addrClient.sin_addr), addrClient.sin_port, buf);   
//		// ����ͻ��˷���quit�ַ�������������˳�
//		if(strcmp(buf, "quit") == 0)
//		{
//			retVal = send(sClient,"quit",strlen("quit"),0);   
//			break;
//		}
//		else		// ������ͻ��˷��ͻ����ַ���
//		{
//			char    msg[BUF_SIZE];  
//			sprintf(msg, "Message received - %s", buf); 
//			retVal = send(sClient, msg, strlen(msg),0);   
//			if(SOCKET_ERROR == retVal)   
//			{   
//				printf("send failed !\n");   
//				closesocket(sServer);   
//				closesocket(sClient);   
//				WSACleanup();   
//				return -1;   
//			}   
//		}
//	}
//    // �ͷ��׽���   
//    closesocket(sServer);   
//    closesocket(sClient);   
//    WSACleanup();   
//	// ��ͣ����������˳�
//	system("pause");
//	return 0;
//}

