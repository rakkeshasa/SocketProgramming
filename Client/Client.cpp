#include <pch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void SendMsg(SOCKET* Sock);
void RecvMsg(SOCKET* Sock);
void ErrorHandling(const char* message);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];
Mutex m;

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;

	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	sprintf(name, "[%s]", argv[3]);
	hSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servAdr.sin_addr);
	servAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");

	thread sndThread(SendMsg, &hSock);
	thread rcvThread(RecvMsg, &hSock);

	sndThread.join();
	rcvThread.join();

	closesocket(hSock);
	WSACleanup();
	return 0;
}

void SendMsg(SOCKET* Sock)
{
	SOCKET hSock = *(Sock);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	while (1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(hSock);
			exit(0);
		}
		sprintf(nameMsg, "%s %s", name, msg);
		send(hSock, nameMsg, strlen(nameMsg), 0);
	}
}

void RecvMsg(SOCKET* Sock)
{
	int hSock = *(Sock);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int strLen;
	while (1)
	{
		strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (strLen == -1)
			break;
		nameMsg[strLen] = 0;
		fputs(nameMsg, stdout);
	}
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
