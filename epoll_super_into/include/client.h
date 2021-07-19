/*====================================================
*   Copyright (C) 2020  All rights reserved.
*
*   文件名称：client.h
*   创 建 者：王东方  541306310@qq.com
*   创建日期：2020年07月20日
*   描    述：
================================================================*/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <wait.h>
#define NUM 128

struct people
{
	char acount[100];
	char passwd[100];
	char tel [100];
};

typedef struct Store_Info
{
	char Store_Name [52];
	int Store_Num;
	int Number;
	int price;
	char intr[100];
}SInfo;

void my_fill (struct people *stu,int flag);
void prt(void);
void prt1(void);
#endif 
