#include "manageable.h"

#ifdef autoRecovery


manageContain::~manageContain()
{
    for(manageable* i : this->allObj)
    {
        if(i->getOwnership())
            delete i;
    }
}

#endif
