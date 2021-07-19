/*====================================================
 *   Copyright (C) 2020  All rights reserved.
 *
 *   文件名称：cilen.c
 *   创 建 者：王东方  541306310@qq.com
 *   创建日期：2020年07月15日
 *   描    述：
 ================================================================*/
#include "client.h"

int main(int argc, char *argv[])
{	


	int fd[2];
	if (pipe (fd)<0)
	{
		perror ("pipe");
		exit (-1);
	}
	pid_t pid =fork ();
	if (pid <0)
	{
		perror ("fork");
		exit(-1);
	}
	if (pid ==0)
	{
		close (fd[0]);
		int num=getpid();
		write (fd[1],&num,4);
		execlp ("mpg123","mpg123","1.mp3",NULL);
	}

	if (pid >0)
	{	
		close (fd[1]);
		int id;
		read (fd[0],&id,4);

		sleep (2);
		system ("clear");


		SInfo sth;
		struct people stu;
		int sfd =socket (AF_INET,SOCK_STREAM,0);
		if (sfd <0)
		{
			perror ("socket");
			exit (-1);
		}
		int opt1 = 1;
		setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&opt1,sizeof(int));
		struct sockaddr_in ip;
		ip.sin_family =AF_INET;
		ip.sin_port = htons (10086);
//		ip.sin_addr.s_addr =inet_addr ("172.16.3.22");
//		ip.sin_addr.s_addr =inet_addr ("192.168.254.125");
		ip.sin_addr.s_addr =inet_addr ("0.0.0.0");
		socklen_t len =sizeof(ip);

		if (connect (sfd,(void*)&ip,len)<0)
		{
			perror ("connet");
			exit (-1);
		}
		int change=0,opt=0;
		ssize_t ret;
		int gg=0,sc=0;


		while (1)
		{
			prt();
			sc=scanf ("%d",&change);
			while (sc!=1)
			{
				puts ("请输入数字！！！");
				scanf ("%*[^\n]");
				sc=scanf ("%d",&change);
			}
			if (change ==1) //注册账号
			{
				my_fill (&stu,0);
				printf ("%s %s %s\n",stu.acount,stu.passwd,stu.tel);
				send (sfd,&change,sizeof(int),0);
				send (sfd,&stu,sizeof(stu),0);
				ret=recv (sfd,&gg,4,0);
				if (ret <0)
				{
					puts ("服务器异常");
					exit(-1);
				}
				if (gg ==1)
				{
					puts ("注册成功");
					continue;
				}
				else 
				{
					puts ("注册失败");
					continue;
				}

			}		
			if (change ==2) //登录账号
			{
				my_fill (&stu,3);
				send (sfd,&change,4,0);
				send (sfd,&stu,sizeof(stu),0);
				ret=recv (sfd,&gg,4,0);
				if (ret <0)
				{
					puts ("服务器异常");
					exit(-1);
				}
				if (gg ==1)
				{	
					puts ("登录成功");
					break;
				}
				else 
				{
					puts ("账号或者密码错误");
					continue;
				}
			}	

			if (change ==3) //修改密码
			{
				my_fill (&stu,1);
				send (sfd,&change,4,0);
				send (sfd,&stu,sizeof(stu),0);
				ret=recv (sfd,&gg,4,0);
				if (ret <0)
				{
					puts ("服务器异常");
					exit(-1);
				}	
				if (gg ==1)
				{
					puts("修改成功");
					continue;
				}
				else 
				{
					puts ("账号或者电话错误");
					continue;
				}

			}
			if (change ==4) //注销账号
			{
				my_fill (&stu,2);
				send (sfd,&change,4,0);
				send (sfd,&stu,sizeof(stu),0);
				ret=recv (sfd,&gg,4,0);
				if (ret <0)
				{
					puts ("服务器异常");
					exit(-1);
				}	
				if (gg==1)
				{
					puts ("注销成功");
					kill (id,SIGKILL);
					exit (-1);
				}
				else 
				{
					puts ("没有此账号");
					continue;
				}
			}

			if (change ==0)
			{
				kill (id,SIGKILL);
				exit (-1);
			}
			if (change ==5)
			{
				kill (id,SIGKILL);
				continue;
			}
			if (change ==6)
			{
				kill (id,SIGSTOP);
				continue;
			}
			if (change ==7)
			{
				kill (id,SIGCONT);
				continue;
			}
			puts ("请输入正确的数字选择");
		}
		while (1)
		{
			prt1 ();
			sc=scanf ("%d",&opt);
			while (sc!=1)
			{
				puts ("请输入数字！！！");
				scanf ("%*[^\n]");
				sc=scanf ("%d",&opt);
			}

			if (opt ==44)//退出
			{
				send (sfd,&opt,4,0);
				kill (id,SIGKILL);
				exit (0);
			}
			if (opt ==11) //查询货物
			{
				send (sfd,&opt,4,0);
				while (1)
				{
					ret=recv (sfd,&gg,sizeof(int),0);

					if (ret <0)
					{
						puts ("服务器异常");
						exit(-1);
					}	
					if (gg !=-1)
						break;

					ret=recv (sfd,&sth,sizeof(sth),0);
					if (gg ==-1)
						printf ("\t数量:%-5d\t价格:%-5d\t名称:%-10s\t介绍:%-10s\n",sth.Store_Num,sth.price,sth.Store_Name, sth.intr);
					if (ret <0)
					{
						puts ("服务器异常");
						exit(-1);
					}	
				}
				continue;
			}	
			if (opt ==22) //下单
			{	
				send (sfd,&opt,4,0);
				char name[100];
				bzero (name,100);
				puts ("请输入你要下单的名字");
				scanf ("%s",name);
				puts (name);
				send (sfd,name,sizeof(name),0);  //send
				ret=recv (sfd,&gg,4,0);
				if (ret <0)
				{
					puts ("服务器异常");
					exit(-1);
				}	
				if (gg==1)
				{
					puts ("下单成功");
					continue;
				}
				else 
				{
					puts ("没有此单号");
					continue;
				}
			}
			if (opt ==33) //退单
			{	
				send (sfd,&opt,4,0);
				char name[100];
				bzero (name,100);
				puts ("请输入你要退单的名字");
				scanf ("%s",name);
				send (sfd,name,sizeof(name),0);
				ret=recv (sfd,&gg,4,0);
				if (ret <0)
				{
					puts ("服务器异常");
					exit(-1);
				}	
				if (gg==1)
				{
					puts ("退单成功");
					continue;
				}
				else 
				{
					puts ("没有此单号");
					continue;
				}
			}
			if (opt ==55)
			{	
				send (sfd,&opt,4,0);
				char buf11[2048];
				bzero(buf11, 2048);
				ret=recv (sfd,buf11,sizeof(buf11),0);
				if (ret <0)
				{
					puts ("没有收到通知");
					break;
				}
				write (1,buf11,strlen(buf11));
				printf("\n");
				continue;
			}

			puts ("请输入正确的数字选择");		
		}
		close (sfd);
		wait (NULL);
	}
	return 0;
}
