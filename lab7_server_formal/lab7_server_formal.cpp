// Server1.0.cpp: 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <time.h>
#include "server.h"


//Function to package request
//Format like this: "|type|number|content|"
void PktRequest(struct RequestPkt* t, char* s)
{
	strcpy(s, "\0");           //clean s
	strcpy(s, "|");            //"|" means a split character
	strcat(s, t->type);        //request type 
	if (strcmp(t->type, "msg") == 0)       //if type is message
	{
		strcat(s, "|");
		strcat(s, t->number);
		strcat(s, "|");
		strcat(s, t->content);
	} 
	strcat(s, "|");            //Finally request package format like this: "|type|number|content|"
} 

//Function to package message
//Format like this: "|msg|order|IP|port|content|"
//If order and port is empty, format like this: "|msg||IP||content|"
void PktMsg(struct MsgPkt* t, char* s)
{
	strcpy(s, "\0");           //clean s
	strcpy(s, "|");            
	s[1] = t->msg;             //message type
	s[2] = '\0';
	strcat(s, "|");
	if (t->msg == '1' || t->msg == '3')       //message type 1 or 3
	{
		strcat(s, t->_order);
		strcat(s, "|");
		strcat(s, t->IP);
		strcat(s, "|");
		strcat(s, t->port);
		strcat(s, "|");
		strcat(s, t->content);
		strcat(s, "|");
	}                          //Finally message package format like this: "|1|order|IP|port|content|"
	else
	{
		strcat(s, "|");
		strcat(s, t->IP);      //If you can't understand why here use IP to figure out type, read notes in struct MsgPkt
		strcat(s, "|");
		if (strcmp(t->IP, "msg") == 0)
		{
			strcat(s, t->port);
			strcat(s, "|");
			strcat(s, "|");
		}                      //Finally message package format like this: "|2||IP|port||"
		if (strcmp(t->IP, "list") == 0)
		{
			strcat(s, t->port);
			strcat(s, "|");
			strcat(s, t->content);
			strcat(s, "|");
		}                      //Finally message package format like this: "|2||IP|port|content|"
		if (strcmp(t->IP, "name") == 0 || strcmp(t->IP, "time") == 0)
		{
			strcat(s, "|");
			strcat(s, t->content);
			strcat(s, "|");
		}                      //Finally message package format like this: "|2||IP||content|"
	}                          
}

//This function used to read message or request package by split character
//Using method: s to save the message we read from target, following by order
//i means after we meet i split characters, we begin to read the message we want
//j, k used to copy valuable message
void cut(char* s, char* target, int _order)
{
	int i = 0;
	int j = 0;
	int k = 0;
	while (i != _order)
	{
		if (target[j++] == '|')
		{
			i++;
		}
	}
	while (target[j] != '|')
	{
		s[k++] = target[j++];
	}
	s[k] = '\0';
}

//For the reason the request package format like this: "|type|number|content|"
//after one split character we read type
//after two split characters we read number
//after three split characters we read content
void unPackageReq(char* s, struct RequestPkt* t)
{
	cut(t->type, s, 1);
	if (s[1] == 'm');
	{
		cut(t->number, s, 2);
		cut(t->content, s, 3);
	}
}

//For the reason the request package format like this: "|msg|order|IP|port|content|"
//msg in s[1]
//after two split characters we read order
//after three split characters we read IP
//after four split characters we read port
//after five split characters we read content
void unPackageMsg(char* s, struct MsgPkt* t)
{
	t->msg = s[1];
	cut(t->_order, s, 2);
	cut(t->IP, s, 3);
	cut(t->port, s, 4);	
	cut(t->content, s, 5);
	return;
}

//Function to get localtime
//0 means msg type
//Finally format like this: "|0||time||year/month/day hour:minute:second|"
void getTime(char* s)
{
	time_t t;
	struct tm* lt;
	time(&t);  //get time stamp
	lt = localtime(&t);  
	char temp[256] = { 0 };

	strcpy(s, "\0");                           //clean s
	strcat(s, "|0||time||");
	sprintf(temp, "%d", lt->tm_year + 1900);
	strcat(s, temp);
	strcat(s, "/");
	sprintf(temp, "%d", lt->tm_mon + 1);
	strcat(s, temp);
	strcat(s, "/");
	sprintf(temp, "%d", lt->tm_mday);
	strcat(s, temp);
	strcat(s, " ");

	sprintf(temp, "%d", lt->tm_hour);
	strcat(s, temp);
	strcat(s, ":");
	sprintf(temp, "%d", lt->tm_min);
	strcat(s, temp);
	strcat(s, ":");
	sprintf(temp, "%d", lt->tm_sec);
	strcat(s, temp);
	strcat(s, "|\0");
}

