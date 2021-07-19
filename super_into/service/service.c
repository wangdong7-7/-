/**************************************************
 *服务器：
 *
 * 1、管理员账号
 *	2、管理仓库
 *	3、群发通知
 *	4、管理下单退单
 *
 **************************************************/ 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "super_into.h"
#include "StoreManage.h" 
#include <arpa/inet.h>


#define Listen_Len 100

typedef struct Pthread
{
	peo ppp;
	int newfd;
	int flag;
	int choice;
	int s, q;
	char buf[1024];
	sqlite3 *db;
	Node *head;
}PTH;

int Usr_Info(int *choice, int sfd, sqlite3 *db, peo *p)
{
	int flag;
	int True = 1;
	int Error = 0;
	switch(*choice)
	{
		case 1:
			//注册
			if(0 == sq_into(db, p))
			{
				printf("%lu:注册成功\n", pthread_self());
				send(sfd, &True, sizeof(True), 0);
			}
			else
			{
				printf("%lu:注册失败\n", pthread_self());
				send(sfd, &Error, sizeof(Error), 0);
			}

			flag = 0;
			break;
		case 2:
			//登录
			printf("%lu:请求登录\n", pthread_self());
			if (sq_login (db, p) == 1)
			{
				send(sfd, &True, sizeof(True), 0);
				flag = 1;
			}
			else
			{
				printf("%lu:登录失败\n", pthread_self());
				send(sfd, &Error, sizeof(Error), 0);
				flag = 0;
			}
			break;
		case 3:
			//修改
			printf("%lu:请求修改\n", pthread_self());
			if (sq_change (db, p) == 1)
			{
				send(sfd, &True, sizeof(True), 0);
				perror("send");
			}
			else 
			{
				printf("%lu:修改失败\n", pthread_self());
				send(sfd, &Error, sizeof(Error), 0);
			}

			flag = 0;
			break;
		case 4:
			//注销
			printf("%lu:请求注销\n", pthread_self());
			if (sq_del(db, p) == 0)
			{
				if (send(sfd, &True, sizeof(True), 0) < 0)
				{
					perror("send");
				}
				printf("%lu:注销成功\n", pthread_self());
				flag = -1;
			}
			else
			{
				send(sfd, &Error, sizeof(Error), 0);
				printf("%lu:注销失败\n", pthread_self());
				flag = 0;
			}
			break;
defult:
			flag = 0;
			break;
	}

	return flag;
}

//群发通知
void Mass_Notification(PTH *p)
{
	char buf[2048];
	int Port_Number;

	FILE *fp = fopen("通知.txt", "r");

	while(1)
	{
		fread(buf, 4, sizeof(buf), fp);
		send(p->newfd, buf, strlen(buf), 0);
		printf("strlen(buf):%ld\n", strlen(buf));
		if (feof(fp) != 0)
			break;
	}
	fclose(fp);
	return;
}

int Store_Manage(int choice, int sfd, PTH *p)
{
	int flag, flag1;

	switch(choice)
	{
		case 11:
			//管理仓库
			printf("%lu:请求查看货物信息\n", pthread_self());
			if(SendInfo(p->head, sfd))//发送信息
			{
				printf("%lu:发送成功\n", pthread_self());
			}
			flag1 = 0;
			break;
		case 22:
			//下单
			flag = 1;
			printf("%lu:正在进行下单操作\n", pthread_self());
			bzero(p->buf, sizeof(p->buf));
			recv(p->newfd, p->buf, sizeof(p->buf), 0);
			int re = Order(p->head, p->buf, flag);
			WriteFile(p->head);
			send(p->newfd, &re, sizeof(re), 0);
			flag1 = 0;
			if (re == 1)
				printf("%lu:下单完成\n", pthread_self());
			else
				printf("%lu:下单失败\n", pthread_self());
			break;
		case 33:
			//管理订单
			//退单
			flag = 0;
			printf("%lu:正在进行退单操作\n", pthread_self());
			bzero(p->buf, sizeof(p->buf));
			recv(p->newfd, p->buf, sizeof(p->buf), 0);
			int r = Order(p->head, p->buf, flag);
			WriteFile(p->head);
			send(p->newfd, &r, sizeof(r), 0);
			if (r == 1)
				printf("%lu:退单完成\n", pthread_self());
			else
				printf("%lu:退单失败\n", pthread_self());
			flag1 = 0;
			break;
		case 44:
			flag1 = 1;
			break;
		case 55:
			//群发通知
			Mass_Notification(p);
			flag1 = 0;
			break;
	}

	return flag1;
}

void *func (void *arg)
{
	PTH *p;
	p = (PTH *)malloc(sizeof(PTH));
	p = (PTH *)arg;
	int flag = 0, flag1 = 0;
	int q;

	while (1)
	{
		if ((recv(p->newfd, &(p->s), sizeof(p->s), 0) <= 0) || (flag == -1))//接收选项：登录、注册etc、、、
		{
			printf("客户%d退出连接\n", p->newfd);
			printf ("线程:%lu结束\n",pthread_self());
			pthread_exit(NULL);
			return 0;
		}
		recv(p->newfd, &p->ppp, sizeof(p->ppp), 0);//接收结构体
		p->flag = Usr_Info(&p->s, p->newfd, p->db, &p->ppp);
		if(p->flag == 1)
		{
			while(1)
			{
				bzero(&q, 4);
				recv(p->newfd, &q, sizeof(p->q), 0);
				flag1 = Store_Manage(q, p->newfd, p);
				if (flag1 == 1)
				{
					flag = -1;
					break;
				}
			}
		}	
	}
}

int main()
{
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in saddr, c_ip;
	socklen_t len = sizeof(c_ip);
	PTH c_info[10];
	pthread_t tid[10];
	int newfd[10]={0};
	Node *head[10];
	for (int i=0; i<10; i++)
		head[i] = Init();

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(10086);
	saddr.sin_addr.s_addr = inet_addr("0.0.0.0");//0.0.0.0 默认本机ip地址

	if (bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr)))
	{
		perror("bind");
		return 0;
	}

	if (listen(sfd, Listen_Len))
	{
		perror("listen");
		return 0;
	}

	sqlite3 *db;
	sqlite3_open ("./all.db",&db);
	//sq_reg (db);
	for (int i=0;i<10;i++)
	{
		c_info[i].db = db;
		c_info[i].head = head[i];
		ScanfLink(c_info[i].head);
		newfd[i]=accept (sfd,(void*)&c_ip,&len);
		if (newfd[i] <0)
		{
			perror ("accept");
			exit (1);
		}
		printf ("客户端%d连接\n", newfd[i]);
		c_info[i].newfd = newfd[i];
		pthread_create (&tid[i],NULL,func,&c_info[i]);
	//	socket();
		pthread_detach(tid[i]);
	}
	for (int i=0;i<10;i++)
		pthread_join (tid[i],NULL);

	close (sfd);
	for (int i=0;i<10;i++)
		close (newfd[i]);

	return 0;
}	
