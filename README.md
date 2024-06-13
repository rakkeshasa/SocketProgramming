# 소켓프로그래밍

## 필요한 개념들
<strong>소켓</strong></BR>
물리적으로 연결된 네트워크 상에서의 데이터 송수신에 사용할 수 있는 소프트웨어적인 장치를 의미한다. 즉, 네트워크 망의 연결에 사용되는 도구다.</BR>
전화기로 예시를 들 수 있다. 전화기는 전화망을 통해서 음성데이터를 주고 받는 도구인데 소켓은 네트워크 망을 통해 데이터를 주고 받는 도구이다.</BR></BR>

<strong>TCP의 특징</strong></BR>
중간에 데이터가 소멸되지 않고 목적지로 전송된다.(신뢰성)</BR>
전송 순서대로 데이터가 수신된다.(순서성)</BR>
전송되는 데이터의 경계가 존재하지 않는다.(경계가 없다)</BR></BR>

<strong>빅 엔디안과 리틀 엔디안</strong></BR>
빅 엔디안은 상위 바이트의 값을 작은 번지수에 저장하는 방식으로 0x12345678을 저장하면 최상위 바이트인 0x12가 작은 번지수에 저장되고
0x78이 큰 번지수에 저장된다.</BR>
리틀 엔디안은 상위 바이트의 값을 큰 번지수에 저장하는 방식으로 최상위 바이트인 0x12가 큰 번지수에 저장되고 0x78이 작은 번지수에 저장된다.</BR>
데이터 저장방식이 다른 CPU에서 데이터를 주고 받을 시 보낸 데이터가 역순으로 저장될 수 있어 네트워크 바이트 순서는 빅 엔디안으로 통일한다.</BR>

## TCP 방식의 에코서버 구현
<strong>1. 서버 구현</strong>
```
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
```
<strong>WSAStartup()</strong>를 호출하여 프로그램에서 요구하는 윈도우 소켓의 버전을 알리고, 
해당 버전을 지원하는 라이브러리의 초기화 작업을 진행합니다.</BR>
MAKEWORD(2, 2)는 윈도우 소켓 2.2 버전을 사용한다는 의미입니다.</BR>
&wsaData는 WSADATA라는 구조체 변수의 주소 값을 전달합니다. <strong>WSAStartup()</strong>이 호출되고 나서 초기화된 라이브러리의 정보가 채워지는 구조체입니다.</BR></BR>

<strong>socket()</strong>을 통해 서버의 문지기를 담당할 서버 소켓을 생성합니다. </BR>
PF_INET은 IPv4 인터넷 프로토콜 체계를 의미하며, SOCK_STREAM은 연결지향형 소켓을 의미합니다.</BR>
IPv4 인터넷 프로토콜과 연결지향형 데이터 전송을 만족하는 프로토콜은 TCP이므로 이 소켓은 TCP 전송 방식 소켓입니다.</BR>
따라서 세번째 인자를 0으로 넣어 IPPROTO_TCP를 생략할 수 있습니다.</BR></BR>

이후 <strong>memset()</strong>함수를 통해 구조체 변수 servAdr의 모든 멤버를 0으로 초기화하고</BR>
(특히, sockaddr_in의 멤버 sin_zero를 0으로 초기화하기 위함)</BR>
주소체계와 IP주소, PORT번호를 초기화 해줍니다. </BR>
여기서 htons()는 호스트의 바이트 순서를 네트워크 바이트 순서인 빅엔디안에 맞춰서 변환해주고, INADDR_ANY는 소켓이 동작하는 컴퓨터의 IP주소를 의미합니다.</BR></BR>

<strong>bind()</strong>를 호출하여 생성한 서버 소켓에 IP주소와 PORT번호를 할당합니다.

<strong>listen()</strong>함수호출을 통해 서버 소켓 생성을 완료하고, 서버 소켓이 클라이언트한테 connect를 받을 수 있는 상태가 됩니다.</BR>
클라이언트가 서버에게 connect를 요청하면 서버에서 클라이언트용 소켓을 생성하기 전까지 연결요청 대기 큐에서 잠시 대기시킵니다.</BR>
listen()의 2번째 인자인 5는 연결요청 대기 큐의 크기입니다.</BR></BR>

<strong>2. 서버측에서 데이터 송수신하기</strong>
```
hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);
if (hClntSock == -1)
    ErrorHandling("accept() error");
else
    printf("Connect client\n");

while ((strLen = recv(hClntSock, message, BUF_SIZE, 0)) != 0)
    send(hClntSock, message, strLen, 0);
        
closesocket(hClntSock);
closesocket(hServSock);
WSACleanup();
```
<strong>accept()</strong>은 연결요청 대기 큐에서 대기중인 클라이언트의 연결요청을 수락하는 역할을 합니다.</BR>
따라서 호출성공 시 내무적으로 데이터 입출력에 사용할 서버용 클라이언트 소켓을 자동으로 생성하고 연결요청 한 클라이언트의 소켓에 연결이 이루어집니다.</BR></BR>

