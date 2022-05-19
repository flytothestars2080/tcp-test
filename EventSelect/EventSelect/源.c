#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#define ip "127.0.0.1"
#include<stdio.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
struct fd_sset
{
	unsigned short count;//��Ч����
	SOCKET socketarr[WSA_MAXIMUM_WAIT_EVENTS];//socket ����
	WSAEVENT eventarr[WSA_MAXIMUM_WAIT_EVENTS];//eventarr �¼�����


};
struct fd_sset arr = { 0,{0},{NULL} };//������������ �¼���socket
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
	{
		for (int i = 0; i < arr.count; i++)
		{
			WSACloseEvent(arr.eventarr[i]);
			closesocket(arr.socketarr[i]);



		}
	}
		break;

		WSACleanup();
	}

	return TRUE;
};

int main()
{
	SetConsoleCtrlHandler(fun, TRUE);//�ص������ò���ϵͳ���еĺ���
	WORD W = MAKEWORD(2,2);
	WSADATA msg;//��Ű汾��Ϣ
	int i=WSAStartup(W,&msg);//���������
	if (i != 0)
	{
		switch (i)
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
	SOCKET socketserver=socket(AF_INET, SOCK_STREAM,0);
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




	







	WSAEVENT eventServer =WSACreateEvent();//�����¼����
	if (eventServer==WSA_INVALID_EVENT)
	{
		int b = WSAGetLastError();
		//��ȡ������
		closesocket(socketserver);
		WSACleanup();
		system("pause");
		printf("buildeventerror%d", b);
		return 0;
	}
	if (SOCKET_ERROR == WSAEventSelect(socketserver, eventServer, FD_ACCEPT))//��socket�;�����ӵ�һ�� ��Windowsϵͳͨ�����������socket
	{

		int b = WSAGetLastError();
		//��ȡ������
		WSACloseEvent(eventServer);
		closesocket(socketserver);
		WSACleanup();
		system("pause");
		printf("eventselecterror%d", b);
		return 0;

	}
	arr.socketarr[arr.count] =socketserver;
	arr.eventarr[arr.count] = eventServer;    //�ֱ�װ����Ӧ������
	arr.count++;


	while (1)
	{
		DWORD nes= WSAWaitForMultipleEvents(arr.count,arr.eventarr,FALSE, WSA_INFINITE,FALSE);//�����ϵͳѯ���¼��Ƿ���
			
		if (nes == WSA_WAIT_FAILED)
		{
			//������
			int a = WSAGetLastError();
			printf("wsawaitforerror%d",a);
			break;
			///#define WAIT_FAILED ((DWORD)0xFFFFFFFF)
		
		}
		/*if (nes == WSA_WAIT_TIMEOUT)//���������д�����ʱ����123ɶ�ľͼӸ����
		{

			continue;
		}*/
		DWORD nIndex = nes - WSA_WAIT_EVENT_0;//��С������Ӧ�¼�������±�



		WSANETWORKEVENTS lpNetevents;//�õ��±��Ӧ���¼�
		if (SOCKET_ERROR == WSAEnumNetworkEvents(arr.socketarr[nIndex ], arr.eventarr[nIndex], &lpNetevents))
		{

			int a = WSAGetLastError();

			printf("wsaenumnetworkevent error %d", a);
			break;
		}


		if (lpNetevents.lNetworkEvents & FD_ACCEPT)//��������ź�����accept����
		{
			if (lpNetevents.iErrorCode[FD_ACCEPT_BIT] == 0)//���û�г���
			{
				SOCKET socketclient = accept(arr.socketarr[nIndex], NULL, NULL);//�����ͻ���
				if (socketclient == INVALID_SOCKET)
				{

					
						continue;
				}
				WSAEVENT clientevent = WSACreateEvent();//�����¼�����
				if (clientevent == WSA_INVALID_EVENT)
				{
					closesocket(socketclient);
					continue;
				}
				if (SOCKET_ERROR == WSAEventSelect(socketclient, clientevent, FD_WRITE | FD_CLOSE | FD_READ))//Ͷ�ݸ�ϵͳ
				{
					closesocket(socketclient);
						WSACloseEvent(clientevent);
						continue;
				}
					//װ���ṹ��
				arr.socketarr[arr.count] = socketclient;
				arr.eventarr[arr.count] = clientevent;
				arr.count++;
				printf("accept event\n");
			}
			else 
			{
				continue;

			}




		}
		 if (lpNetevents.lNetworkEvents & FD_WRITE)//���Է���Ϣ���ͻ�����
		{
			if (lpNetevents.iErrorCode[FD_WRITE_BIT] == 0)//û����
			{
				if (SOCKET_ERROR == send(arr.socketarr[nIndex], " connect success", strlen("connect success"), 0))
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
		


		 if (lpNetevents.lNetworkEvents & FD_READ)//�ͻ��˷���Ϣ��
		{
			if (lpNetevents.iErrorCode[FD_READ_BIT] == 0)
			{
				char buff[1500] = {0};

				if (SOCKET_ERROR == recv(arr.socketarr[nIndex], buff, 1499, 0))
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


	 if (lpNetevents.lNetworkEvents & FD_CLOSE)//�ͻ�������
		{
			/*if (lpNetevents.iErrorCode[FD_CLOSE_BIT] == 0)
			{

			}
			else 
			{
				
			}*/
			printf("client close\n");
			if (arr.count == 0)
			{
				break;
			}
			closesocket(arr.socketarr[nIndex]);
			arr.socketarr[nIndex] = arr.socketarr[arr.count - 1];//ɾ��socket �׽���
			WSACloseEvent(arr.eventarr[nIndex]);
			arr.eventarr[nIndex] =arr.eventarr[arr.count-1 ];//ɾ��event   �¼�
			//������һ
			arr.count--;//һ��Ҫ�ǵ�д�����
			printf("client close errod code %d\n", lpNetevents.iErrorCode[FD_CLOSE_BIT]);

		}
		
	

	}

	for (int i = 0; i < arr.count; i++)
	{
		WSACloseEvent(arr.eventarr[i]);
		closesocket(arr.socketarr[i]);



	}




	 WSACloseEvent(eventServer);//�ͷ��¼����
	




	closesocket(socketserver);

	WSACleanup();


	system("pause");


	return 0;



}

