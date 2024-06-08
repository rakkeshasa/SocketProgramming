#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024
void ErrorHandling(const char *message);

int main()
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    char message[BUF_SIZE];
    int strLen, i;

    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSize;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(7777);

    if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    clntAdrSize = sizeof(clntAdr);

    hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);
    if (hClntSock == -1)
        ErrorHandling("accept() error");
    else
        printf("Connect client \n");

    while ((strLen = recv(hClntSock, message, BUF_SIZE - 1, 0)) != 0)
        send(hClntSock, message, strLen, 0);

    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

