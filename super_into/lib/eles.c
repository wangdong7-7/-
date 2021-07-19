/*====================================================
*   Copyright (C) 2020  All rights reserved.
*
*   文件名称：eles.c
*   创 建 者：王东方  541306310@qq.com
*   创建日期：2020年07月21日
*   描    述：
================================================================*/

#include "client.h"
void my_fill (struct people *stu,int flag)
{
	if (flag ==0)
	{
		puts ("请输入账号");
		scanf ("%s",stu->acount);
		puts ("请输入密码");
		scanf ("%s",stu->passwd);
		puts ("请输入电话");
		scanf ("%s",stu->tel);
	}
	if (flag ==3)
	{
		puts ("请输入账号");
		scanf ("%s",stu->acount);
		puts ("请输入密码");
		scanf ("%s",stu->passwd);
	}

	if (flag ==1)
	{
		puts ("请输入账号");
		scanf ("%s",stu->acount);
		puts ("请输入要修改的密码");
		scanf ("%s",stu->passwd);
		puts ("请输入绑定的电话");
		scanf ("%s",stu->tel);
	}
	if (flag ==2)
	{
		puts ("请输入要注销的账号");
		scanf ("%s",stu->acount);
	}
}
void prt(void)
{
		printf ("\t\t*****************************************\n");
		printf ("\t\t*                                       *\n");
		printf ("\t\t*           超级杂货铺登录界面          *\n");
		printf ("\t\t*                                       *\n");
		printf ("\t\t*  1：注册账号        2：登录账号       *\n");
		printf ("\t\t*  3：修改密码        4：注销账号       *\n");
		printf ("\t\t*             0: 退出系统               *\n");
		printf ("\t\t*                                       *\n");
		printf ("\t\t*  5：关闭背景音乐    6：暂停背景音乐   *\n");
		printf ("\t\t*  7：继续播放背景音乐                  *\n");
		printf ("\t\t*                                       *\n");
		printf ("\t\t*****************************************\n\n");

}
void prt1(void)
{
		printf ("\t\t*****************************************\n");
		printf ("\t\t*                                       *\n");
		printf ("\t\t*              超级杂货铺               *\n");
		printf ("\t\t*                                       *\n");
		printf ("\t\t*  11：查询货物清单        22：下单     *\n");
		printf ("\t\t*  33：退单                44：退出     *\n");
		printf ("\t\t*  55: 收听通知                         *\n");
		printf ("\t\t*                                       *\n");
		printf ("\t\t*****************************************\n\n");

}


