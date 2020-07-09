#pragma once
#include <functional>
#include <vector>
#include "rule.hpp"
#include "col.hpp"
#include "BPlusTree.h"

using namespace std;

//返回的下标组必须排序
//如果操作的是视图，就把查询结果和视图的allSub取交集（返回的还是原表的sub而不是视图表的）

class index
{
protected:
    bool supportMod=false;

public:
    virtual vector<int> find(ruleExp* rule)=0;
    col* c; //会直接跟着列变
    index(col* c) : c(c) {}

    bool isSupportMod() { return this->supportMod; }
    //都是在操作col前调用
    virtual void add(Basic* v) { throw string("This index does not support updating"); }
    virtual void mod(int opSub, Basic* v) { throw string("This index does not support updating"); }
    virtual void del(int opSub) { throw string("This index does not support updating"); }

    virtual ~index() {}
};

class traversalIndex : public index
{
public:
    traversalIndex(col* _c) : index(_c) {}

    virtual vector<int> find(ruleExp* rule)
    {
        vector<int> result;
        auto data=c->getAllData();
        for(int i=0;i<data.size();i++)
        {
            Basic* v=data[i];
            if(rule->eval(v))
                result.push_back(i);
        }
        return result; //这个本来就是从前到后遍历的，就不用排序了
    }
};

class binarySearchIndex : public index
{
private:
    vector<float> sortVec;

    void colToVec()
    {
        sortVec.clear();
        for(Basic* v : this->c->getAllData())
            sortVec.push_back(binarySearchIndex::getVal(v));
    }

    void resort()
    {
        sort(sortVec.begin(),sortVec.end());
    }

public:
    static float getVal(Basic* v)
    {
        if(v->getType()==INT)
        {
            Int* rv=(Int*)v;
            return rv->val;
        }
        else if(v->getType()==FLOAT)
        {
            Float* rv=(Float*)v;
            return rv->val;
        }
        else
            throw string("type mismatch");
    }

    binarySearchIndex(col* _c) : index(_c)
    {
        this->colToVec();
        this->resort();
    }

    virtual void add(Basic *v) override
    {
        this->sortVec.push_back(binarySearchIndex::getVal(v));
        this->resort();
    }
    virtual void mod(int opSub, Basic *v) override
    {
        this->del(opSub);
        this->add(v);
    }
    virtual void del(int opSub) override
    {
        this->sortVec.erase(this->sortVec.begin()+opSub);
    }
    virtual vector<int> find(ruleExp *rule) override
    {
        if(rule->operandIsBasic())
        {
            int value=binarySearchIndex::getVal(rule->getOperand2B());
            int sub=helper::find(this->sortVec,value);
            vector<int> result;
            if(rule->getOp()==EQU)
            {
                result.push_back(sub);
                return result;
            }
            else if(rule->getOp()==GRAT)
            {
                for(int i=sub+1;i<this->sortVec.size();i++)
                    result.push_back(this->sortVec[i]);
                return result;
            }
            else if(rule->getOp()==SMAL)
            {
                for(int i=0;i<sub;i++)
                    result.push_back(this->sortVec[i]);
                return result;
            }
        }
        throw string("this index does not support this exp");
    }
};

class BPlusTreeIndex : public index //使用该索引默认为unique约束列
{
private:
    BPlusTree* pTree;

public:
    BPlusTreeIndex(col* _c) : index(_c)
    {
        supportMod=true;
        pTree = new BPlusTree;

        bool isInt;
        if(c->getType()==INT)
            isInt=true;
        else if(c->getType()==FLOAT)
            isInt=false;
        else
            throw string("type mismatch");

        int sub=0;
        for(Basic* v : c->getAllData())
        {
            float val;
            if(isInt)
            {
                Int* rv=(Int*)v;
                val=rv->val;
            }
            else
            {
                Float* rv=(Float*)v;
                val=rv->val;
            }
            pTree->Insert(make_pair(val,sub));
            sub++;
        }
    }

    virtual void add(Basic* v)
    {
        float val=binarySearchIndex::getVal(v);
        int sub=c->getAllData().size();
        pTree->Insert(make_pair(val,sub));
    }

    virtual void del(int opSub)
    {
        Basic* v=c->getAllData()[opSub];
        pTree->Delete(binarySearchIndex::getVal(v));
    }

    virtual void mod(int opSub, Basic* v)
    {
        this->del(opSub);
        float val=binarySearchIndex::getVal(v);
        pTree->Insert(make_pair(val,opSub));
    }

    virtual vector<int> find(ruleExp* rule)
    {
        if(rule->getOp()==EQU)
        {
            Basic* v=rule->getOperand2B();
            int aresult=pTree->Search(binarySearchIndex::getVal(v));
            vector<int> result;
            result.push_back(aresult);
            return result;
        }
        else
            throw string("this index does not support this exp");
    }

    virtual ~BPlusTreeIndex() { delete pTree; }
};

