#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#define SERVERPORT 9000
#define BUFSIZE	   512
void ErrorHandling(char* message);

int main(int argc, char* argv[]) {
	int retval;

	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
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

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		while (1) {
			
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("   [from Client] >> %s\n", buf);

			// 데이터 입력
			printf("[보낼 메시지] << ");
			if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
				break;

			// '\n' 문자 제거
			len = strlen(buf);
			if (buf[len - 1] == '\n')
				buf[len - 1] = '\0';
			if (strlen(buf) == 0)
				break;

			// 데이터 보내기
			retval = send(client_sock, buf, strlen(buf), 0);
		}
		// closesocket()
		closesocket(client_sock);
		printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		printf("서버도 종료할까요? (y or n)\n");
		scanf_s("%c", &yn, sizeof(yn));
		if (yn == 'y') break;
		else continue;
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}