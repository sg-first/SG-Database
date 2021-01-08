#include "manageable.h"

#ifdef autoRecovery

void managePair::del()
{
    if(*(this->isDelete)==false)
    {
        if(this->managePoint->isOwnership()) //没被删才能访问
            delete managePoint; //用户持有所有权才删
    }
    delete isDelete; //无论如何都把这个bool删掉，因为manageContain析构函数遍历完所有managePair都没了
}

manageContain::~manageContain()
{
    for(managePair& pair : this->allObj)
    {
        pair.del();
    }
}

#endif