//Function to get host name
//0 means msg type
//Finally format like this: "|0||name||hostname|"
void getHostName(char* s)
{
	char temp[255];
	strcpy(s, "\0");
	strcat(s, "|0||name||");
	gethostname(temp, 255);
	strcat(s, temp);
	strcat(s, "|");
}

//Function to get client list
//0 means msg type
//Finally format like this: "|0||list|total clients|client 1's order*client 1's address*client 1's port\nclient 2's ...|"
void getClientList(char* s)
{
	//TODO
	strcpy(s, "\0");
	strcpy(s, "|0||list|");

	char temp[128] = { 0 };
	sprintf(temp, "%d", order);
	strcat(s, temp);
	strcat(s, "|");
	for (int i = 1; i < order; i++)
	{
		sprintf(temp, "%d", i);
		strcat(s, temp);
		strcat(s, "*");
		strcat(s, inet_ntop(AF_INET,(void*)&clientAddr[i].sin_addr, sendBuf, 16));
		strcat(s, "*");
		sprintf(temp, "%d", htons(clientAddr[i].sin_port));
		strcat(s, temp);
		strcat(s, "\n");
	}
	strcat(s, "|");
}

//
// used to write buffer about forward msg
// send()
//
void forwardMessage(char *buffer, SOCKET srcSocket) {
	struct RequestPkt* temp = (struct RequestPkt*)malloc(sizeof(struct RequestPkt));
	char sendAnotherClient[256] = { 0 };

	unPackageReq(buffer, temp);
	int _index = atoi(temp->number);
	SOCKET targetSocket = client[_index];

	struct MsgPkt* msgPackage = (struct MsgPkt*)malloc(sizeof(struct MsgPkt));
	msgPackage->msg = '3';

	int _idx_src = 0;
	for (int i = 1; i <= order; i++)
	{
		if(client[i] == srcSocket)
		{
			_idx_src = i;
		}
	}
	
	sprintf(msgPackage->_order, "%d", _idx_src);
	sprintf(msgPackage->port,  "%d", htons(clientAddr[_idx_src].sin_port));
	strcpy(msgPackage->IP, inet_ntop(AF_INET, (void*)&clientAddr[_idx_src].sin_addr, sendBuf, 16));

	strcpy(msgPackage->content, "Received from ");
	strcat(msgPackage->content, msgPackage->IP);
	strcat(msgPackage->content, ":");
	strcat(msgPackage->content, msgPackage->port);
	strcat(msgPackage->content, "\n");
	strcat(msgPackage->content, temp->content);
	
	PktMsg(msgPackage, sendAnotherClient);
	//printf("%s", sendAnotherClient);
	
	//FORWARD
	int ret = send(targetSocket, sendAnotherClient, strlen(sendAnotherClient), 0);
			
	//format like this: "|0||msg||N|" (for failed)  "|0||msg||Y|"  (for success)
	strcpy(buffer, "\0");   //clean buffer
	strcat(buffer, "|0||msg|");
	if ( ret <= 0)
	{
		printf("send() to target failure!\n");
		strcat(buffer, "|N|");
	}
	else
	{
		strcat(buffer, "|Y|");
	}

}

//
//switch()
//
void processRequestEachThread(char *buffer, int *_flag, SOCKET srcSocket) {
	if (buffer[1] == 't')            //time
	{
		getTime(buffer);
	}
	else if (buffer[1] == 'n')       //name
	{
		getHostName(buffer);
	}
	else if (buffer[1] == 'l')       //list
	{
		getClientList(buffer);
	}
	else if (buffer[1] == 'q')       //disconnect
	{
		strcpy(buffer, "|0||quit|||"); //TODO
		*_flag = 0;
	}
	else if (buffer[1] == 'm')       //message
	{
		forwardMessage(buffer, srcSocket);
	}
}


//
//将准备退出的客户端删除，并将后面所有的内容往前移动一格
//
void reArrangeStack(SOCKET clientSocket){
	int _index = 0;
	for(int i = 1; i <= order; i++)
	{
		if(client[i] == clientSocket) //use client socket to identify the client
		{
			_index = i;
			break;
		}
	}
	for(int i = _index; i < order; i++)
	{
		client[i] = client[i + 1];
		clientAddr[i] = clientAddr[i + 1];
	}

	order--;
}


