#include <pch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define BUF_SIZE 1024
void ErrorHandling(const char* message);

int main()
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAdr;
	char message[BUF_SIZE];
	int strLen, readLen;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &sendAdr.sin_addr);
	sendAdr.sin_port = htons(7777);

	if (connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");
	else
		puts("Connected...........");

	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		strLen = strlen(message);
		send(hSocket, message, strLen, 0);
		readLen = 0;
		while (1)
		{
			readLen += recv(hSocket, &message[readLen], BUF_SIZE - 1, 0);
			if (readLen >= strLen)
				break;
		}
		message[strLen] = 0;
		printf("Message from server: %s", message);
	}
	
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
