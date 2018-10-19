#include<stdio.h>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")


//ÕìÌý¶Ë¿Ú
//#define SERVER_PORT 6666

#define SERVER_TIME        1
#define SERVER_NAME        2
#define SERVER_CLIENTLIST  3
#define SERVER_FORWARD     4
#define SERVER_DISCONN     5

#define MAXCONTENT         512


#define USER_OP_CONNECT       'c'
#define USER_OP_BREAKCONN     'b'
#define USER_OP_TIME		  't'
#define USER_OP_NAME          'n'
#define USER_OP_LIST          'l'
#define USER_OP_SEND          's'
#define USER_OP_QUIT          'q'
#define USER_OP_HELP          'h'

struct RequestPkt
{
	char type[5];              //message type
	char number[3];            //the order of the client will receive the message
	char content[MAXCONTENT];  //contents the target client will receive
};

struct MsgPkt
{
	char msg;                  //message type
	char order[3];             //order of the client who send the message
	char IP[20];               //Attention: if it is a reponse package, IP means type, port means number and content means content
	char port[5];
	char content[MAXCONTENT];
};


/*
 * WinSock Initialization Function
 */
void Initialize_WinSocket();

SOCKET Socket_Systemcall();

void GetHelp();

DWORD WINAPI User_Pipeline(LPVOID lpParameter);

void CreateSubThread_User(  );

DWORD WINAPI Receive_Pipeline(LPVOID lpParameter);

void CreateSubThread_Receiving(  );

void Connect2Server(  );

void getTime(  );

void getName(  );

void getClientList(  );

void forward(  );

void QuitProgram (  );

void DisconnetfromServer(  );

void print();

void PrintDash();

void PktRequest(struct RequestPkt* t, char* s);

void PktMsg(struct MsgPkt* t, char* s);

void UnPackageReq(char* s, struct RequestPkt *t);

void UnPackageMsg(char* s, struct MsgPkt* t);

void Cut(char* s, char* target, int order);