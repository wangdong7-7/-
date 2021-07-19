#include "ServerOperation.h"
#include <iostream>
#include <pthread.h>
#include <string.h>
#include "RequestFactory.h"
#include "RespondFactory.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <signal.h>
using namespace std;

bool ServerOperation::m_stop = false;	// 静态变量初始化

ServerOperation::ServerOperation(ServerInfo * info)
{
	memcpy(&m_info,info,sizeof(ServerInfo));
	m_shm = new SecKeyShm(info->shmkey, info->maxnode);

}

ServerOperation::~ServerOperation()
{
	delete m_shm;
}

void ServerOperation::startWork()
{
	m_server.setListen(m_info.sPort);
	while (1)
	{
		TcpSocket* socket = m_server.acceptConn(3);
		if (socket == NULL)
		{
			cout << "accept 超时或失败" << endl;
			continue;
		}
		cout << "客户端成功连接服务器..." << endl;
		// 创建子线程
		pthread_t pid;
		pthread_create(&pid, NULL, working, this);
		// 线程分离, 子线程自己释放pcb
		pthread_detach(pid);
		m_listSocket.insert(make_pair(pid, m_client));
	}
}

int ServerOperation::secKeyAgree(RequestMsg * reqMsg, char ** outData, int & outLen)
{
	//验证消息认证码
	char key[64];
	unsigned int len;
	unsigned char md[SHA256_DIGEST_LENGTH];
	memset(key, 0x00, sizeof(key));
	char authCode[SHA256_DIGEST_LENGTH * 2 + 1];
	sprintf(key, "@%s+%s@", reqMsg->serverId, reqMsg->clientId);
	HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)reqMsg->r1, strlen(reqMsg->r1), md, &len);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&authCode[2 * i], "%02x", md[i]);
	}
	int ret=strcmp(authCode, reqMsg->authCode);
	if (ret != 0)
	{

		cout << "错误" << endl;
		return -1;
	}
	//r1,r2拼接，生成密钥
	RespondMsg rspMsg;
	getRandString(sizeof(rspMsg.r2),rspMsg.r2);
	char buf[1024];
	unsigned char md1[SHA_DIGEST_LENGTH];
	memset(md1, 0x00, sizeof(md1));
	char seckey[SHA_DIGEST_LENGTH * 2 + 1];
	memset(buf, 0x00, sizeof(buf));
	memset(seckey, 0x00, sizeof(seckey));
	sprintf(buf, "%s%s", reqMsg->r1, rspMsg.r2);
	SHA1((unsigned char*)buf, strlen((char*)buf), md1);
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&seckey[i * 2], "%02x", md1[i]);
	}
	cout << "秘钥: " << seckey << endl;
	rspMsg.seckeyid = 1;
	strcpy(rspMsg.serverId,m_info.serverID);
	strcpy(rspMsg.clientId, reqMsg->clientId);
	//发送数据
	int datalen;
	char* sendData;
	CodecFactory* fac = new RespondFactory(&rspMsg);
	Codec* pCode = fac->createCodec();
	pCode->msgEncode(&sendData, datalen);
	pthread_t pid = pthread_self();
	TcpSocket* server =m_listSocket[pid];
	server->sendMsg(sendData,datalen);
	//写入共享内存
	NodeSHMInfo node;
	memset(&node, 0x00, sizeof(NodeSHMInfo));
	node.status = 0;
	strcpy(node.seckey, seckey);
	strcpy(node.clientID, rspMsg.clientId);
	strcpy(node.serverID, rspMsg.serverId);
	node.seckeyID = rspMsg.seckeyid;

	//将秘钥信息写入共享内存
	m_shm->shmWrite(&node);

	//关闭网络连接
	free(sendData);
	delete fac;
	delete pCode;

	return 0;
}



void ServerOperation::getRandString(int len, char * randBuf)
{
	int flag = -1;
	// 设置随机种子
	srand(time(NULL));
	// 随机字符串: A-Z, a-z, 0-9, 特殊字符(!@#$%^&*()_+=)
	char chars[] = "!@#$%^&*()_+=";
	for (int i = 0; i < len - 1; ++i)
	{
		flag = rand() % 4;
		switch (flag)
		{
		case 0:
			randBuf[i] = 'Z' - rand() % 26;
			break;
		case 1:
			randBuf[i] = 'z' - rand() % 26;
			break;
		case 3:
			randBuf[i] = rand() % 10 + '0';
			break;
		case 2:
			randBuf[i] = chars[rand() % strlen(chars)];
			break;
		default:
			break;
		}
	}
	randBuf[len - 1] = '\0';
}

// 友元函数, 可以在该友元函数中通过对应的类对象调用期私有成员函数或者私有变量
// 子线程 - 进行业务流程处理
void * working(void * arg)
{
	puts("star working!!");
	if (arg == NULL)
		exit(0);
	//接收数据
	pthread_t pid = pthread_self();
	ServerOperation* op = (ServerOperation*)arg;
	TcpSocket *server = op->m_listSocket[pid];
	if (server ==NULL)
	{
		puts("server error");
		exit(-1);
	}
	char *indata=NULL;
	int datalen=-1;
	puts("开始接受数据");
	server->recvMsg(&indata,datalen);
	//解码
	CodecFactory *fac = new RequestFactory();
	Codec *pCodec = fac->createCodec();
	RequestMsg* pMsg = (RequestMsg*)pCodec->msgDecode(indata, datalen);
	puts("请求接受完成");
	char* outdata;
	switch (pMsg->cmdType)
	{
	case RequestCodec::NewOrUpdate:
		op->secKeyAgree(pMsg,&outdata,datalen);
		break;

	case RequestCodec::Check:
		op->secKeyCheck();
		break;
	case RequestCodec::Revoke:
		op->secKeyRevoke();
	case RequestCodec::View:
		op->secKeyView();
		break;
	case 0:
		exit(0);

	default:
		break;
	}
}
	

