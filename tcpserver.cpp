/*********************************************************************************
  *FileName:  TcpServer
  *Author:  李学健
  *Version:  1.0
  *Date:  2016-11-11
  *Description:  本程序的主要目的为作为网络通信的服务端与客户端建立连接之后
				 接收客户端发来的字符控制摄像头旋转
  *Others:  
  *Function List:  //主要函数列表，每条记录应包含函数名及功能简要说明
     1.Serial_write：串口写函数
     2.Serial_read：串口读函数
	 3.Serial_open：打开串口
  *History:  //修改历史记录列表，每条修改记录应包含修改日期、修改者及修改内容简介
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
HANDLE hcom;//串口句柄，全局

#define BUF_SIZE    64      // 缓冲区大小
 
int Serial_write(const void *Buf,int size)  
{  
    DWORD dw;  
    WriteFile(hcom,Buf,size,&dw,NULL);  
	    //if(dw==size)
		//printf("数据发送成功！\n");
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
        printf("打开串口失败！\n");
        return FALSE;
    }
	else
		printf("打开串口成功！\n");

	//配置串口参数 2400-n-8-1
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
		printf("配置串口失败！\n");
		return FALSE;
	}
	else
		printf("串口配置成功！\n");
	if( !PurgeComm( hcom, PURGE_RXCLEAR ) )  
	{
		printf("IO清理失败！\n");
		return FALSE; 
	}
	else 
		printf("IO清理成功！\n");
		
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
    SOCKET  sClient;						// 客户端套接字，用于实现与客户端的通信
	int     retVal;							// 调用各种Socket函数的返回值 

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

		// 获取当前系统时间
		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDateTime[30];
		sprintf(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		// 打印输出的信息
		printf("%s, Recv From Client [%s:%d] :%s\n", sDateTime, inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, buf);   
		// 如果客户端发送quit字符串，则服务器退出
		
		
		//strcmp(buf, "w") == 0;
		if (strcmp(buf, "w") == 0)    
		{
			Serial_write(ActionUp,7);
			printf("正在向上转...\n");	
		}
		else if (strcmp(buf, "x") == 0) 
		{
			Serial_write(ActionDown,7);
			printf("正在向下转...\n");			
		}
		else if(strcmp(buf, "a") == 0) 
		{
			Serial_write(ActionLeft,7);
			printf("正在向左转...\n");
		}
		else if (strcmp(buf, "d") == 0)
		{
			Serial_write(ActionRight,7);
			printf("正在向右转...\n");	
		}
		else if (strcmp(buf, "s") == 0)
		{
			Serial_write(ActionStop,7);
			printf("已停止...\n");	
		}
		/*else if (strcmp(buf, "q") == 0)
		{
           break;
		}
		else
		{
			printf("指令错误，请重新输入命令\n");
		}*/
		
		if(strcmp(buf, "quit") == 0)
		{
			retVal = send(sClient,"quit",strlen("quit"),0);   
			break;
		}
		else		// 否则向客户端发送回显字符串
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






//// TcpServer.cpp : 定义控制台应用程序的入口点。
////
//
//#include "stdafx.h"
//#include <WINSOCK2.H>   
//#include <iostream>
//  
//using namespace std;
//
//#pragma comment(lib,"WS2_32.lib")   
//#define BUF_SIZE    64      // 缓冲区大小
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	WSADATA wsd;					// WSADATA变量，用于初始化Windows Socket   
//    SOCKET  sServer;					// 服务器套接字，用于监听客户端请求
//    SOCKET  sClient;					// 客户端套接字，用于实现与客户端的通信   
//    int     retVal;							// 调用各种Socket函数的返回值   
//    char    buf[BUF_SIZE];			// 用于接受客户端数据的缓冲区   
//  
//    // 初始化套接字动态库   
//    if(WSAStartup(MAKEWORD(2,2),&wsd) != 0)   
//    {   
//        printf("WSAStartup failed !\n");   
//        return 1;   
//    }     
//    // 创建用于监听的套接字   
//    sServer = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);   
//    if(INVALID_SOCKET == sServer)   
//    {   
//        printf("socket failed !\n");   
//        WSACleanup();   
//        return -1;   
//    }     
//	// 设置套接字为非阻塞模式
//	int iMode = 1;
//	retVal = ioctlsocket(sServer, FIONBIO, (u_long FAR*) &iMode);
//	if(retVal == SOCKET_ERROR)
//	{
//		printf("ioctlsocket failed !\n");
//        WSACleanup();   
//        return -1;   
//	}
//	
//    // 设置服务器套接字地址   
//    SOCKADDR_IN addrServ;   
//    addrServ.sin_family = AF_INET;   
//    addrServ.sin_port = htons(9990);		// 监听端口为9990
//    addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    
//    // 绑定套接字sServer到本地地址，端口9990  
//    retVal = bind(sServer,(const struct sockaddr*)&addrServ,sizeof(SOCKADDR_IN));   
//    if(SOCKET_ERROR == retVal)   
//    {   
//        printf("bind failed !\n");   
//        closesocket(sServer);   
//        WSACleanup();   
//        return -1;   
//    }     
//    // 监听套接字   
//    retVal = listen(sServer,1);   
//    if(SOCKET_ERROR == retVal)   
//    {   
//        printf("listen failed !\n");   
//        closesocket(sServer);   
//        WSACleanup();   
//        return -1;   
//    }     
//    // 接受客户请求   
//    printf("TCP Server start...\n");
//    sockaddr_in addrClient;							// 客户端地址
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
//    // 循环接收客户端的数据，直接客户端发送quit命令后退出。  
//	while(true)
//	{
//		ZeroMemory(buf,BUF_SIZE);						// 清空接收数据的缓冲区
//		retVal = recv(sClient,buf,BUFSIZ,0);				// 
//		if(SOCKET_ERROR == retVal)   
//		{   
//			printf("recv failed !\n");   
//			closesocket(sServer);   
//			closesocket(sClient);   
//			WSACleanup();   
//			return -1;   
//		}   
//		// 获取当前系统时间
//		SYSTEMTIME st;
//		GetLocalTime(&st);
//		char sDateTime[30];
//		sprintf(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
//		// 打印输出的信息
//		printf("%s, Recv From Client [%s:%d] :%s\n", sDateTime, inet_ntoa(addrClient.sin_addr), addrClient.sin_port, buf);   
//		// 如果客户端发送quit字符串，则服务器退出
//		if(strcmp(buf, "quit") == 0)
//		{
//			retVal = send(sClient,"quit",strlen("quit"),0);   
//			break;
//		}
//		else		// 否则向客户端发送回显字符串
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
//    // 释放套接字   
//    closesocket(sServer);   
//    closesocket(sClient);   
//    WSACleanup();   
//	// 暂停，按任意键退出
//	system("pause");
//	return 0;
//}

