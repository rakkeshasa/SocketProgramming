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
    TIMEVAL timeout;
    fd_set reads, cpyReads;

    int adrSz;
    char buf[BUF_SIZE];
    int strLen, fdNum, i;

    SOCKADDR_IN servAdr, clntAdr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(7777);

    if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    FD_ZERO(&reads);
    FD_SET(hServSock, &reads);

    while (true)
    {
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
            break;

        if (fdNum == 0)
            continue;

        for (i = 0; i < reads.fd_count; i++)
        {
            if (FD_ISSET(reads.fd_array[i], &cpyReads))
            {
                if (reads.fd_array[i] == hServSock)
                {
                    adrSz = sizeof(clntAdr);
                    hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
                    FD_SET(hClntSock, &reads);
                    printf("connected client: %d\n", hClntSock);
                }
                else
                {
                    strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
                    if (strLen == 0)
                    {
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(cpyReads.fd_array[i]);
                        printf("close client: %d\n", cpyReads.fd_array[i]);
                    }
                    else
                    {
                        send(reads.fd_array[i], buf, strLen, 0);
                    }
                }
            }
        }
    }
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