여기서 구현한 accept()은 클라이언트의 연결요청이 있을 때까지 함수를 반환하지 않으므로 연결요청이 오기 전까지 기다리므로 블로킹함수입니다.</BR></BR>

while문을 통해 서버가 클라이언트에게 받은 데이터를 그대로 클라이언트에게 보내 에코서버를 구현했습니다.</BR></BR>

<strong>3. 클라이언트 구현하기</strong>
```
if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
    ErrorHandling("connect() error!");
else
    puts("Connected...........");

while (1)
{
    fputs("Input message(Q to quit): ", stdout);
    fgets(message, BUF_SIZE, stdin);

    if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        break;

    send(hSocket, message, strlen(message), 0);
    strLen = recv(hSocket, message, BUF_SIZE - 1, 0);
    message[strLen] = 0;
    printf("Message from server: %s", message);
}

closesocket(hSocket);
WSACleanup();
```
서버와 같이 소켓을 생성하고 <strong>connect함수</strong>를 통해 서버에 연결요청을 합니다.</br>
서버에서 accept을 했다면 while문 내의 코드들을 동작하게 되고, q나 Q를 입력하면 while문을 종료합니다.</br>
<strong>send()</strong>를 통하여 서버에게 데이터를 보내고 <strong>recv()</strong>를 통해 서버가 보낸 데이터를 받습니다.</br></br>

## 멀티플렉싱 서버
멀티플렉싱 서버는 프로세스나 스레드를 여러개 생성하지 않고, <strong>단 하나의 서버 프로세스</strong>로 여러 클라이언트 소켓들의 요청을 처리하는 서버입니다.</BR></BR>

이번 멀티플레싱 서버는 <strong>select함수</strong>를 통하여 구현했습니다.</BR>
select함수는 한곳에 모여있는 파일 디스크립터들을 동시에 관찰할 수 있습니다. 따라서 select함수를 사용하기 위해서는 우선 파일 디스크립터를 관찰 항목에 따라 구분해서 모아야합니다.</BR>
여기서 관찰 항목은 수신, 전송, 예외 3가지 케이스로 해당 케이스 별로 파일 디스크립터를 모으기 위해 fd_set형 변수를 사용합니다.</BR></BR>

<strong>FD(File Descriptor)</strong>란?</BR>
파일 디스크립터란 socket함수를 사용하여 소켓을 생성하면 정수값이 반환되는데 이 값이 파일 디스크립터입니다.</BR>
윈도우에서 SOCKET형으로 반환되지만 소스코드를 살펴보면 unsigned __int64형인 것을 확인할 수 있습니다.</BR>
특히 파일 디스크립터는 같은 프로그램 내에서 <strong>중복되지 않기 때문에 고윳값으로 사용이 가능</strong>합니다.</BR></BR>

```
WSADATA wsaData;
SOCKET hServSock, hClntSock;
fd_set reads, cpyReads;

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
```

FD_ZERO(&reads)는 fd_set형 변수인 reads의 모든 비트를 0으로 초기화합니다.</BR>
FD_SET(hServSock, &reads)는 reads에 파일 디스크립터인 hServSock(서버 소켓)를 등록합니다.</BR>
따라서 hServSock은 select함수의 관찰대상이 되었으며 hServSock에 변화가 생기면 select함수는 반환을 합니다.</BR>
만약 hServSock의 파일 디스크립터가 1이라면, FD_SET()을 통해 fd1은 0에서 1로 set됩니다.</BR></BR>

```
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
```
fd_set형인 cpyRead에 reads의 내용을 복사하고 있는데, select함수를 거치면 변화가 없는 파일 디스크립터들의 비트가 0으로 초기화 되므로 1로 복구하기 위해서는 원본이 필요하기 때문에 복사합니다.</BR></BR>

timeout은 TIMEVAL형 변수로 select함수가 파일 디스크립터에 변화가 생기지 않을 경우 블로킹 상태에 머물게 되므로 해당 상황을 막기 위해 지정된 시간이 지나면 함수가 반환을 하도록 합니다.</BR>
만약 시간 초과로 반환된다면 select함수는 0을 반환하므로 fdNum이 0이라면 continue를 하여 다시 select함수로 돌아가게 합니다.</BR></BR>

select(0, &cpyReads, 0, 0, &timeout)를 통해 파일 디스크립터 중 몇 개가 read 이벤트가 발생했는지 반환합니다.</br>
첫 번째 인자는 Linux계열에서는 검사 대상이 되는 파일 디스크립터의 수이나 윈도우에서는 리눅스와의 호환성을 위해서만 존재합니다. 따라서 0으로 두고 무시합니다.</br></br>

select가 1이상을 반환했다면 for문을 통해 fd_set형인 reads의 set개수만큼 반복합니다.</br>
FD_ISSET()을 통해 상태변화가 있었던 파일 디스크립터를 찾습니다.</br>
if (reads.fd_array[i] == hServSock)를 통해 상태변화가 일어난 파일 디스크립터가 서버 소켓이면 accept함수를 통해 클라이언트 소켓을 생성해주고 <strong>fd_set형 변수 reads에 클라이언트 소켓의 파일 디스크립터를 등록</strong>합니다.</br></br>

