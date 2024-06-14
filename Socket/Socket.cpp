#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <thread>

#define BUF_SIZE 100
#define MAX_CLNT 256

void HandleClnt(SOCKET* Sock);
void SendMsg(char* msg, int len);
void ErrorHandling(const char *message);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
Mutex m;
HANDLE hMutex;

int main()
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSz;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hMutex = CreateMutex(NULL, FALSE, NULL);
    hServSock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(7777);

    if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    while (1)
    {
        clntAdrSz = sizeof(clntAdr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

        m.lock();
        clntSocks[clntCnt++] = hClntSock;
        m.unlock();

        thread(HandleClnt, &hClntSock).detach();
        printf("Connected client IP: %s \n", inet_ntoa(clntAdr.sin_addr));
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void HandleClnt(SOCKET* Sock)
{
    SOCKET hClntSock = *(Sock);
    int strLen = 0, i;
    char msg[BUF_SIZE];

    while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0)
        SendMsg(msg, strLen);

    m.lock();
    for (i = 0; i < clntCnt; i++) 
    {
        if (hClntSock == clntSocks[i])
        {
            while (i++ < clntCnt - 1)
                clntSocks[i] = clntSocks[i + 1];
            break;
        }
    }
    clntCnt--;
    m.unlock();
    closesocket(hClntSock);
}
void SendMsg(char* msg, int len) 
{
    int i;
    m.lock();
    for (i = 0; i < clntCnt; i++)
        send(clntSocks[i], msg, len, 0);
    m.unlock();
}

void ErrorHandling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

