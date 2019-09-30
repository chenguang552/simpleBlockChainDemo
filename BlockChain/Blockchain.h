#include "Block.h"
#include "service.h"
#include <vector>
class Blockchain
{
public:
    Blockchain();
    void AddBlock(Block bNew);
    void broadcast(CBlockChainClient);
private:
    uint32_t _nDifficulty;
    vector<Block> _vChain;
    Block _GetLastBlock() const;//获取最新的区块，由const关键字，表示输出的内容不可更改
};