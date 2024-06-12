#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 30
#define TTL 64
void ErrorHandling(const char *message);

int main()
{
    WSADATA wsaData;
    SOCKET hSendSock;
    SOCKADDR_IN mulAdr;
    int timeLive = TTL;
    FILE* fp;
    char buf[BUF_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hSendSock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&mulAdr, 0, sizeof(mulAdr));
    mulAdr.sin_family = AF_INET;
    inet_pton(AF_INET, "224.1.1.2", &mulAdr.sin_addr.s_addr);
    mulAdr.sin_port = htons(7777);

    setsockopt(hSendSock, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&timeLive, sizeof(timeLive));
    if ((fp = fopen("news.txt", "r")) == NULL)
        ErrorHandling("fopen() error");

    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        sendto(hSendSock, buf, strlen(buf), 0, (SOCKADDR*)&mulAdr, sizeof(mulAdr));
        Sleep(2000);
    }
    closesocket(hSendSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

