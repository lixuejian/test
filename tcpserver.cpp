/*********************************************************************************
  *FileName:  TcpServer
  *Author:  李学健
  *Version:  1.0
  *Date:  2016-11-11
  *Description:  本程序的主要目的为作为网络通信的服务端与客户端建立连接之后
				 接收客户端发来的字符并显示出来
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
 
int main()
{
    WSADATA wsadata;
    SOCKET listenSocket;
    SOCKET  sClient;						// 客户端套接字，用于实现与客户端的通信
	int     retVal;							// 调用各种Socket函数的返回值 

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


		// 获取当前系统时间
		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDateTime[30];
		sprintf(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		// 打印输出的信息
		printf("%s, Recv From Client [%s:%d] :%s\n", sDateTime, inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, buf);   
		// 如果客户端发送quit字符串，则服务器退出		
		
		
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