#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<WinSock2.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//��ʾ����Ws2_32.lib�����
int main()
{
	WORD wdversion = MAKEWORD(2, 2);//��ȡ�汾
	WSADATA wdsockmsg;//��Ű汾��Ϣ
	int wt = WSAStartup(wdversion, &wdsockmsg);//�������
	if (wt != 0)
	{
		switch (wt)
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

	if (2 != HIBYTE(wdsockmsg.wVersion) || 2 != LOBYTE(wdsockmsg.wVersion))//У��汾
	{

		WSACleanup();
		printf("�����汾����");
		return 0;


	}

	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, 0);//����socket
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
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//ip��ַ
	if (SOCKET_ERROR == connect(socketserver, (struct sockaddr*)&si, sizeof(si)))
	{
		int a = WSAGetLastError();
		//���������
		printf("%d", a);
		closesocket(socketserver);
		WSACleanup();
		return 0;


	}
	else
	{
		printf("���������ӳɹ�\n");
	}

	
	while (1)
	{
		char buff[1500] = { 0 };//�ǵó�ʼ��
		//int res = recv(socketserver, buff, 1024, 0);
		//if (0 == res)
		//{
		//	printf("�����ж�\n");

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
				printf("�������ѹر�\n");
			}
		
		}
	}



	closesocket(socketserver);
	WSACleanup();
	system("pause");
	return 0;
}