#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <thread>

#define BUF_SIZE 1024
void CALLBACK ReadCallback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCallback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(const char *message);

typedef struct
{
    SOCKET hClntSock;
    char buf[BUF_SIZE];
    WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

int main()
{
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;
    LPWSAOVERLAPPED lpOvLp;
    DWORD recvBytes, mode = 1, flagInfo = 0;
    LPPER_IO_DATA hbInfo;
    int recvAdrSize;
 
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;


    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); // blocking socket
    ioctlsocket(hLisnSock, FIONBIO, &mode); // non-blocking socket

    memset(&lisnAdr, 0, sizeof(lisnAdr));
    lisnAdr.sin_family = AF_INET;
    lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnAdr.sin_port = htons(7777);

    if (bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hLisnSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    recvAdrSize = sizeof(recvAdr);
    while (1)
    {
        SleepEx(100, TRUE);
        hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSize); // repeat accpet()
        if (hRecvSock == INVALID_SOCKET)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK) // no connect() request
                continue;
            else
                ErrorHandling("accpet() error");
        }

        // Client connect() request
        puts("Client connected...");

        // Each Client have WSAOVERLAPPED struct
        lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
        memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));

        // hRecvSock info save to PER_IO_DATA struct
        hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        hbInfo->hClntSock = (DWORD)hRecvSock;
        (hbInfo->wsaBuf).buf = hbInfo->buf;
        (hbInfo->wsaBuf).len = BUF_SIZE;

        lpOvLp->hEvent = (HANDLE)hbInfo;
        WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOvLp/*callback 3rd parameter*/, ReadCallback);
        // Thanks to lpOvLp, callback function can access socket handle and buffer.
    }

    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

// Input Data is completed -> Echo data to client
void CALLBACK ReadCallback(DWORD dwError, DWORD RecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
    // 입력이 완료된 소켓의 핸들과 버퍼 정보 추출
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD sendBytes;

    if (RecvBytes == 0)
    {
        closesocket(hSock);
        free(lpOverlapped->hEvent);
        free(lpOverlapped);
        puts("Client disconnected...");
    }
    else
    {
        bufInfo->len = RecvBytes;
        WSASend(hSock, bufInfo, 1, &sendBytes, 0, lpOverlapped, WriteCallback);
    }
}

void CALLBACK WriteCallback(DWORD dwError, DWORD SendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD recvBytes, flagInfo = 0;

    WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, ReadCallback);
}

void ErrorHandling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

