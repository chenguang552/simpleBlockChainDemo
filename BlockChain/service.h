#ifndef SERVICE_H
#define SERVICE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <WinSock2.h>
#include <windows.h>
#include <process.h>
#include <vector>

#pragma comment(lib,"ws2_32.lib")

#define BLOCKCHAIN_SOCKET_PORT 10010
#define IPSFILE "./ips.txt"
/*
监听固定端口 用来同步网络中的广播消息
消息协议
    协议标志 minibc 占6字节  协议版本v0.1.0 占6字节  数据类型 [00账单生成记录/01块生成消息/02块确认消息] 占2字节 数据大小 [00000000] 占8位 数据
*/
struct NETDATA{
    std::string flag = "minibc"; 
    std::string version = "v0.1.0";
    int dataType;
    int dataLen;
    std::string data;
    NETDATA(std::string f,std::string v,int dt,int dl,std::string d):flag(f),version(v),dataType(dt),dataLen(dl),data(d){}
};


class CBlockChainService
{
    public:
        CBlockChainService();
        ~CBlockChainService();

        void start();
    private:
        int initServiceSocket();
        int destroyServiceSocket();
        int sRecvData();
		//static DWORD WINAPI sRecvData(LPVOID pParam);
        static NETDATA parseData(std::string);

    private:
        SOCKET serviceSocket;
        HANDLE threadHandle;
};

class CBlockChainClient
{
public:
	CBlockChainClient();
	~CBlockChainClient();
	void broadcast(std::string, int);
private:
	int initClientSocket();
	int cSendData(std::string ip, std::string data, int dataLen);

private:
	std::vector<std::string> ips;
};

#endif//!SERVICE_H