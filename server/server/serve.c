
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
	int i = WSAStartup(wdversion, &wdsockmsg);
	if (i != 0)
	{
		switch (i)
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

	if (2 != HIBYTE(wdsockmsg.wVersion) || 2 != LOBYTE(wdsockmsg.wVersion))
	{
		
		WSACleanup();
		printf("网络库版本不对");
		return 0;


	}

	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, 0);
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
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//ip 地址


	int t = bind(socketserver, (const struct sockaddr*)&si, sizeof(si));//绑定
	if (SOCKET_ERROR == t)
	{
		int b = WSAGetLastError();//获取错误码
		WSACleanup();
		system("pause");
		printf("error%d", b);
		return 0;
	}
	int l = listen(socketserver, SOMAXCONN);//开始监听
	if (SOCKET_ERROR == listen(socketserver, SOMAXCONN))
	{
		int b = WSAGetLastError();//获取错误码
		WSACleanup();
		system("pause");
		printf("error%d", b);
		return 0;

	}
	struct sockaddr_in clientmsg;
	int len = sizeof(clientmsg);

	SOCKET socketclient = accept(socketserver, (struct sockaddr*)&clientmsg, &len);//开始链接  创建一个socket用来装客户端的信息
	if (INVALID_SOCKET == socketclient)
	{
		int b = WSAGetLastError();//获取错误码
		WSACleanup();
		system("pause");
		printf("error%d", b);
		return 0;

	}
	else printf("客户端链接成功\n");



	if (SOCKET_ERROR == send(socketclient, "欢迎你的使用", sizeof("欢迎你的使用"), 0))
	{
		int a = SOCKET_ERROR;
		printf("socketsenderror %d\n", a);
	}
	while (1) 
	{
		char buff[1500] = { 0 };//记得初始化
		int res = recv(socketclient, buff, 1024, 0);
		if (0 == res)
		{
			printf("链接中断\n");

		}
		else if (res == SOCKET_ERROR)
		{
			printf("error%d", res);
		}
		else {
			printf("%d %s\n", res, buff);
		}
		
		scanf("%s", buff);
		if (SOCKET_ERROR == send(socketclient, buff,strlen(buff) , 0))
		{
			int a = SOCKET_ERROR;
			printf("socketsenderror %d", a);
		}


	}
	
	 
	closesocket(socketclient);
    closesocket(socketserver);
		WSACleanup();
		
		system("pause");
		return 0;


	
}