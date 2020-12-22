#pragma once
#include "basicType.h"
#include "typeHelper.hpp"
#include "typeHelper.hpp"
#include <vector>
using namespace std;

enum recType{ADD,DEL,MOD};

class record
{
private:
    void setAddTarget(vector<shared_ptr<Basic>> addTarget) //“增”时需要指定增进去的元组，会自动拷贝
    {
        for(shared_ptr<Basic> v : addTarget)
            this->targetTuple.push_back(typeHelper::copy(v));
    }

public:
    int opSub=-1;
    recType type;

    record(vector<shared_ptr<Basic>> addTarget) : type(ADD) //“增”的构造函数
    {
        this->setAddTarget(addTarget);
    }

    record(int opSub) : opSub(opSub), type(DEL) {} //“删”的构造函数

    record(int opSub, vector<shared_ptr<Basic>> modTarget) : opSub(opSub), type(MOD) //“改”的构造函数
    {
        this->setAddTarget(modTarget);
    }

    record(const record &r) : opSub(r.opSub), type(r.type)
    {
        this->setAddTarget(r.targetTuple);
    }

    ~record(){}

    vector<shared_ptr<Basic>> targetTuple; //仅当type!=del时使用，变更之后的元组，placeholder对象为不修改
};

