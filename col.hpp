#pragma once
#include <vector>
#include <list>
#include "basicType.h"
#include "record.h"
#include "config.h"

class col
{
private:
    const TYPE type;
    vector<Basic*> allData; //push进来直接视为持有所有权

public:
    col(TYPE type,string ID) : type(type), ID(ID) {}
    col(const col &c) : type(c.type), ID(c.ID)
    {
        for(Basic* v : this->allData)
            this->allData.push_back(typeHelper::copy(v));
    }
    string ID;
    //fix:还要有触发器和约束

    const vector<Basic*>& getAllData() { return this->allData; }

    TYPE getType() { return this->type; }

    vector<Basic*> getDate(vector<int> filtered_index){
        vector<Basic*> result;
        for(int index:filtered_index){
            result.push_back(typeHelper::copy(allData[index]));
        }
        return result;
    }

    void pushData(Basic* v) //应该使用这个函数push
    {
        if(v->getType()!=this->getType())
            throw string("type mismatch");
        else
            this->allData.push_back(v);
    }

    col* genNewCol(const vector<int>& subList)
    {
        col* result=new col(this->type,this->ID);
        for(int i : subList)
            result->allData.push_back(typeHelper::copy(this->allData[i]));
        return result;
    }

    bool mod(int opSub,Basic* v) //会拷贝，返回对这个值的修改是否实际进行
    {
        if(v->getType()!=PLACEHOLDER && !typeHelper::isEqu(v,this->allData[opSub]))
        {
            delete this->allData[opSub];
            this->allData[opSub]=typeHelper::copy(v);
            return true;
        }
        return false;
    }

    void del(int opSub)
    {
        this->allData.erase(this->allData.begin()+opSub);
    }

    ~col()
    {
        for(Basic* v : allData)
            delete v;
    }
};

