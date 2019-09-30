#include"Blockchain.h"
Blockchain::Blockchain()
{
    _vChain.emplace_back(Block(0,"Genesis Block"));
    _nDifficulty=3;
}
void Blockchain::broadcast(CBlockChainClient s)
{
    std::string data;

    // 按格式填充数据
    data.assign(_GetLastBlock().GetHash());

    s.broadcast(data,data.length());
}
void Blockchain::AddBlock(Block bNew)
{
    bNew.sPrevHash=_GetLastBlock().GetHash();
    bNew.MineBlock(_nDifficulty);
    _vChain.push_back(bNew);
}
Block Blockchain::_GetLastBlock() const
{
    return _vChain.back();
}