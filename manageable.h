#pragma once
#include <list>
#include "config.h"
using namespace std;

#ifdef autoRecovery

class manageable;

class manageContain
{
private:
    list<manageable*> allObj;

public:
    static manageContain* contain;
    static void init() { manageContain::contain=new manageContain; }
    static void reset()
    {
        manageContain* ts=manageContain::contain;
        delete manageContain::contain;
        manageContain::init();
    }

    void add(manageable* m) { this->allObj.push_back(m); }
    ~manageContain();
};

#endif

class manageable
{
private:
    bool userManage=true;

public:
    manageable()
    {
        #ifdef autoRecovery
        manageContain::contain->add(this);
        #endif
    }
    bool getOwnership() { return this->userManage; }
    void setSystemManage() { this->userManage=false; }
    virtual ~manageable() {}
};
