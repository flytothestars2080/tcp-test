#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#define ip "127.0.0.1"
#include<stdio.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
struct fd_sset
{
	unsigned short count;//有效个数
	SOCKET socketarr[WSA_MAXIMUM_WAIT_EVENTS];//socket 数组
	WSAEVENT eventarr[WSA_MAXIMUM_WAIT_EVENTS];//eventarr 事件数组


};
struct fd_sset arr[20];//包含两个数组 事件和socket
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
	{
		for (int j = 0; j < 20; j++)
		{


			for (int i = 0; i < arr[i].count; i++)
			{
				closesocket(arr[j].socketarr[i]);
				WSACloseEvent(arr[j].eventarr[i]);  //释放第j组第i个socket



			}

		}



	}
		break;

		WSACleanup();
	}

	return TRUE;
};

int main()
{
	SetConsoleCtrlHandler(fun, TRUE);//回调函数让操作系统运行的函数
	WORD W = MAKEWORD(2, 2);
	WSADATA msg;//存放版本信息
	int i = WSAStartup(W, &msg);//启动网络库
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
	if (2 != HIBYTE(msg.wVersion) || 2 != LOBYTE(msg.wVersion))
	{
		WSACleanup();

		printf("网络库版本不对\n");
		return 0;
	}
	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, 0);
	if (socketserver == INVALID_SOCKET)
	{

		int a = WSAGetLastError();
		//清理网络库
		printf("socketbuilderror%d\n", a);
		WSACleanup();
		return 0;

	}

	struct sockaddr_in si;
	si.sin_family = AF_INET;//IP类型 
	si.sin_port = htons(12345);//端口号
	si.sin_addr.S_un.S_addr = inet_addr(ip);//ip 地址
	int t = bind(socketserver, (const struct sockaddr*)&si, sizeof(si));
	if (SOCKET_ERROR == t)
	{
		int b = WSAGetLastError();//获取错误码
		WSACleanup();
		system("pause");
		printf("binderror%d", b);
		return 0;
	}
	int l = listen(socketserver, SOMAXCONN);
	if (SOCKET_ERROR == listen(socketserver, SOMAXCONN))
	{
		int b = WSAGetLastError();//获取错误码
		WSACleanup();
		system("pause");
		printf("listenerror%d", b);

	}



	

	//memset(arr,0,sizeof(arr) );






	WSAEVENT eventServer = WSACreateEvent();//创建事件句柄
	if (eventServer == WSA_INVALID_EVENT)
	{
		int b = WSAGetLastError();
		//获取错误码
		closesocket(socketserver);
		WSACleanup();
		system("pause");
		printf("buildeventerror%d", b);
		return 0;
	}
	if (SOCKET_ERROR == WSAEventSelect(socketserver, eventServer, FD_ACCEPT))//将socket和句柄链接到一起 让Windows系统通过句柄监控这个socket
	{

		int b = WSAGetLastError();
		//获取错误码
		WSACloseEvent(eventServer);
		closesocket(socketserver);
		WSACleanup();
		system("pause");
		printf("eventselecterror%d", b);
		return 0;

	}
	//一共20组socket和事件,每组64
	arr[0].socketarr[arr[0].count] = socketserver;
	arr[0].eventarr[arr[0].count] = eventServer;    //分别装进对应的数组
	arr[0].count++;


	while (1)
	{
		for (int j = 0; j < 20; j++)
		{
			if (arr[j].count == 0)
			{
				continue;
			}

			DWORD nes = WSAWaitForMultipleEvents(arr[j].count, arr[j].eventarr, FALSE, 0, FALSE);//向操作系统询问某组socket和事件是否发生

		if (nes == WSA_WAIT_FAILED)
		{
			//出错了
			int a = WSAGetLastError();
			printf("wsawaitforerror%d", a);
			break;
			///#define WAIT_FAILED ((DWORD)0xFFFFFFFF)

		}
		if (nes == WSA_WAIT_TIMEOUT)//如果参数四写具体的时间如123啥的就加个这个
		{

			continue;
		}
		DWORD nIndex = nes - WSA_WAIT_EVENT_0;//这一组有事件发生返回最小发生响应事件数组的下标




		for (unsigned i = nIndex; i < arr[j].count; i++)
		{
			DWORD nes = WSAWaitForMultipleEvents(1, &arr[j].eventarr[i], FALSE, 0, FALSE);//向操作系统询问事件 从最小发生响应事件开始一个一个问

			if (nes == WSA_WAIT_FAILED)
			{
				//出错了
				int a = WSAGetLastError();
				printf("wsawaitforerror%d", a);
				continue;
				///#define WAIT_FAILED ((DWORD)0xFFFFFFFF)

			}
			if (nes == WSA_WAIT_TIMEOUT)//如果参数四写具体的时间如123啥的就加个这个
			{

				continue;
			}

			WSANETWORKEVENTS lpNetevents;//得到下标对应的事件
			if (SOCKET_ERROR == WSAEnumNetworkEvents(arr[j].socketarr[i], arr[j].eventarr[i], &lpNetevents))
			{

				int a = WSAGetLastError();

				printf("wsaenumnetworkevent error %d", a);
				continue;
			}


			if (lpNetevents.lNetworkEvents & FD_ACCEPT)//如果来的信号中有accept请求
			{
				if (lpNetevents.iErrorCode[FD_ACCEPT_BIT] == 0)//如果没有出错
				{
					SOCKET socketclient = accept(arr[j].socketarr[i], NULL, NULL);//创建客户端
					if (socketclient == INVALID_SOCKET)
					{


						continue;
					}
					WSAEVENT clientevent = WSACreateEvent();//创建事件对象
					if (clientevent == WSA_INVALID_EVENT)
					{
						closesocket(socketclient);
						continue;
					}
					if (SOCKET_ERROR == WSAEventSelect(socketclient, clientevent, FD_WRITE | FD_CLOSE | FD_READ))//投递给系统
					{
						closesocket(socketclient);
						WSACloseEvent(clientevent);
						continue;
					}



					for (int m = 0; m < 20; m++)
					{
						if (arr[m].count < 64)
						{//装进结构体
							arr[m].socketarr[arr[m].count] = socketclient;
							arr[m].eventarr[arr[m].count] = clientevent;
							arr[m].count++;
							break;

						}

					}
					
					
					printf("accept event\n");
				}
				else
				{
					continue;

				}




			}
			if (lpNetevents.lNetworkEvents & FD_WRITE)//可以发消息给客户端了
			{
				if (lpNetevents.iErrorCode[FD_WRITE_BIT] == 0)//没出错
				{
					if (SOCKET_ERROR == send(arr[j].socketarr[i], " connect success", strlen("connect success"), 0))
					{
						int a = GetLastError();
						printf("send failed %d\n", a);
						continue;

					}

					printf("write event\n");
				}
				else
				{
					printf("socket error%d", lpNetevents.iErrorCode[FD_WRITE_BIT]);
					continue;
				}

			}



			if (lpNetevents.lNetworkEvents & FD_READ)//客户端发消息了
			{
				if (lpNetevents.iErrorCode[FD_READ_BIT] == 0)
				{
					char buff[1500] = { 0 };

					if (SOCKET_ERROR == recv(arr[j].socketarr[i], buff, 1499, 0))
					{
						int a = GetLastError();
						printf("recy fail error code%d", a);
						continue;

					}
					else
					{
						printf("socket send buff :%s\n", buff);
					}
					printf("  read event \n");
				}
				else
				{
					printf("socket recy error:%d", lpNetevents.iErrorCode[FD_READ_BIT]);
					continue;
				}
			}


			if (lpNetevents.lNetworkEvents & FD_CLOSE)//客户端下线
			{
				/*if (lpNetevents.iErrorCode[FD_CLOSE_BIT] == 0)
				{

				}
				else
				{

				}*/
				printf("client close\n");
				if (arr[j].count == 0)
				{
					break;
				}
				closesocket(arr[j].socketarr[i]);
				arr[j].socketarr[i] = arr[j].socketarr[arr[j].count - 1];//删除socket 套接字
				WSACloseEvent(arr[j].eventarr[i]);
				arr[j].eventarr[i] = arr[j].eventarr[arr[j].count - 1];//删除event   事件
				//数量减一
				arr[j].count--;//一定要记得写这个！
				printf("client close errod code %d\n", lpNetevents.iErrorCode[FD_CLOSE_BIT]);

			}
		}

		}

	}





	for (int j = 0; j < 20; j++)
	{


		for (int i = 0; i < arr[i].count; i++)
		{
			closesocket(arr[j].socketarr[i]);
			WSACloseEvent(arr[j].eventarr[i]);  //释放第j组第i个socket



		}

	}

	WSACleanup();


	system("pause");


	return 0;



}
