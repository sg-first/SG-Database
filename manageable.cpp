#include "manageable.h"

#ifdef autoRecovery

manageContain* manageContain::contain;

void managePair::del()
{
    if(*(this->isDelete)==false)
    {
        if(this->managePoint->isOwnership()) //没被删才能访问
            delete managePoint; //用户持有所有权才删
    }
}

manageContain::~manageContain()
{
    for(managePair& pair : this->allObj)
        pair.del();
    for(managePair& pair : this->allObj)
        delete pair.isDelete;
}

#endif
