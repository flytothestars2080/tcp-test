
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
	struct sockaddr_in clientmsg;
	int len = sizeof(clientmsg);

	SOCKET socketclient = accept(socketserver, (struct sockaddr*)&clientmsg, &len);//��ʼ����  ����һ��socket����װ�ͻ��˵���Ϣ
	if (INVALID_SOCKET == socketclient)
	{
		int b = WSAGetLastError();//��ȡ������
		WSACleanup();
		system("pause");
		printf("error%d", b);
		return 0;

	}
	else printf("�ͻ������ӳɹ�\n");



	if (SOCKET_ERROR == send(socketclient, "��ӭ���ʹ��", sizeof("��ӭ���ʹ��"), 0))
	{
		int a = SOCKET_ERROR;
		printf("socketsenderror %d\n", a);
	}
	while (1) 
	{
		char buff[1500] = { 0 };//�ǵó�ʼ��
		int res = recv(socketclient, buff, 1024, 0);
		if (0 == res)
		{
			printf("�����ж�\n");

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