#ifndef __STOREMANAGE_H__
#define __STOREMANAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct Store_Info
{
	char Store_Name[52];
	int Store_Num;
	int Number;
	int price;
	char Introduce[100];
}SInfo;

typedef struct LinkList
{
	SInfo St_Info;
	struct LinkList *pre;
	struct LinkList *next;
}Node;

Node *Init();
void InsertInfo(Node *head);
void ScanfLink(Node *head);
void WriteFile(Node *head);
int SendInfo(Node *head, int sockfd);
int Order(Node *head, char *buf, int flag);

#endif
