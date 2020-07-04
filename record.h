#pragma once
#include "basicType.h"
#include "helper.h"
#include "helper.h"
#include <vector>
using namespace std;

enum recType{add,del,mod};

class record
{
private:
    void setAddTarget(vector<Basic*> addTarget) //“增”时需要指定增进去的元组，会自动拷贝
    {
        for(Basic* v : addTarget)
            this->addTarget.push_back(helper::copy(v));
    }

public:
    int opSub=-1;
    recType type;

    record(vector<Basic*> addTarget) : type(add) //“增”的构造函数
    {
        this->setAddTarget(addTarget);
    }

    record(int opSub) : opSub(opSub), type(del) {} //“删”的构造函数

    record(int opSub, vector<Basic*> modTarget) : opSub(opSub), type(mod) //“改”的构造函数
    {
        this->setAddTarget(modTarget);
    }

    record(const record &r) : opSub(r.opSub), type(r.type)
    {
        this->setAddTarget(r.addTarget);
    }

    ~record()
    {
        for(Basic* v : addTarget)
            delete v;
    }

    vector<Basic*> addTarget; //仅当type!=del时使用，变更之后的元组，placeholder对象为不修改
};

