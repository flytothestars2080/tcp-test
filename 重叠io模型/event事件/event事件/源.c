#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#define ip "127.0.0.1"
#define g_max 1024//socket的最大数量
#define g_recymax 1024//接受字符最大的数量
#include<stdio.h>
#include<winsock2.h>
#include<mswsock.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
SOCKET ALLsocket[g_max];
WSAOVERLAPPED ALLOLP[g_max];//重叠结构数组
char recystr[g_recymax];
int g_count;
int postaccept();//投递acceptex并且会创建一个新socket并且投递recy
int postrecy(int index);//投递recycy根据下标recy
void clear();//清除socket和事件对象
int postsend(int index);

BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		break;

		clear();
	}

	return TRUE;
};
int main()
{
	SetConsoleCtrlHandler(fun, TRUE);
	WORD W = MAKEWORD(2, 2);
	WSADATA msg;//存放版本信息
	int b = WSAStartup(W, &msg);//启动网络库
	if (b != 0)
	{
		switch (b)
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
	SOCKET socketserver = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL,0,WSA_FLAG_OVERLAPPED);
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
	ALLsocket[0] = socketserver;
	ALLOLP[g_count].hEvent=WSACreateEvent();//初始化
	g_count++;
	if (postaccept() != 0)//投递acceptex且会创建一个新socket和重叠结构并且投递recy
	{
		clear();
		WSACleanup();
		return 0;
	}
	while (1)
	{
		for (int i=0; i < g_count; i++)
		{
			DWORD Dwindex=WSAWaitForMultipleEvents(1,&ALLOLP[i].hEvent,FALSE,0,TRUE);//等信号
			if (Dwindex == WSA_WAIT_FAILED || WSA_WAIT_TIMEOUT == Dwindex)
			{
				continue;
			}
			//有信号了
			DWORD dwstate;//发送和接收的状态
			DWORD dwflag;
			BOOL bflag = WSAGetOverlappedResult(ALLsocket[i], &ALLOLP[i], &dwstate, TRUE, &dwflag);
			//重置信号
			WSAResetEvent(ALLOLP[i].hEvent);
			//获取socket上的具体操作
			if (FALSE == bflag)//失败
			{
				int a = WSAGetLastError();
				if (10054 == a)
				{

					printf("forced close\n");
					//关闭socket和event
					closesocket(ALLsocket[i]);
					WSACloseEvent(ALLOLP[i].hEvent);
					//从记录数组中删掉
					ALLsocket[i] = ALLsocket[g_count - 1];
					ALLOLP[i] = ALLOLP[g_count - 1];
					//循环控制变量减一
					i--;
					//个数减一
					g_count--;
					if (g_count == 1)
					{
						postaccept();
					}
					
					
				}
				continue;
			}
			if (i == 0&& 0 == dwstate)//说明服务器socket上accept
			{
				printf("accept\n");
				/*postsend(g_count);*/
				
				//wsarecy 
				postrecy(g_count); //
				// send
				//计数下标++
				
				g_count++;
				//再次accept
				postaccept();
				continue;
			}
			if (dwstate != 0)//发送或接收成功了
			{

				if (recystr[0] != 0)
				{
					printf("recy\n");
					printf("%s\n", recystr);
					//清空字符
					memset(recystr, 0, g_recymax);
					//继续接收
					postrecy(i);


				}
				else
				{
					//send
				}
				continue;
			}
			if (0 == dwstate && i!=0)
			{
				printf("close\n");
				//客户端正常下线
				//关闭socket和event
				closesocket(ALLsocket[i]);
				WSACloseEvent(ALLOLP[i].hEvent);
				//从记录数组中删掉
				ALLsocket[i] = ALLsocket[g_count - 1];
				ALLOLP[i] = ALLOLP[g_count - 1];
				//循环控制变量减一
				i--;
				//个数减一
				g_count--;
				if (g_count == 1)
				{
					postaccept();
				}
				
			}
			
		}
		
	}
	
	clear();
	WSACleanup();
	system("pause");


	return 0;

}

int postaccept()
{
	while (1)
	{
		//创建新的客户端socket
		ALLsocket[g_count] = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);//客户端的socket
		ALLOLP[g_count].hEvent = WSACreateEvent();

		char str[1024];
		DWORD D = 0;
		BOOL B = AcceptEx(ALLsocket[0], ALLsocket[g_count], str, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &D, &ALLOLP[0]);//把链接请求管理投递给操作系统
		if (B == TRUE)//立即完成
		{

			//wsarecy
			postrecy(g_count);//
			// send
			//计数下标++
			g_count++;
			//再次accept
		/*	postaccept();*/
			continue;
		}
		else
		{
			int a = GetLastError();
			if (a == ERROR_IO_PENDING)//异步等待
			{
				break;
			}
			else
			{
				break;
			}

		}

	}
	return 0;
	
}
int postrecy(int index)
{
	WSABUF buff;
	buff.buf = recystr;
	buff.len = g_recymax;
	DWORD number_recy;
	DWORD  flag = 0;
	int a=WSARecv(ALLsocket[index], &buff, 1,&number_recy,&flag,&ALLOLP[index],NULL);
	if (a == 0)//立即返回
	{

		printf("%s\n", buff.buf);
		//清空字符
		memset(recystr, 0, g_recymax);
		//继续接收
		postrecy(index);
		return 0;
	}
	else
	{ 
		int c = WSAGetLastError();
		if (WSA_IO_PENDING == c)
		{
			//延迟处理
			return 0;
		}
		else
		{
			return c;//真出错了
		}
	}

};
void clear()
{

	for (int i = 0; i < g_count; i++)
	{
		closesocket(ALLsocket[i]);
		WSACloseEvent(ALLOLP[i].hEvent);

	}
};
int postsend(int index)
{

	WSABUF buff;
	buff.buf = "你好";
	buff.len = g_recymax;
	DWORD number_send;
	DWORD  flag = 0;
	int a = WSASend(ALLsocket[index], &buff, 1, &number_send, flag, &ALLOLP[index], NULL);
	if (a == 0)//立即返回
	{

		printf("send成功\n" );
		//清空字符
		return 0;
	}
	else
	{
		int c = WSAGetLastError();
		if (WSA_IO_PENDING == c)
		{
			//延迟处理
			return 0;
		}
		else
		{
			return c;//真出错了
		}
	}
};


