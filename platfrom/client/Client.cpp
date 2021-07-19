/*====================================================
*   Copyright (C) 2021  All rights reserved.
*
*   文件名称：Client.cpp
*   创 建 者：王东方  541306310@qq.com
*   创建日期：2021年02月10日
*   描    述：
================================================================*/
#include "client.h"
using namespace std;
Clientfunc::Clientfunc (ClientInfo *info)
{
	memcpy(&m_info,info,sizeof(ClientInfo));
	m_shm =new SecKeyShm(info->shmKey,info->maxNode);
}
void Clientfunc:: getRandstring(char *randbuf,int len)
{
	srand(time(NULL));
	int flag;
	char buf[10]="!@#$%^&*";
	for (int i=0;i<len-1;i++)
	{
		flag=rand()%4;
		switch(flag)
		{
		case 0:
			randbuf[i]=rand()%10+'0';
			break;
		case 1:
			randbuf[i]=rand()%26+'a';
			break;                                                      
		case 2:
			randbuf[i]=rand()%26+'A';
			break;
		case 3:
			randbuf[i]=buf[rand()%strlen(buf)];
			break;
		default:
			break;
		}
	}
}
int Clientfunc ::secKeyagree()
{
	char randbuf[64]={0};
	getRandstring(randbuf,sizeof(randbuf));
	cout<<"随机数"<<randbuf<<endl;
	RequestMsg msg;
	memset(&msg,0x00,sizeof(RequestMsg));
	msg.cmdType=RequestCodec::NewOrUpdate;
	strcpy(msg.clientId,m_info.clientID);
	strcpy(msg.serverId,m_info.serverID);
	strcpy(msg.r1, randbuf);
	//hmac函数生成哈希值
	char key[1024] = {0};
	sprintf(key,"@%s+%s@",m_info.serverID,m_info.clientID);
	cout<<"key:"<<key<<endl;
	unsigned char md[SHA256_DIGEST_LENGTH];
	unsigned int len=0;
	HMAC(EVP_sha256(), key,strlen(key), (unsigned char *)msg.r1, strlen((char*)msg.r1), md, &len);
	for (int i = 0; i < SHA256_DIGEST_LENGTH;i++)
	{
		sprintf(&msg.authCode[2 * i], "%02x", md[i]);
	}
	cout<<"消息验证码"<<msg.authCode<<endl;
	//报文编解码
	char* outdata;
	int datalen;
	CodecFactory* fac = new RequestFactory(&msg);
	Codec* pcode = fac->createCodec();
	pcode->msgEncode(&outdata, datalen);
	delete fac,pcode;
	m_socket.connectToHost(m_info.serverIP,m_info.serverPort);
	//发送数据
	m_socket.sendMsg(outdata, datalen);
	char* indata;
	m_socket.recvMsg(&indata, datalen);
	//解码
	CodecFactory *factory = new RespondFactory();
	Codec *pCodec = factory->createCodec();
	RespondMsg *pmsg = (RespondMsg *)pCodec->msgDecode(indata, datalen);
	//判断是否成功
	if (pmsg->rv == -1)
	{
		cout << "faile" << endl;
		return -1;
	}
	else
	{
		cout << "成功"<< endl;
	}
	//将服务端的r2和客户端的r1拼接
	unsigned char buf[1024];
	sprintf((char*)buf, "%s%s", msg.r1, pmsg->r2);
	unsigned char md1[SHA_DIGEST_LENGTH];
	char seckey[SHA_DIGEST_LENGTH * 2 + 1];
	SHA1(buf, strlen((char*)buf), md1);
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&seckey[i * 2], "%02x", md1[i]);
	}
	NodeSHMInfo node;
	strcpy(node.seckey,seckey);
	strcpy(node.clientID, msg.clientId);
	strcpy(node.serverID, msg.serverId);
	node.seckeyID = pmsg->seckeyid;
	cout<<"秘钥"<<seckey<<endl;
	m_shm->shmWrite(&node);
	m_socket.disConnect();
	free (indata);
}
Clientfunc::~Clientfunc()
{}
