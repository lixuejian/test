/*********************************************************************************
*FileName: TcpClient
*Author:  李学健
*Version:  1.0
*Date:  2016-11-11
*Description:  本程序的主要目的为作为网络通信的客户端与服务端建立连接之后向服务端发送字符
*Others:  
*Function List:  //主要函数列表，每条记录应包含函数名及功能简要说明
1.
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

int main()
{
	WSADATA wsadata;						// 用于初始化Windows Socket 
	SOCKET s;								// 与服务器进行通信的套接字 
	SOCKADDR_IN serverAddr;			        // 服务器地址

	// 初始化Windows Socket
	if(WSAStartup(MAKEWORD(2,2),&wsadata)!= 0)
	{   
		printf("WSAStartup failed !\n"); 
		cout<<"错误代码："<<WSAGetLastError()<<endl;
		return 1;   
	} else
	{
		cout<<"WSAStartup success..."<<endl;
	}

	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		cout<<"create listenSocket failured"<<endl;
		cout<<"错误代码："<<WSAGetLastError()<<endl;
	}else
	{
		printf("create listenSocket success...\n");
	}



	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port= htons(5150);
	serverAddr.sin_addr.s_addr = inet_addr("192.168.1.2");

	int ierror = connect(s,(SOCKADDR*)&serverAddr,sizeof(serverAddr));

	if (ierror == SOCKET_ERROR)
	{
		cout<<"connect failure ";
		cout<<"错误代码："<<WSAGetLastError()<<endl;
		return 0;
	}
	else
	{
		cout<<"connect success"<<endl;
		cout<<"**********************************************"<<endl;
	}

	string str;
	cin>>str;

	char buf[100];
	while(str.length() > 0)
	{
		int iRet = send(s,str.c_str(),str.length(),0);
		if (SOCKET_ERROR == iRet)
		{
			cout<<"send failure"<<endl;
			printf("[MSP] Fail to send %d\n", WSAGetLastError());
		}else
		{
			cout<<"send success"<<endl;
		}

		memset(buf,0,100);
		int n = recv(s,buf,100,0);
		cout<<"接收到的来自服务器的信息为:"<<buf<<endl;
		if(strcmp(buf , "quit") == 0)
		{  
			break;
		}

		cin>>str;
	}

	closesocket(s);

	system("pause");

	return 0;
}




