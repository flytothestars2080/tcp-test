#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#define ip "127.0.0.1"
#define g_max 1024//socket���������
#define g_recymax 1024//�����ַ���������
#include<stdio.h>
#include<winsock2.h>
#include<mswsock.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
SOCKET ALLsocket[g_max];
WSAOVERLAPPED ALLOLP[g_max];//�ص��ṹ����
char recystr[g_recymax];
int g_count;
int postaccept();//Ͷ��acceptex���һᴴ��һ����socket����Ͷ��recy
int postrecy(int index);//Ͷ��recycy�����±�recy
void clear();//���socket���¼�����
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
	SOCKET socketserver = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL,0,WSA_FLAG_OVERLAPPED);
	if (socketserver == INVALID_SOCKET)
	{

		int a = WSAGetLastError();
		//���������
		printf("socketbuilderror%d\n", a);
		WSACleanup();
		return 0;

	}

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
	int l = listen(socketserver, SOMAXCONN);
	if (SOCKET_ERROR == listen(socketserver, SOMAXCONN))
	{
		int b = WSAGetLastError();//��ȡ������
		WSACleanup();
		system("pause");
		printf("listenerror%d", b);

	}
	ALLsocket[0] = socketserver;
	ALLOLP[g_count].hEvent=WSACreateEvent();//��ʼ��
	g_count++;
	if (postaccept() != 0)//Ͷ��acceptex�һᴴ��һ����socket���ص��ṹ����Ͷ��recy
	{
		clear();
		WSACleanup();
		return 0;
	}
	while (1)
	{
		for (int i=0; i < g_count; i++)
		{
			DWORD Dwindex=WSAWaitForMultipleEvents(1,&ALLOLP[i].hEvent,FALSE,0,TRUE);//���ź�
			if (Dwindex == WSA_WAIT_FAILED || WSA_WAIT_TIMEOUT == Dwindex)
			{
				continue;
			}
			//���ź���
			DWORD dwstate;//���ͺͽ��յ�״̬
			DWORD dwflag;
			BOOL bflag = WSAGetOverlappedResult(ALLsocket[i], &ALLOLP[i], &dwstate, TRUE, &dwflag);
			//�����ź�
			WSAResetEvent(ALLOLP[i].hEvent);
			//��ȡsocket�ϵľ������
			if (FALSE == bflag)//ʧ��
			{
				int a = WSAGetLastError();
				if (10054 == a)
				{

					printf("forced close\n");
					//�ر�socket��event
					closesocket(ALLsocket[i]);
					WSACloseEvent(ALLOLP[i].hEvent);
					//�Ӽ�¼������ɾ��
					ALLsocket[i] = ALLsocket[g_count - 1];
					ALLOLP[i] = ALLOLP[g_count - 1];
					//ѭ�����Ʊ�����һ
					i--;
					//������һ
					g_count--;
					if (g_count == 1)
					{
						postaccept();
					}
					
					
				}
				continue;
			}
			if (i == 0&& 0 == dwstate)//˵��������socket��accept
			{
				printf("accept\n");
				/*postsend(g_count);*/
				
				//wsarecy 
				postrecy(g_count); //
				// send
				//�����±�++
				
				g_count++;
				//�ٴ�accept
				postaccept();
				continue;
			}
			if (dwstate != 0)//���ͻ���ճɹ���
			{

				if (recystr[0] != 0)
				{
					printf("recy\n");
					printf("%s\n", recystr);
					//����ַ�
					memset(recystr, 0, g_recymax);
					//��������
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
				//�ͻ�����������
				//�ر�socket��event
				closesocket(ALLsocket[i]);
				WSACloseEvent(ALLOLP[i].hEvent);
				//�Ӽ�¼������ɾ��
				ALLsocket[i] = ALLsocket[g_count - 1];
				ALLOLP[i] = ALLOLP[g_count - 1];
				//ѭ�����Ʊ�����һ
				i--;
				//������һ
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
		//�����µĿͻ���socket
		ALLsocket[g_count] = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);//�ͻ��˵�socket
		ALLOLP[g_count].hEvent = WSACreateEvent();

		char str[1024];
		DWORD D = 0;
		BOOL B = AcceptEx(ALLsocket[0], ALLsocket[g_count], str, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &D, &ALLOLP[0]);//�������������Ͷ�ݸ�����ϵͳ
		if (B == TRUE)//�������
		{

			//wsarecy
			postrecy(g_count);//
			// send
			//�����±�++
			g_count++;
			//�ٴ�accept
		/*	postaccept();*/
			continue;
		}
		else
		{
			int a = GetLastError();
			if (a == ERROR_IO_PENDING)//�첽�ȴ�
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
	if (a == 0)//��������
	{

		printf("%s\n", buff.buf);
		//����ַ�
		memset(recystr, 0, g_recymax);
		//��������
		postrecy(index);
		return 0;
	}
	else
	{ 
		int c = WSAGetLastError();
		if (WSA_IO_PENDING == c)
		{
			//�ӳٴ���
			return 0;
		}
		else
		{
			return c;//�������
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
	buff.buf = "���";
	buff.len = g_recymax;
	DWORD number_send;
	DWORD  flag = 0;
	int a = WSASend(ALLsocket[index], &buff, 1, &number_send, flag, &ALLOLP[index], NULL);
	if (a == 0)//��������
	{

		printf("send�ɹ�\n" );
		//����ַ�
		return 0;
	}
	else
	{
		int c = WSAGetLastError();
		if (WSA_IO_PENDING == c)
		{
			//�ӳٴ���
			return 0;
		}
		else
		{
			return c;//�������
		}
	}
};