/*
 * SubThread lifecycle for each accepted client
 * recv() —— send() structure
 */
DWORD WINAPI ProcessClientRequests(LPVOID lpParam) {
	printf("SubThread ID:%4d create!\n", GetCurrentThreadId());

	SOCKET clientSocket = (SOCKET)(LPVOID)lpParam;
	
	char buffer[256] = { 0 };    //response string
	int ret;
	
	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		ret = recv(clientSocket, buffer, sizeof(buffer), 0);
		//printf("%s\n", buffer);
		if (ret <= 0)
		{
			printf("receive error!\n");
			break;
		}

		int _flag = 1;
		processRequestEachThread(buffer, &_flag, clientSocket);

		printf("The %d-th subthread, socket::%d prepare response IS %s\n", GetCurrentThreadId(), clientSocket, buffer);
		//response for each Requests from corresponding client...
		ret = send(clientSocket, buffer, strlen(buffer), 0);
		if (ret <= 0)
        {
			printf("send() response to client error!\n");
            break;
        }

		//if disconnect request, BREAK the thread
		if(_flag == 0)
		{
			break;
		}
	}
	
	printf("Thread ID:%d stop!\n", GetCurrentThreadId());
	//TODO  re-arrange the stack
	reArrangeStack(clientSocket);
	closesocket(clientSocket);

	return 1;
}


void Initialize_WinSocket() {

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);  //希望使用的WinSock DLL的版本
	
	if ( WSAStartup( wVersionRequested, &wsaData ) != 0 )
	{
		printf("WSAStartup() failed!\n");
		return;
	}
	
	//确认WinSock DLL支持版本2.2：
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup();
		printf("Invalid Winsock version!\n");
		return;
	}
}

SOCKET Socket_Systemcall() {
	//socket system call
	SOCKET Server_SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(Server_SOCKET == INVALID_SOCKET)
	{
		WSACleanup();
		printf("socket() failed!\n");
		return 0; //TODO
	}
	printf("SOCKET: %lu\n", Server_SOCKET);

	//Create User_Pipeline's eventMsg ( , mannul, reset , init , )
	//hEvent_User = CreateEvent(NULL, TRUE, FALSE, NULL);

	return Server_SOCKET;
}

void BindnListen(SOCKET Server_SOCKET) {
	int len = sizeof(SOCKADDR);
    SOCKADDR_IN serviceAddr;
    serviceAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serviceAddr.sin_family = AF_INET;
    serviceAddr.sin_port = htons(SERVER_PORT);
    
    if (bind(Server_SOCKET, (SOCKADDR*)&serviceAddr, len) == INVALID_SOCKET)
    {
        printf("failed bind!\n");
        closesocket(Server_SOCKET);
        WSACleanup();
        return ;
    }
    if (listen(Server_SOCKET, MAX) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(Server_SOCKET);
        WSACleanup();
        return ;
    }
}


int main()
{
	Initialize_WinSocket();

	SOCKET serverSocket = Socket_Systemcall();

	BindnListen(serverSocket);


	SOCKET clientSocket;
	HANDLE threads[MAX];
	int addr_length = sizeof(SOCKADDR);

	while ( true )
	{
		printf("Listening on port %d\n-------------------------\n", SERVER_PORT);
		order++;
		clientSocket = accept(serverSocket, (SOCKADDR*)(&clientAddr[order]), &addr_length);
		printf("order: %d\n", order);

		if (clientSocket == INVALID_SOCKET)
		{
			printf("accept error!");
			break;
		}

		if (order < MAX)
		{
			client[order] = clientSocket;
			HANDLE hThread_t = CreateThread(NULL, NULL, ProcessClientRequests, (LPVOID)clientSocket, 0, NULL);
			if(hThread_t == NULL)
			{
				printf("CreatThread AnswerThread() failed.\n");
				continue;
			}
			threads[order] = hThread_t;
			Sleep(1000);

			//tips of the incoming client...
			printf("Order of the accepted client:  %d\n", order);
			printf("IP of the accepted client:     %s\n", inet_ntop(AF_INET, (void*)&clientAddr[order].sin_addr, sendBuf, 16));
			printf("Port of the accepted client:   %d\n", htons(clientAddr[order].sin_port));
			printf("The applied SOCKET of the accepted client: %d\n", client[order]);
		}
		else
		{
			printf("Exceeding the max amount of queue...");
			closesocket(clientSocket);
		}
	}

	closesocket(serverSocket);
	WSACleanup();

    return 0;
}
