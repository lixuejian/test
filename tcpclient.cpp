/*********************************************************************************
*FileName: TcpClient
*Author:  ��ѧ��
*Version:  1.0
*Date:  2016-11-11
*Description:  ���������ҪĿ��Ϊ��Ϊ����ͨ�ŵĿͻ��������˽�������֮�������˷����ַ�
*Others:  
*Function List:  //��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
1.
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

int main()
{
	WSADATA wsadata;						// ���ڳ�ʼ��Windows Socket 
	SOCKET s;								// �����������ͨ�ŵ��׽��� 
	SOCKADDR_IN serverAddr;			        // ��������ַ

	// ��ʼ��Windows Socket
	if(WSAStartup(MAKEWORD(2,2),&wsadata)!= 0)
	{   
		printf("WSAStartup failed !\n"); 
		cout<<"������룺"<<WSAGetLastError()<<endl;
		return 1;   
	} else
	{
		cout<<"WSAStartup success..."<<endl;
	}

	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		cout<<"create listenSocket failured"<<endl;
		cout<<"������룺"<<WSAGetLastError()<<endl;
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
		cout<<"������룺"<<WSAGetLastError()<<endl;
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
		cout<<"���յ������Է���������ϢΪ:"<<buf<<endl;
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




