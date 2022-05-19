#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#define ip "127.0.0.1"
#define g_max 1024//socket的最大数量
#define g_recymax 1024//接受字符最大的数量
#include<winsock2.h>
#include<stdio.h>
#include<mswsock.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
HANDLE  hport;
SOCKET ALLsocket[g_max];
HANDLE* hthread;
int nProcessorsCount;
WSAOVERLAPPED ALLOLP[g_max];//重叠结构数组
char recystr[g_recymax];
int g_count;
int postaccept(void);//投递acceptex并且会创建一个新socket并且投递recy
int postrecy(int index);//投递recycy根据下标recy
void clear(void);//清除socket和事件对象
int postsend(int index);
BOOL g_flag=TRUE;//控制线程的
DWORD WINAPI ThreadProc(LPVOID lpParameter);

BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:g_flag = FALSE;
		CloseHandle(hport);
		clear();
		for (int i = 0; i < nProcessorsCount; i++)
		{
			CloseHandle(hthread[i]);

		}
		break;
		
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
	SOCKET socketserver = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socketserver == INVALID_SOCKET)
	{

		int a = WSAGetLastError();
		//清理网络库
		printf("socketbuilderror%d\n", a);
		WSACleanup();
		return 0;

	}
	ALLsocket[0] = socketserver;
	ALLOLP[g_count].hEvent = WSACreateEvent();//初始化
	g_count++;

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
	//创建完成端口

	  hport =CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
	if (0 == hport)
	{
		int a = GetLastError();
		printf("%d", a);
		closesocket(socketserver);
		WSACleanup();
		return 0;
	}

	//绑定完成端口
	HANDLE  hport1 = CreateIoCompletionPort((HANDLE)socketserver, hport, 0, 0);//注意第三个参数是绑定标记
	if (hport1 != hport)
	{
		int a = GetLastError();
		printf("hport1-function error%d", a);
		CloseHandle(hport);
		closesocket(socketserver);
		WSACleanup();
		return 0;
	}


	int l = listen(socketserver, SOMAXCONN);
	if (SOCKET_ERROR == listen(socketserver, SOMAXCONN))
	{
		int b = WSAGetLastError();//获取错误码
		WSACleanup();
		system("pause");
		printf("listenerror%d", b);
		return 0;

	}

	if (postaccept() != 0)//投递acceptex且acept成功会创建一个新socket和重叠结构并且投递recy
	{
		clear();
		WSACleanup();
		return 0;
	}
	//核心数
	SYSTEM_INFO systemProcessorsCount;
	GetSystemInfo(&systemProcessorsCount);
	 nProcessorsCount = systemProcessorsCount.dwNumberOfProcessors;
	//创建线程数组
	 hthread =(HANDLE*)malloc(sizeof(HANDLE) * nProcessorsCount);
	if (hthread == NULL)
		return;
	for (int i = 0; i < nProcessorsCount; i++)
	{
		
	 hthread[i]=CreateThread(NULL,0,ThreadProc,hport,0,NULL);
	 if (NULL == hthread[i])//创建失败
	 {
		 int a = GetLastError();
		 printf("thread- error%d", a);
		 CloseHandle(hport);
		 closesocket(socketserver);
		 WSACleanup();
		 return 0;

	 }
	}
	//阻塞
	while (1)
	{
		Sleep(1000);

	}
	//释放线程句柄
	for (int i = 0; i < nProcessorsCount; i++)
	{
		CloseHandle(hthread[i]);
		
	}
	free(hthread);
	CloseHandle(hport);
	clear();
	WSACleanup();
	system("pause");


	return 0;

}

int postaccept(void)
{
	while (1)
	{
		//创建新的客户端socket
		ALLsocket[g_count] = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);//客户端的socket
		ALLOLP[g_count].hEvent = WSACreateEvent();

		char str[1024];
		DWORD D = 0;
		BOOL B = AcceptEx(ALLsocket[0], ALLsocket[g_count], str, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &D, &ALLOLP[0]);//把链接请求管理投递给操作系统

		int a = GetLastError();
		if (a != ERROR_IO_PENDING)//异步等待
		{

			return 1;
		}

		return 0;
	}
}
int postrecy(int index)
{
	WSABUF buff;
	buff.buf = recystr;
	buff.len = g_recymax;
	DWORD number_recy;
	DWORD  flag = 0;
	int a = WSARecv(ALLsocket[index], &buff, 1, &number_recy, &flag, &ALLOLP[index], NULL);
	
		int c = WSAGetLastError();
		if (WSA_IO_PENDING != c)
		{
			//出错了
			return 1;
		}
		return 0;
};
void clear(void)
{

	for (int i = 0; i < g_count; i++)
	{
		if (0== ALLsocket[i])
			continue;
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
	
		int c = WSAGetLastError();
		if (WSA_IO_PENDING != c)
		{
			//出错了
			return 1;
		}
		return 0;
};
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	HANDLE port = (HANDLE)lpParameter;
	DWORD     NumberOfBytes;
	ULONG_PTR index;
	LPOVERLAPPED  lpOverlapped;//重叠结构的指针
	while (g_flag)
	{
	BOOL bflag = GetQueuedCompletionStatus(port,&NumberOfBytes,&index,&lpOverlapped,INFINITE);// 获取请求-尝试从指定的I / O完成端口出列I / O完成数据包
		if (FALSE == bflag)
		{
			
			int a = GetLastError();
			if (a == 64)
				printf("force close\n");
			printf("error %d\n", a);
			continue;
		}
		//分类处理
		if (0 == index)//accept完成
		{
			printf("accept\n");
			//绑定完成端口
			HANDLE  hport1 = CreateIoCompletionPort((HANDLE)ALLsocket[g_count], hport,g_count, 0);//注意第三个参数是绑定标记
			if (hport1 != hport)
			{
				int a = GetLastError();
				printf("hport1-function error%d", a);
				closesocket(ALLsocket[g_count]);
				/*continue;*/
			}
			//recy
			postrecy(g_count);
			g_count++;
			//给新客户端投递accept
			postaccept();
		}
		else
		{
			if (NumberOfBytes == 0)//close
			{
				printf("close\n");
				//关闭
				closesocket(ALLsocket[index]);
				if(ALLOLP[index].hEvent!=0)
				WSACloseEvent(ALLOLP[index].hEvent);
				//从数组中删掉
				ALLsocket[index] = 0;
				ALLOLP[index].hEvent = NULL;
				
			}
			else
			{
				if (recystr[0] != '0')//recy
				{
					printf("recy\n");
					printf("%s\n", recystr);
					//清空
					memset(recystr, 0, sizeof(recystr));
					//再次recy
					postrecy(index);
				}
				else//send
				{
					printf("send ok\n");
					
				}
			}
		}
	}
	
	return 0;
}


