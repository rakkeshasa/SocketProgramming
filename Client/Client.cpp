#include <pch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ErrorHandling(const char* message);

int main()
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAdr;

	WSABUF dataBuf;
	char msg[] = "작중에서는 사고가 굳어진 어른들이 모자라고만 생각했다고 묘사된다. 그러나 <어린 왕자>가 베스트셀러로 유명해지면서 '코끼리를 잡아먹은 보아뱀' 이야기가 널리 퍼져 아이든 어른이든 그 유명한 그림을 보여줬을 때 모두 '코끼리를 잡아먹은 보아뱀'이라고 답하게 되었다고 한다. 이에 대해 작가인 생텍쥐페리는 오히려 이 사실에 절망감을 느꼈다고 한다. 생텍쥐페리는 이 이야기를 통해 어린이들의 동심에서 비롯된 '틀에 얽매이지 않는 생각'을 말하고자 했지만 정작 이 우화가 '또 다른 생각의 틀'을 만들었기 때문이었다. 클리셰 파괴를 위한 클리셰 작중 삽화가 대부분의 판본에서 동일한데 그 이유는 작가가 직접 그렸기 때문이다.작중 화자 스스로가 자신은 보아뱀 그림 이후에 그림 공부를 한 적이 없다고 말했고, 어린 왕자가 화자가 그린 그림을 보고 이상하게 그렸다고 지적하는 부분도 있을 정도로 아주 잘 그렸다고는 말하기 힘들지만 그럼에도 묘한 매력이 있다.근래에는 다른 작가가 그린 화려한 일러스트로 바꿔서 출판되는 경우도 많지만, 역시 원작의 그림을 선호하는 사람들이 많다.오히려 작가가 책을 집필할 때 같이 그렸기 때문에 그림도 작품의 일부라고 할 수 있으며";
	DWORD sendBytes = 0;

	WSAEVENT evObj;
	WSAOVERLAPPED overlapped;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &sendAdr.sin_addr);
	sendAdr.sin_port = htons(7777);

	if (connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");
	else
		puts("Connected...........");

	evObj = WSACreateEvent();
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = evObj;
	dataBuf.len = strlen(msg) + 1;
	dataBuf.buf = msg;

	if (WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("데이터 송신 중...");
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
		}
		else
		{
			ErrorHandling("WSASend() error");
		}
	}

	printf("보낸 데이터의 크기: %d \n", sendBytes);
	WSACloseEvent(evObj);
	
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
