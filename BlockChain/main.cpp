
#include<iostream>
#include<cstdint>
#include "Blockchain.h"
#include "service.h"
using namespace std;


DWORD WINAPI RECV_SERVICE(LPVOID pParam)
{
	CBlockChainService s;
	s.start();
	return 0L;
}

int main()
{
	HANDLE t = CreateThread(NULL, 0, RECV_SERVICE, NULL, 0, NULL);
	
	CloseHandle(t);
	WaitForSingleObject(t, INFINITE);


	CBlockChainClient c;
    Blockchain bChain=Blockchain();
    cout<<"Mining block 1..."<<endl;
    bChain.AddBlock(Block(1,"Block 1 Data"));
    bChain.broadcast(c);

    cout<<"Mining block 2..."<<endl;
    bChain.AddBlock(Block(2,"Block 2 Data"));
    bChain.broadcast(c);
 
    cout<<"Mining block 3..."<<endl;
    bChain.AddBlock(Block(3,"Block 3 Data"));
    bChain.broadcast(c);
    
    return 0;
}