#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#define IP "10.0.1.65"
int mysocket;

//build a function of recvthread
DWORD WINAPI Threadrecv(LPVOID comein){
	char word[255];
	while(1){
		memset(word,1,sizeof(word));
		int ret=recv(mysocket,word,strlen(word),0);
		if(ret==-1){
			if(WSAGetLastError()==WSAECONNRESET){
				closesocket(mysocket);
				return 0;
			}
			printf("recv error\n");
			continue;
		}
		else{
			word[ret]='\0';
			printf("%s\n",word);
		}
	}
}

//build a function of sendthread
DWORD WINAPI Threadsend(LPVOID goout){
	char word[255];
	while(1){
		memset(word,1,sizeof(word));
		gets(word);
			if(send(mysocket,word,strlen(word),0)==-1){
				printf("send error\n");
				continue;
			}
	}
}

int main(){
//wsastartup
	WSADATA wsaData;
	WORD sockVersion=MAKEWORD(2,2);
	if(WSAStartup(sockVersion,&wsaData)){
		printf("wsastartup error\n");
		return 0;
	}
//set up socket
	mysocket=socket(AF_INET,SOCK_STREAM,0);
	if(mysocket==-1){
		printf("socket error\n");
		return 0;
	}
	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=htons(4399);
	sin.sin_addr.S_un.S_addr=inet_addr(IP);
//connect
	if(connect(mysocket,(SOCKADDR*)&sin,sizeof(SOCKADDR))==-1){
		printf("connect error\n");
		return 0;
	}
//set up a thread for send and a thread for recv	
	HANDLE athread=CreateThread(NULL,0,Threadsend,NULL,0,NULL);
	if(athread==NULL){
		printf("create sendthread error\n");
		return 0;
	}	
	HANDLE bthread=CreateThread(NULL,0,Threadrecv,NULL,0,NULL);
	if(bthread==NULL){
		printf("create recvthread error\n");
		return 0;
	}else	while(1);
//closesocket
	closesocket(mysocket);
//wsacleanup
	WSACleanup();
	return 0;
}