#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <thread>

#define BUF_SIZE 1024
void ErrorHandling(const char *message);

int main()
{
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;
    int recvAdrSize;

    WSABUF dataBuf;
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;

    char buf[BUF_SIZE];
    DWORD recvBytes = 0, flags = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    memset(&lisnAdr, 0, sizeof(lisnAdr));
    lisnAdr.sin_family = AF_INET;
    lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnAdr.sin_port = htons(7777);

    if (bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hLisnSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    recvAdrSize = sizeof(recvAdr);
    hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSize);

    evObj = WSACreateEvent();
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;
    dataBuf.len = BUF_SIZE;
    dataBuf.buf = buf;

    if (WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            puts("WSA_IO_PENDING!!");
            puts("데이터 수신 중... 기다려주세요");
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
            WSAGetOverlappedResult(hRecvSock, &overlapped, &recvBytes, FALSE, NULL);
        }
        else
        {
            ErrorHandling("WSARecv() error");
        }
    }

    printf("받은 메시지: %s \n", buf);
    WSACloseEvent(evObj);
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

