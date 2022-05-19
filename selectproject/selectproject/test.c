#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<WinSock2.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//��ʾ����Ws2_32.lib�����
fd_set allsockets;//����һ������
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
	SetConsoleCtrlHandler(fun,TRUE);//�ص������ò���ϵͳ���еĺ���


	WORD wdversion = MAKEWORD(2, 2);//��ȡ�汾
	WSADATA wdsockmsg;//��Ű汾��Ϣ
	int i = WSAStartup(wdversion, &wdsockmsg);
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
	else printf("�����򿪳ɹ�\n");

	if (2 != HIBYTE(wdsockmsg.wVersion) || 2 != LOBYTE(wdsockmsg.wVersion))
	{

		WSACleanup();
		printf("�����汾����");
		return 0;


	}

	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, 0);
	if (socketserver == INVALID_SOCKET)
	{

		int a = WSAGetLastError();
		//���������
		WSACleanup();
		return 0;


	}
	else {
		printf("socket �����ɹ�\n");
	}

	struct sockaddr_in si;
	si.sin_family = AF_INET;//IP���� ipv4ot ipv6
	si.sin_port = htons(12345);//�˿ں�
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//ip ��ַ


	int t = bind(socketserver, (const struct sockaddr*)&si, sizeof(si));//��
	if (SOCKET_ERROR == t)
	{
		int b = WSAGetLastError();//��ȡ������
		WSACleanup();
		system("pause");
		printf("error%d", b);
		return 0;
	}
	int l = listen(socketserver, SOMAXCONN);//��ʼ����
	if (SOCKET_ERROR == listen(socketserver, SOMAXCONN))
	{
		int b = WSAGetLastError();//��ȡ������
		WSACleanup();
		system("pause");
		printf("error%d", b);
		return 0;

	}
	
	FD_ZERO(&allsockets);//����������
	FD_SET(socketserver, &allsockets);//�ѷ�����socket��װ��ȥ
	while (1)
	{
		fd_set readsockets = allsockets;//��ʼ��
		fd_set writesockets = allsockets;
		fd_set errorsockets = allsockets;
		//����ȴ�ʱ��
		struct timeval s;
		s.tv_sec = 3;//��
		s.tv_usec = 0;//����
		int nes=select(0, &readsockets, &writesockets,&errorsockets,&s);
		if (0 == nes)
		{
			continue;
		}
		else if (nes > 0)
		{
			//����Ӧ

			for (u_int i = 0; i < errorsockets.fd_count; i++)//����errorsockets����
			{
				char buff[100] = {0};
				int len=0;
				if (SOCKET_ERROR == getsockopt(writesockets.fd_array[i], SOL_SOCKET, SO_ERROR, buff, &len))
				{
					printf("���ش���ʧ��\n");

				}
				else
				{
					if(buff[3]!=' ') ///����Ƿ������Ϊ������ͻ᷵��' '
					printf("%s \n", buff );

				}
				

			}


				for (u_int i = 0; i < writesockets.fd_count; i++)//����writesockets����
				{

					//printf("socketserver %d writesockets %d\n", socketserver, writesockets.fd_array[i]);
					/*if (send(writesockets.fd_array[i], "ok", sizeof("ok"), 0) == SOCKET_ERROR)
					{

						int a=GetLastError();
					}
					
					*/
				}
			for (u_int i = 0; i < readsockets.fd_count; i++)//����readsockets����
			{
				if (readsockets.fd_array[i] == socketserver)//˵���������з�Ӧ��
				{
					//Ҫaccept
					SOCKET socketclient = accept(socketserver, NULL, NULL);
					if (socketclient == INVALID_SOCKET)
					{

						//������
						WSAGetLastError();
						continue;
					}
					else
					{
						send(socketclient, "�����ӵ�������", sizeof("�����ӵ�������"), 0);

					}
					FD_SET(socketclient,&allsockets);



				}
				else//�ͻ����з�Ӧ
				{
			
					char buff[1500] = {0};

				int Recy=recv(readsockets.fd_array[i],buff,sizeof(buff),0);
				if (Recy == 0)
				{
					SOCKET sockettemp = readsockets.fd_array[i];
					FD_CLR(readsockets.fd_array[i], &allsockets);
					closesocket(sockettemp);
					printf("�ͻ���������");
					//�ͻ�������
				}
				else if (Recy == SOCKET_ERROR)
				{ //Զ������ǿ�ȹر���һ�����е����ӡ� 10054
					int a=WSAGetLastError();//������
					printf("%d\n", a);
					
					switch (a)
					{
					case 10054: 

					{
						SOCKET sockettemp = readsockets.fd_array[i];
						FD_CLR(readsockets.fd_array[i], &allsockets);
						closesocket(sockettemp);
						printf("�ͻ����ѱ�ǿ������\n");

					}break;
						
					}
				}
				else 
				{
					printf("%s\n", buff);//��ӡ�յ�����Ϣ
					/*char b[100];
					int n=0;
					printf("�Ƿ���Ҫ��ͻ��˷�����Ϣ����Ҫ������0\n");
					scanf("%d", &n);

						if (n!=0)
					{
							printf("�����룺\n");
							scanf("%s", b);
							send(readsockets.fd_array[i], b, 100, 0);

					}
					else send(readsockets.fd_array[i], "���������յ�\n",sizeof("���������յ�\n"), 0);*/
					send(readsockets.fd_array[i], "���������յ�", sizeof("���������յ�"), 0);
				}
				
				}
				



			}

			

		}
		else //nes<0
		{
	int a=GetLastError();
	printf("selecterror%d\n",a);
	break;
			//������

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