#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <windows.h>
#include <string.h>
#include <stdlib.h>

//build a function of recvthread

int theirsocket,theirlen=sizeof(SOCKADDR_IN),mysocket;
struct sockaddr_in theiraddr;

DWORD WINAPI Threadrecv(LPVOID comein){
	int ret;
	char word[255];
	while(1){
		memset(word,1,sizeof(word));
		ret=recv(theirsocket,word,strlen(word),0);
		if(ret==-1){
			if(WSAGetLastError()==WSAECONNRESET){
				printf("good bye friend,I've left");
				closesocket(theirsocket);
				break;
			}
			printf("recv error\n");
			continue;
		}
		else if(ret){
			word[ret]='\0';
			puts(word);
		}
	}
	return 0;
}

//build a function of sendthread
DWORD WINAPI Threadsend(LPVOID goout){
	char word[255];
	while(1){
		memset(word,1,sizeof(word));
		gets(word);
		if(send(theirsocket,word,strlen(word),0)==-1){
			printf("send error\n");
			continue;
		}
	}
}

DWORD WINAPI Threadaccept(LPVOID hear){
//accept
	while(1){
		theirsocket=accept(mysocket,(SOCKADDR*)&theiraddr,&theirlen);
		if(theirsocket==-1){
			printf("accept error\n");
			continue;
		}
//set up a sendthread and a recvthread
		HANDLE athread=CreateThread(NULL,0,Threadsend,NULL,0,NULL);
		if(athread==NULL){
			printf("create sendthread error\n");
			return 0;
		}
		HANDLE bthread=CreateThread(NULL,0,Threadrecv,NULL,0,NULL);
		if(bthread==NULL){
			printf("create recvthread error\n");
			return 0;
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
	mysocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(mysocket==-1){
		printf("socket error\n");
		return 0;
	}
	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=htons(4399);
	sin.sin_addr.S_un.S_addr=INADDR_ANY;
//bind
	if(bind(mysocket,(LPSOCKADDR)&sin,sizeof(sin))==-1){
		printf("bind error\n");
		return 0;
	}
//listen
	if(listen(mysocket,5)==-1){
		printf("listen error\n");
		closesocket(mysocket);
		return 0;
	}
//set up a thread
	HANDLE athread=CreateThread(NULL,0,Threadaccept,NULL,0,NULL);
	if(athread==NULL){
		printf("create mainthread error\n");
		return 0;
	}else	while(1);
//closesocket
	closesocket(mysocket);
	closesocket(theirsocket);
//wsacleanup
	WSACleanup();
	return 0;
}