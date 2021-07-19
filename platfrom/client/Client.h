/*====================================================
*   Copyright (C) 2021  All rights reserved.
*
*   文件名称：Client.h
*   创 建 者：王东方  541306310@qq.com
*   创建日期：2021年02月10日
*   描    述：
================================================================*/

#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "TcpSocket.h"
#include "SecKeyShm.h"
#include "RequestCodec.h"
#include <time.h>
#include "RequestCodec.h"
#include <string.h>
#include <time.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include "CodecFactory.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include <iostream>
class ClientInfo
{
public:
	char clientID[12];
	char serverID[12];
	char serverIP[32];
	unsigned short serverPort;
	int maxNode;
	int shmKey;
};
class Clientfunc
{
public:
	Clientfunc (ClientInfo * info);
	~Clientfunc();
	int secKeyagree();
	// 秘钥校验
	int secKeyCheck() {}
	// 秘钥注销
	int secKeyRevoke() {}
	// 秘钥查看
	int secKeyView() {}
private:
	void getRandstring(char *buf,int len);
private:
	ClientInfo m_info;
	TcpSocket m_socket;
	SecKeyShm *m_shm;
};

#endif 
