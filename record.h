#pragma once
#include "basicType.h"
#include "helper.h"
#include "helper.h"
#include <vector>
using namespace std;

enum recType{add,del,mod};

class recordBase
{
public:
    int opSub; //增不需要这个，但另两个都需要，而且还分成了两个类，为了方便就把它放在基类里了
    recType type;
    recordBase(int opSub, recType type) : opSub(opSub), type(type) {}
};


class tableRecord : public recordBase //对表中整个元组的增删
{
private:
    void setAddTarget(vector<Basic*> addTarget) //“增”时需要指定增进去的元组，会自动拷贝
    {
        for(Basic* v : addTarget)
            this->addTarget.push_back(helper::copy(v));
    }

public:
    tableRecord(vector<Basic*> addTarget) : recordBase(-1,add) //“增”的构造函数
    {
        this->setAddTarget(addTarget);
    }

    tableRecord(int _opSub) : recordBase(_opSub,del) {} //“删”的构造函数

    tableRecord(const tableRecord &r) : recordBase(r.opSub,r.type)
    {
        this->setAddTarget(r.addTarget);
    }

    ~tableRecord()
    {
        for(Basic* v : addTarget)
            delete v;
    }

    vector<Basic*> addTarget; //仅当type!=del时使用，变更之后的元组，placeholder对象为不修改
};


class colModTable : public recordBase //对某列某个元素的改
{
public:
    Basic* modTarget;
    colModTable(int _opSub, Basic* modTarget) : recordBase(_opSub,mod),
        modTarget(helper::copy(modTarget)) {}
    colModTable(const colModTable &r) : recordBase(r.opSub,r.type), modTarget(helper::copy(r.modTarget)) {}
    ~colModTable() { delete this->modTarget; }
};