만약 서버 소켓이 아니라면 코드 상 클라이언트 소켓밖에 없으므로 recv함수를 실행하고 수신한 데이터가 문자열 데이터인지 EOF인지 확인합니다.</BR>
수신한 데이터가 EOF라면 클라이언트 소켓을 종료하고 FD_CLR을 통해 reads에서 클라이언트 소켓의 파일 디스크립터의 정보를 삭제합니다.</br>
EOF가 아니라면 받은 내용을 다시 클라이언트에게 보내 에코 서비스를 제공합니다.</br></br>

### 시현 영상

https://github.com/rakkeshasa/SocketProgramming/assets/77041622/70300319-fdd5-4896-96a0-7b19530dea0e

</BR>

## 멀티캐스트 서버
멀티캐스트란?</BR>
송신측에서 A.B.C.D라는 멀티캐스트 IP로 데이터를 송신한다면 수신측이 A.B.C.D에 가입하여 송신측에서 보낸 데이터를 받겠다고 한다.</BR>
멀티캐스트 IP에 가입하면 A.B.C.D로 전송되는 데이터들이 IP에 가입된 수신측들이 전달되는 형태가 멀티캐스트이다.</BR></BR>

특정 IP에 가입하여 송신되는 데이터를 받는 것이기에 따로 서버(송신)와 클라(수신)간의 연결 절차가 없으므로 UDP방식으로 송수신한다.</BR>
일반적인 상황에서 데이터 수신측이 1000명이라면 송신측은 데이터를 총 1000번 보내줘야한다.</BR>
멀티캐스트를 사용할 시 송신측에서 데이터를 1번만 보내면 라우터에서 데이터가 담긴 패킷을 복사해서 멀티캐스트 IP에 가입된 수신측에게 전달한다.</BR>
따라서 송신측은 1000번이 아니라 1번만 보내도 되고, 트래픽 측면에서 이득을 취할 수 있다.</BR></BR>

<strong>1. 서버 구현</strong>
```
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
```
따로 연결과정이 없기 때문에 socket()에서 SOCK_DGRAM을 전달하여 UDP방식으로 소켓을 생성합니다.</BR>
inet_pton(AF_INET, "224.1.1.2", &mulAdr.sin_addr.s_addr)에서 데이터를 보낼 브로드캐스트 IP주소 "224.1.1.2"를 지정해줍니다.</BR>
setsockopt()함수는 데이터가 담긴 패킷을 얼마나 먼 라우터까지 전달할 수 있는지 TTL(Time To Live)를 세팅합니다.</br>
이후 news.txt에 있는 내용을 hSendSock을 통해 멀티캐스트 IP주소로 보냅니다.</BR></BR>

<strong>2. 클라이언트 구현</strong>
```
hRecvSock = socket(PF_INET, SOCK_DGRAM, 0);
memset(&adr, 0, sizeof(adr));
adr.sin_family = AF_INET;
adr.sin_addr.s_addr = htonl(INADDR_ANY);
adr.sin_port = htons(7777);

if (bind(hRecvSock, (SOCKADDR*)&adr, sizeof(adr)) == SOCKET_ERROR)
	ErrorHandling("bind() error");

inet_pton(AF_INET, "224.1.1.2", &joinAdr.imr_multiaddr.s_addr);
joinAdr.imr_interface.s_addr = htonl(INADDR_ANY);

if (setsockopt(hRecvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&joinAdr, sizeof(joinAdr)) == SOCKET_ERROR)
	ErrorHandling("setsock() error");

while (1)
{
	strLen = recvfrom(hRecvSock, buf, BUF_SIZE - 1, 0, NULL, 0);
	if (strLen < 0)
		break;

	buf[strLen] = 0;
	fputs(buf, stdout);
}
```
hRecvSock은 멀티캐스트 IP에 가입할 소켓이며, 주요하게 살펴볼 부분은 <strong>joinAdr</strong>을 세팅하는 코드부분입니다.</br>
```
inet_pton(AF_INET, "224.1.1.2", &joinAdr.imr_multiaddr.s_addr);
joinAdr.imr_interface.s_addr = htonl(INADDR_ANY);
```
<strong>joinAdr<strong>의 imr_multiaddr에는 가입할 멀티캐스트 IP주소를 넣고, imr_interface에는 자기 자신(호스트)의 IP주소를 담습니다.</BR>
이후 <strong>setsockopt()</strong>을 통해 hRecvSock을 대상으로 멀티캐스트 IP그룹에 가입시킵니다.</BR>




브로드캐스트란?</BR>
한번에 여러 호스트에게 데이터를 전송한다는 점에서 멀티캐스트와 유사합니다.</BR>
전송 범위에서 차이가 나는데 브로드캐스트는 동일한 네트워크로 연결되어 있는 호스트로, 데이터의 전송 대상이 제한됩니다.</BR></BR>


