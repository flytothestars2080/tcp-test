#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<WinSock2.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//表示链接Ws2_32.lib这个库
fd_set allsockets;//创建一个集合
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
	{for (u_int i = 0; i < allsockets.fd_count; i++)
	{
		closesocket(allsockets.fd_array[i]);

	}
	WSACleanup();
	}
	}
	return TRUE;
}


int main()
{
	SetConsoleCtrlHandler(fun,TRUE);//回调函数让操作系统运行的函数


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
	
	FD_ZERO(&allsockets);//将集合清零
	FD_SET(socketserver, &allsockets);//把服务器socket的装进去
	while (1)
	{
		fd_set readsockets = allsockets;//初始化
		fd_set writesockets = allsockets;
		fd_set errorsockets = allsockets;
		//定义等待时间
		struct timeval s;
		s.tv_sec = 3;//秒
		s.tv_usec = 0;//毫秒
		int nes=select(0, &readsockets, &writesockets,&errorsockets,&s);
		if (0 == nes)
		{
			continue;
		}
		else if (nes > 0)
		{
			//有响应

			for (u_int i = 0; i < errorsockets.fd_count; i++)//遍历errorsockets数组
			{
				char buff[100] = {0};
				int len=0;
				if (SOCKET_ERROR == getsockopt(writesockets.fd_array[i], SOL_SOCKET, SO_ERROR, buff, &len))
				{
					printf("返回错误失败\n");

				}
				else
				{
					if(buff[3]!=' ') ///检测是否出错，因为不出错就会返回' '
					printf("%s \n", buff );

				}
				

			}


				for (u_int i = 0; i < writesockets.fd_count; i++)//编历writesockets数组
				{

					//printf("socketserver %d writesockets %d\n", socketserver, writesockets.fd_array[i]);
					/*if (send(writesockets.fd_array[i], "ok", sizeof("ok"), 0) == SOCKET_ERROR)
					{

						int a=GetLastError();
					}
					
					*/
				}
			for (u_int i = 0; i < readsockets.fd_count; i++)//遍历readsockets数组
			{
				if (readsockets.fd_array[i] == socketserver)//说明服务器有反应了
				{
					//要accept
					SOCKET socketclient = accept(socketserver, NULL, NULL);
					if (socketclient == INVALID_SOCKET)
					{

						//出错了
						WSAGetLastError();
						continue;
					}
					else
					{
						send(socketclient, "已连接到服务器", sizeof("已连接到服务器"), 0);

					}
					FD_SET(socketclient,&allsockets);



				}
				else//客户端有反应
				{
			
					char buff[1500] = {0};

				int Recy=recv(readsockets.fd_array[i],buff,sizeof(buff),0);
				if (Recy == 0)
				{
					SOCKET sockettemp = readsockets.fd_array[i];
					FD_CLR(readsockets.fd_array[i], &allsockets);
					closesocket(sockettemp);
					printf("客户端已下线");
					//客户端下线
				}
				else if (Recy == SOCKET_ERROR)
				{ //远程主机强迫关闭了一个现有的连接。 10054
					int a=WSAGetLastError();//出错了
					printf("%d\n", a);
					
					switch (a)
					{
					case 10054: 

					{
						SOCKET sockettemp = readsockets.fd_array[i];
						FD_CLR(readsockets.fd_array[i], &allsockets);
						closesocket(sockettemp);
						printf("客户端已被强型下线\n");

					}break;
						
					}
				}
				else 
				{
					printf("%s\n", buff);//打印收到的消息
					/*char b[100];
					int n=0;
					printf("是否需要向客户端发送消息不需要请输入0\n");
					scanf("%d", &n);

						if (n!=0)
					{
							printf("请输入：\n");
							scanf("%s", b);
							send(readsockets.fd_array[i], b, 100, 0);

					}
					else send(readsockets.fd_array[i], "服务器已收到\n",sizeof("服务器已收到\n"), 0);*/
					send(readsockets.fd_array[i], "服务器已收到", sizeof("服务器已收到"), 0);
				}
				
				}
				



			}

			

		}
		else //nes<0
		{
	int a=GetLastError();
	printf("selecterror%d\n",a);
	break;
			//出错了

		}

		char a[5];
		scanf("%s", a);
		if (a[0] == 'e' && a[1] == 'x' && a[2] == 'i' && a[3] == 't')
			break;

	}


	closesocket(socketserver);
	for (u_int i = 0; i < allsockets.fd_count; i++)
	{
		closesocket(allsockets.fd_array[i]);

	}

	WSACleanup();


	system("pause");


	return 0;
}