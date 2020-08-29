#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#define SERVERPORT 9000
#define BUFSIZE	   512
void ErrorHandling(char* message);

int main(int argc, char* argv[]) {
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandling((char*)"WSAStartup() error!");

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		ErrorHandling((char*)"socket() error");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		ErrorHandling((char*)"bind() error");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		ErrorHandling((char*)"listen() error");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, len;
	char buf[BUFSIZE + 1], yn;

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
			ErrorHandling((char*)"accept() error");

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			
			// ������ �ޱ�
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("   [from Client] >> %s\n", buf);

			// ������ �Է�
			printf("[���� �޽���] << ");
			if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
				break;

			// '\n' ���� ����
			len = strlen(buf);
			if (buf[len - 1] == '\n')
				buf[len - 1] = '\0';
			if (strlen(buf) == 0)
				break;

			// ������ ������
			retval = send(client_sock, buf, strlen(buf), 0);
		}
		// closesocket()
		closesocket(client_sock);
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		printf("������ �����ұ��? (y or n)\n");
		scanf_s("%c", &yn, sizeof(yn));
		if (yn == 'y') break;
		else continue;
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}