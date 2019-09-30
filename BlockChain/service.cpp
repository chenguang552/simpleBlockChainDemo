#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#include "service.h"

CBlockChainService::CBlockChainService()
{
    initServiceSocket();
}

CBlockChainService::~CBlockChainService()
{
    destroyServiceSocket();
}

int CBlockChainService::initServiceSocket()
{
    WORD versionRequired = MAKEWORD(2, 0);;
    WSADATA wsaData;
	int err = WSAStartup(versionRequired, &wsaData);//协议库的版本信息
    if (err)
    {
        std::cout << "WSAStartup err" << std::endl;
        return -1;
    }
    serviceSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serviceSocket == INVALID_SOCKET)
    {
        std::cout << "create socket err" << std::endl;
		WSACleanup();
        return -1;
    }

    sockaddr_in server ={0};
    server.sin_family = AF_INET;
    server.sin_port = htons(BLOCKCHAIN_SOCKET_PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(serviceSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cout << "Bind error" << std::endl;
        return -1;
    }

    //开始监听
    if (listen(serviceSocket, 10) == SOCKET_ERROR)
    {
        std::cout << "Listen error" << std::endl;
        return -1;
    }


    return 0;
}

int CBlockChainService::destroyServiceSocket()
{
    closesocket(serviceSocket);
    return 0;
}

//DWORD WINAPI CBlockChainService::sRecvData(LPVOID pParam)
int CBlockChainService::sRecvData()
{
    SOCKET OtherNode;
    sockaddr_in nodeAddr;
    int len = sizeof(nodeAddr);
    char msg[1024+1] = {0};
    while(1)
    {
        OtherNode = accept(serviceSocket,(sockaddr*)&nodeAddr,&len);
        if (OtherNode == INVALID_SOCKET){
            std::cout << "Accept error" << std::endl;
            return 1;
        }

        std::string strMsg = "";
        int Num = 0;
        int num = 0;
        while ((num = recv(OtherNode, msg, 1024, 0)) > 0)
        {
            strMsg.append(msg,num);
            Num += num;
        }
		std::cout << strMsg << std::endl;
        NETDATA syncData = parseData(strMsg);
        std::cout << syncData.data << std::endl;

        send(OtherNode, "00", 2, 0);
        closesocket(OtherNode);
    }

	return 0;
}

NETDATA CBlockChainService::parseData(std::string arg)
{
    if(arg.length() <= 0)
        return NETDATA("","",0,0,"");
    std::string f = arg.substr(0,6);
    std::string v = arg.substr(6,6);

    int dt;
    int dl;

    std::stringstream c2i;
    c2i << arg.substr(12,2);
    c2i >> dt;

    c2i << arg.substr(14,8);
    c2i >> dl;
    
    std::string d = arg.substr(22,dl);
    return NETDATA(f,v,dt,dl,d);
}

void CBlockChainService::start()
{
	sRecvData();
}

CBlockChainClient::CBlockChainClient()
{
	initClientSocket();
	// 将广播ip数据读入内存
	std::ifstream infile(IPSFILE);
	if(!infile.is_open())
	{
		std::cout << "load ips err" << std::endl;
		return;
	}
	std::string readBuf;
	while(getline(infile,readBuf))
	{
		ips.push_back(readBuf);
	}
	infile.close();
}

CBlockChainClient::~CBlockChainClient()
{

}

int CBlockChainClient::initClientSocket()
{
    WORD ClientSocketVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(ClientSocketVersion, &data) != 0)
    {
        return 1;
    }
    return 0;
}

int CBlockChainClient::cSendData(std::string ip,std::string data,int dataLen)
{
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Socket error" << std::endl;
        return 1;
    }
    sockaddr_in sock_in;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(BLOCKCHAIN_SOCKET_PORT);
    sock_in.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    if (connect(clientSocket, (sockaddr*)&sock_in, sizeof(sock_in) )== SOCKET_ERROR)
    {
        std::cout << "Connect error [" << ip << "]" << std::endl;
        return 1;
    }

    std::string sendData("minibcv0.1.001");
    char cdl[8+1] = {0};
    sprintf(cdl,"%08d",dataLen);
    sendData.append(cdl,8);
	sendData.append(data.c_str(),dataLen);
    send(clientSocket, sendData.c_str(), dataLen+22, 0);
/*
    char revdata[2+1] = {0};
    int num = recv(clientSocket, revdata, 2, 0);
    if (num > 0 || revdata == "00"){
        std::cout << "send success [" << ip << "]" << std::endl;
    }else{
        std::cout << "send error [" << ip << "]" << std::endl;
    }
*/
    closesocket(clientSocket);
	return 0;
}

void CBlockChainClient::broadcast(std::string data, int dataLen)
{
    // 遍历ip 发送数据
    for(auto i:ips)
    {
        cSendData(i,data,dataLen);
    }
}

