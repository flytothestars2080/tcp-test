#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#define ip "127.0.0.1"
#define g_max 1024//socket���������
#define g_recymax 1024//�����ַ���������
#include<winsock2.h>
#include<stdio.h>
#include<mswsock.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
HANDLE  hport;
SOCKET ALLsocket[g_max];
HANDLE* hthread;
int nProcessorsCount;
WSAOVERLAPPED ALLOLP[g_max];//�ص��ṹ����
char recystr[g_recymax];
int g_count;
int postaccept(void);//Ͷ��acceptex���һᴴ��һ����socket����Ͷ��recy
int postrecy(int index);//Ͷ��recycy�����±�recy
void clear(void);//���socket���¼�����
int postsend(int index);
BOOL g_flag=TRUE;//�����̵߳�
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
	WSADATA msg;//��Ű汾��Ϣ
	int b = WSAStartup(W, &msg);//���������
	if (b != 0)
	{
		switch (b)
		{
		case WSASYSNOTREADY:printf("������ϵͳ");
			break;
		case WSAVERNOTSUPPORTED:printf("����汾");
			break;
		case WSAEINPROGRESS:printf("���������");
			break;
		case WSAEPROCLIM:printf("��ص�һЩ���");
			break;
		case WSAEFAULT:printf("��������");
			break;
		}
		return 0;
	}
	if (2 != HIBYTE(msg.wVersion) || 2 != LOBYTE(msg.wVersion))
	{
		WSACleanup();

		printf("�����汾����\n");
		return 0;
	}
	SOCKET socketserver = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socketserver == INVALID_SOCKET)
	{

		int a = WSAGetLastError();
		//���������
		printf("socketbuilderror%d\n", a);
		WSACleanup();
		return 0;

	}
	ALLsocket[0] = socketserver;
	ALLOLP[g_count].hEvent = WSACreateEvent();//��ʼ��
	g_count++;

	struct sockaddr_in si;
	si.sin_family = AF_INET;//IP���� 
	si.sin_port = htons(12345);//�˿ں�
	si.sin_addr.S_un.S_addr = inet_addr(ip);//ip ��ַ
	int t = bind(socketserver, (const struct sockaddr*)&si, sizeof(si));
	if (SOCKET_ERROR == t)
	{
		int b = WSAGetLastError();//��ȡ������
		WSACleanup();
		system("pause");
		printf("binderror%d", b);
		return 0;
	}
	//������ɶ˿�

	  hport =CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
	if (0 == hport)
	{
		int a = GetLastError();
		printf("%d", a);
		closesocket(socketserver);
		WSACleanup();
		return 0;
	}

	//����ɶ˿�
	HANDLE  hport1 = CreateIoCompletionPort((HANDLE)socketserver, hport, 0, 0);//ע������������ǰ󶨱��
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
		int b = WSAGetLastError();//��ȡ������
		WSACleanup();
		system("pause");
		printf("listenerror%d", b);
		return 0;

	}

	if (postaccept() != 0)//Ͷ��acceptex��acept�ɹ��ᴴ��һ����socket���ص��ṹ����Ͷ��recy
	{
		clear();
		WSACleanup();
		return 0;
	}
	//������
	SYSTEM_INFO systemProcessorsCount;
	GetSystemInfo(&systemProcessorsCount);
	 nProcessorsCount = systemProcessorsCount.dwNumberOfProcessors;
	//�����߳�����
	 hthread =(HANDLE*)malloc(sizeof(HANDLE) * nProcessorsCount);
	if (hthread == NULL)
		return;
	for (int i = 0; i < nProcessorsCount; i++)
	{
		
	 hthread[i]=CreateThread(NULL,0,ThreadProc,hport,0,NULL);
	 if (NULL == hthread[i])//����ʧ��
	 {
		 int a = GetLastError();
		 printf("thread- error%d", a);
		 CloseHandle(hport);
		 closesocket(socketserver);
		 WSACleanup();
		 return 0;

	 }
	}
	//����
	while (1)
	{
		Sleep(1000);

	}
	//�ͷ��߳̾��
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
		//�����µĿͻ���socket
		ALLsocket[g_count] = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);//�ͻ��˵�socket
		ALLOLP[g_count].hEvent = WSACreateEvent();

		char str[1024];
		DWORD D = 0;
		BOOL B = AcceptEx(ALLsocket[0], ALLsocket[g_count], str, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &D, &ALLOLP[0]);//�������������Ͷ�ݸ�����ϵͳ

		int a = GetLastError();
		if (a != ERROR_IO_PENDING)//�첽�ȴ�
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
			//������
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
	buff.buf = "���";
	buff.len = g_recymax;
	DWORD number_send;
	DWORD  flag = 0;
	int a = WSASend(ALLsocket[index], &buff, 1, &number_send, flag, &ALLOLP[index], NULL);
	
		int c = WSAGetLastError();
		if (WSA_IO_PENDING != c)
		{
			//������
			return 1;
		}
		return 0;
};
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	HANDLE port = (HANDLE)lpParameter;
	DWORD     NumberOfBytes;
	ULONG_PTR index;
	LPOVERLAPPED  lpOverlapped;//�ص��ṹ��ָ��
	while (g_flag)
	{
	BOOL bflag = GetQueuedCompletionStatus(port,&NumberOfBytes,&index,&lpOverlapped,INFINITE);// ��ȡ����-���Դ�ָ����I / O��ɶ˿ڳ���I / O������ݰ�
		if (FALSE == bflag)
		{
			
			int a = GetLastError();
			if (a == 64)
				printf("force close\n");
			printf("error %d\n", a);
			continue;
		}
		//���ദ��
		if (0 == index)//accept���
		{
			printf("accept\n");
			//����ɶ˿�
			HANDLE  hport1 = CreateIoCompletionPort((HANDLE)ALLsocket[g_count], hport,g_count, 0);//ע������������ǰ󶨱��
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
			//���¿ͻ���Ͷ��accept
			postaccept();
		}
		else
		{
			if (NumberOfBytes == 0)//close
			{
				printf("close\n");
				//�ر�
				closesocket(ALLsocket[index]);
				if(ALLOLP[index].hEvent!=0)
				WSACloseEvent(ALLOLP[index].hEvent);
				//��������ɾ��
				ALLsocket[index] = 0;
				ALLOLP[index].hEvent = NULL;
				
			}
			else
			{
				if (recystr[0] != '0')//recy
				{
					printf("recy\n");
					printf("%s\n", recystr);
					//���
					memset(recystr, 0, sizeof(recystr));
					//�ٴ�recy
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


