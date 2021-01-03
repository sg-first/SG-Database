#pragma once
#include <vector>
#include <list>
#include "basicType.h"
#include "record.h"
#include "config.h"

class col : public manageable
{
private:
    const TYPE type;
    vector<Basic*> allData;

public:
    col(TYPE type,string ID) : type(type), ID(ID) {}
    col(const col &c) : type(c.type), ID(c.ID)
    {
        for(Basic* v : this->allData)
        {
            auto copyObj=typeHelper::copy(v);
            copyObj->setSystemManage(); //转移所有权到col
            this->allData.push_back(copyObj);
        }
    }
    string ID;
    //fix:还要有触发器和约束

    const vector<Basic*>& getAllData() { return this->allData; }

    TYPE getType() { return this->type; }

    vector<Basic*> getData(vector<int> filtered_index) //把指定下标的元素get出来
    {
        vector<Basic*> result;
        for(int index:filtered_index) {
            result.push_back(typeHelper::copy(allData[index]));
        }
        return result;
    }

    string toStr()
    {
        string result="";
        result+=(this->ID+":"+to_string(int(this->type)))+"\n";
        for(int i=0;i<this->allData.size();++i){
            result+=allData[i]->toStr()+"\n";
        }
        return result;
    }

    void pushData(Basic* v) //必须使用这个函数添加数据
    {
        if(v->getType()!=this->getType())
            throw string("type mismatch");
        else
        {
            v->setSystemManage();
            this->allData.push_back(v);
        }
    }

    col* genNewCol(const vector<int>& subList) //把指定下标的元素生成一张新表
    {
        col* result=new col(this->type,this->ID);
        for(int i : subList)
            result->allData.push_back(typeHelper::copy(this->allData[i])); //会拷贝
        return result;
    }

    bool mod(int opSub,Basic* v) //会拷贝，返回对这个值的修改是否实际进行
    {
        if(v->getType()!=PLACEHOLDER && !typeHelper::isEqu(v,this->allData[opSub]))
        {
            auto copyObj=typeHelper::copy(v);
            copyObj->setSystemManage();
            this->allData[opSub]=copyObj;
            return true;
        }
        return false;
    }

    void del(int opSub)
    {
        delete this->allData[opSub];
        this->allData.erase(this->allData.begin()+opSub);
    }

    ~col()
    {
        for(Basic* v : allData)
            delete v;
    }
};

