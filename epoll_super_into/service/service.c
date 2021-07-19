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
#include <sys/epoll.h>
#include <fcntl.h>

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
	PTH c_info[1000];
	Node *head[1000];
	for (int i=0; i<1000; i++)
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
	int epfd=epoll_create (1000);
	if (epfd <0)
	{
		perror ("epoll create");
		exit (-1);
	}

	struct epoll_event tep,ep[1000];
	int client[1000],ret,n,socked;
	char buf[100];
	int max=-1,connfd,j,cfd;
	char str[100];

	//将接收到的客户端初始化为-1
	for (int i=0;i<1000;i++)
	{
		client[i]=-1;
	}

	//将监听的文件描述符上树
	tep.events=EPOLLIN;
	tep.data.fd=sfd;
	int res=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&tep);
	if (res==-1)
	{
		perror ("epoll_ctl");
		exit (-1);
	}

	//循环监听
	while (1)
	{
		res=epoll_wait(epfd,ep,1000,-1);
		if (res==-1)
		{
			perror ("wait");
			exit(-1);
		}

			printf ("--res =%d ---------",res);
		for (int i=0;i<res;i++)
		{
			printf ("---ep =%d sfd=%d\n---",ep[i].data.fd,sfd);
			cfd=ep[i].data.fd;
			if (!(ep[i].events & EPOLLIN))
				continue;
			if (ep[i].data.fd==sfd)
			{
				c_info[i].db = db;
				c_info[i].head = head[i];
				ScanfLink(c_info[i].head);

				//接收文件描述符
				connfd=accept(sfd,(void*)&c_ip,&len);
				c_info[i].newfd = connfd;
				//设置非阻塞I/O，保证后面可以读完缓存区的信息流
				int flag=fcntl(connfd,F_GETFL);
				flag |=O_NONBLOCK;
				fcntl(connfd,F_SETFL,flag);
				printf("received from %s at PORT %d\n", 
						inet_ntop(AF_INET, &c_ip.sin_addr, str, sizeof(str)), 
						ntohs(c_ip.sin_port));

				for (j=0;j<1000;j++)
				{
					if (client[j]<0)
					{
						client[j]=connfd;
						break;
					}
				}
				if (connfd ==1000)
				{
					puts ("too many client");
					exit(-1);
				}
				if (j >max)
					max=j;
				tep.events=EPOLLIN;
				tep.data.fd= connfd;
				ret=epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&tep);
				if (ret==-1)
				{
					perror ("ctl add");
					exit(-1);
				}
			}
			else
			{
				func(&c_info[i]);
			}
		}

	}
	close (sfd);
	close (epfd);
	return 0;
}	
