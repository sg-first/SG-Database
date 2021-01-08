#pragma once
#include <list>
#include "config.h"
using namespace std;

#ifdef autoRecovery

class manageable;

class managePair
{
private:
    manageable* managePoint;
    bool* isDelete;
    //managePair(manageable* managePoint) : managePoint(managePoint) {}
    void del(); //仅限reset时调用
    friend class manageContain;

public:
    bool* getIsDelete() { return this->isDelete; }
    static managePair newPair(manageable* managePoint)
    {
        managePair pair;
        pair.managePoint=managePoint;
        pair.isDelete=new bool(false);
        return pair;
    }
};

class manageContain
{
private:
    list<managePair> allObj;

public:
    static manageContain* contain;
    static void init() { manageContain::contain=new manageContain; }
    static void reset()
    {
        delete manageContain::contain;
        manageContain::init();
    }

    void add(managePair m) { this->allObj.push_back(m); }
    ~manageContain();
};

#endif

class manageable
{
private:
    bool userManage=true;
    #ifdef autoRecovery
    bool* isDelete;
    #endif

public:
    manageable()
    {
        #ifdef autoRecovery
        managePair pair=managePair::newPair(this);
        this->isDelete=pair.getIsDelete();
        manageContain::contain->add(pair);
        #endif
    }
    bool isOwnership() { return this->userManage; }
    void setSystemManage() { this->userManage=false; }
    virtual ~manageable()
    {
        #ifdef autoRecovery
        *isDelete=true;
        #endif
    }
};
