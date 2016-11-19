#ifndef BLOCK_H
#define BLOCK_H


class Block
{
private:
    bool active;
public:
    Block();
    bool isBlockActive();
    void setBlockActive(bool blockActive);
};

#endif // BLOCK_H
