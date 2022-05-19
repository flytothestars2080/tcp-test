#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<WinSock2.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//表示链接Ws2_32.lib这个库
int main()
{
	WORD wdversion = MAKEWORD(2, 2);//获取版本
	WSADATA wdsockmsg;//存放版本信息
	int wt = WSAStartup(wdversion, &wdsockmsg);//打开网络库
	if (wt != 0)
	{
		switch (wt)
		{
		case WSASYSNOTREADY:printf("请重启系统");
			break;
		case WSAVERNOTSUPPORTED:printf("请检查版本");
			break;
		case WSAEINPROGRESS:printf("请重启软件");
			break;
		case WSAEPROCLIM:printf("请关点一些软件");
			break;
		case WSAEFAULT:printf("参数错了");
			break;
		}
		return 0;

	}
	else printf("网络库打开成功\n");

	if (2 != HIBYTE(wdsockmsg.wVersion) || 2 != LOBYTE(wdsockmsg.wVersion))//校验版本
	{

		WSACleanup();
		printf("网络库版本不对");
		return 0;


	}

	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, 0);//创建socket
	if (socketserver == INVALID_SOCKET)
	{

		int a = WSAGetLastError();
		//清理网络库
		WSACleanup();
		return 0;


	}
	else {
		printf("socket 创建成功\n");
	}
	struct sockaddr_in si;
	si.sin_family = AF_INET;//IP类型 ipv4ot ipv6
	si.sin_port = htons(12345);//端口号
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//ip地址
	if (SOCKET_ERROR == connect(socketserver, (struct sockaddr*)&si, sizeof(si)))
	{
		int a = WSAGetLastError();
		//清理网络库
		printf("%d", a);
		closesocket(socketserver);
		WSACleanup();
		return 0;


	}
	else
	{
		printf("服务器连接成功\n");
	}

	
	while (1)
	{
		char buff[1500] = { 0 };//记得初始化
		//int res = recv(socketserver, buff, 1024, 0);
		//if (0 == res)
		//{
		//	printf("链接中断\n");

		//}
		//else if (res == SOCKET_ERROR)
		//{
		//	printf("error%d", res);
		//}
		//else {
		//	printf("%d %s\n", res, buff);
		//}

		scanf("%s", buff);
		
	
		if (buff[0]=='e'&&buff[1]=='x'&&buff[2]=='i'&&buff[3]=='t')
		{
			break;

		}
		if (SOCKET_ERROR == send(socketserver, buff,1024, 0))
		{
			int a = SOCKET_ERROR;
			printf("socketsenderror %d\n", a);
			if (a == -1)
			{
				printf("服务器已关闭\n");
			}
		
		}
	}



	closesocket(socketserver);
	WSACleanup();
	system("pause");
	return 0;
}