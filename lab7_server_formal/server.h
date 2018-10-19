#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT  4390
#define MAX          105

#define MAXCONTENT   512

struct RequestPkt
{
	char type[5];              //message type
	char number[3];            //the order of the client will receive the message
	char content[MAXCONTENT];  //contents the target client will receive
};

struct MsgPkt
{
	char msg;                  //message type
	char _order[3];             //order of the client who send the message
	char IP[20];               //Attention: if it is a reponse package, IP means type, port means number and content means content
	char port[10];
	char content[MAXCONTENT];
};

/* Global Variables starts here... */
int order = 0;   //Index for the two below£¬ actually starting from 1 !!!
				 //also represent the total number of connected clients
SOCKET client[MAX] = { 0 };
struct sockaddr_in clientAddr[MAX];

char sendBuf[128] = { 0 };  //for function inet_ntop()
/* Global Variables ends... */
