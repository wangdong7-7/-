/*====================================================
*   Copyright (C) 2020  All rights reserved.
*
*   文件名称：super_into.h
*   创 建 者：王东方  541306310@qq.com
*   创建日期：2020年07月20日
*   描    述：
================================================================*/

#ifndef _SUPER_INTO_H_
#define _SUPER_INTO_H_

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct _people_
{
	char acount[100];
	char passwd[100];
	char tel [100];
}peo;


//创建表和表的结构，调用一次就够
int sq_reg (sqlite3 *db);
//登录
int sq_login (sqlite3 *db,peo *p);
//全部查看 (查看表的所有内容)
int sq_show (sqlite3 *db);
//创建新用户 
int sq_into (sqlite3 *db,peo *p);
//修改（按照账号和电话）
int sq_change (sqlite3 *db,peo *p);
//删除 (按照账号)
int sq_del (sqlite3 *db,peo *p);
#endif 
