/*====================================================
*   Copyright (C) 2020  All rights reserved.
*
*   文件名称：super_into.c
*   创 建 者：王东方  541306310@qq.com
*   创建日期：2020年07月20日
*   描    述：
================================================================*/
#include "super_into.h"

//创建表和表的结构，调用一次就够
int sq_reg (sqlite3 *db)
{
	if (db ==NULL)
	{
		fprintf (stderr,"NULL\n");
	//	exit (-1);
	}

	char *err=NULL;
	char buf[100]={};
	sprintf (buf,"create table mess (acount char(50) primary key,passwd char(50) not null,tel char(50));");
	if (sqlite3_exec (db,buf,NULL,NULL,&err)!=0)
		printf ("exec err %s\n",err);

	return 0;
}
//创建新用户 
int sq_into (sqlite3 *db,peo *p)
{
	if (db ==NULL)
	{
		fprintf (stderr,"NULL\n");
	//	exit (-1);
	}
	char *err=NULL;
	char buf[100]={};
	sprintf (buf,"insert into mess values ('%s','%s','%s');",p->acount,p->passwd,p->tel);
	if (sqlite3_exec (db,buf,NULL,NULL,&err)!=0)
		printf ("exec err %s",err);

	return 0;
}


//全部查看 (查看表的所有内容)
int sq_show (sqlite3 *db)
{
	if (db ==NULL)
	{
		fprintf (stderr,"NULL\n");
	//	exit (-1);
	}
	char **daty=NULL;
	char buf[100];
	int a=0,b=0;
	sprintf (buf,"select * from mess");
	a=0,b=0;
	if (sqlite3_get_table (db,buf,&daty,&a,&b,NULL)<0)
		puts ("get_table error");
	for (int i=0;i<=a;i++)
	{	
		for (int j=0;j<b;j++)
		{
			printf ("%-10s ",daty[i*b+j]);
		}
		puts("");
	}
}
//修改（按照账号和电话）
int sq_change (sqlite3 *db,peo *p)
{
	if (db ==NULL)
	{
		fprintf (stderr,"NULL\n");
		exit (-1);
	}

	char acount[100],passwd[100],tel[100];

	char *err=NULL;
	char buf[100]={};
	sprintf (buf,"update mess set passwd ='%s' where acount='%s' and tel ='%s';",p->passwd,p->acount,p->tel);
	if (sqlite3_exec (db,buf,NULL,NULL,&err)!=0)
		printf ("exec err %s\n",err);
	sprintf (buf,"select * from mess where acount='%s' and tel ='%s';",p->acount,p->tel);
	int a=0,b=0;
	char **err1=NULL;
	if (sqlite3_get_table (db,buf,&err1,&a,&b,NULL)<0)
		puts ("get_table error");
	if (a>= 1)
	{
		puts ("修改成功");
		return 1;
	}
	else 
	{
		printf  ("账号或者电话错误\n");
		return -1;
	}
}

//删除 (按照账号)
int sq_del (sqlite3 *db,peo *p)
{
	if (db ==NULL)
	{
		fprintf (stderr,"NULL\n");
		exit (-1);
	}		
	char *err=NULL;
	char buf[100]={};	

	sprintf (buf,"select * from mess where acount='%s';",p->acount);
	int a=0,b=0;
	char **err1=NULL;
	if (sqlite3_get_table (db,buf,&err1,&a,&b,NULL)<0)
		puts ("get_table error");
	if (a>= 1)
	{
		puts ("删除成功");
	}
	else 
	{
		printf  ("没有此账号\n");
		return -1;
	}

	sprintf (buf,"delete from  mess where acount ='%s';",p->acount);
	if (sqlite3_exec (db,buf,NULL,NULL,&err)!=0)
		printf ("exec err %s",err);
	return 0;
}
//登录
int sq_login (sqlite3 *db,peo *p)
{	
	if (db ==NULL)
	{
		fprintf (stderr,"NULL\n");
		exit (-1);
	}
	char **daty=NULL;
	char buf[100];
	int a=0,b=0;	
	
	sprintf (buf,"select * from mess where acount='%s' and passwd='%s';",p->acount,p->passwd);
	if (sqlite3_get_table (db,buf,&daty,&a,&b,NULL)<0)
		puts ("get_table error");
	if (a>=1)
	{
		puts ("登录成功");
		return 1;
	}
	else 
	{
		puts ("没有此账号");
		return -1;
	}
}






