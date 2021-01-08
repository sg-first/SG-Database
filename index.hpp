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
    //都是在操作col前调用，如果支持修改下面三个必须都支持
    virtual void add(Basic* v) { throw string("This index does not support updating"); } //和col里的共用一个数据对象，无所有权
    virtual void mod(int opSub, Basic* v) { throw string("This index does not support updating"); }
    virtual void del(int opSub) { throw string("This index does not support updating"); } //因为无所有权，所以不删除数据对象

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
    vector<pair<float,int>> sortVec;

    void colToVec()
    {
        sortVec.clear();
        auto allData=this->c->getAllData();
        for(int i=0;i<allData.size();i++)
        {
            Basic* v=allData[i];
            sortVec.push_back(make_pair(binarySearchIndex::getVal(v),i));
        }
    }

    static int binFind(const vector<pair<float,int>> &invec, float value) //返回的是sub和opSub（表中的）
    {
      int low = 0, high = invec.size()-1;
      //assert(!invec.empty() && pos>=0);
      while(low <=high)
      {
          int mid = (low+high)/2;
          if(invec[mid].first == value)
              return mid;
          else if(invec[mid].first < value)
            low = mid+1;
          else
            high = mid-1;
      }
      return -1;
    }

    void resort()
    {
        sort(sortVec.begin(),sortVec.end(),
             [](pair<int,int> i1,pair<int,int> i2) {
                return i1.first<i2.first;
             }
        );
    }

public:
    static float getVal(Basic* v)
    {
        if(v->getType()==INT)
        {
            return ((Int*)v)->val;
        }
        else if(v->getType()==FLOAT)
        {
            return ((Float*)v)->val;
        }
        else
            throw string("type mismatch");
    }

    binarySearchIndex(col* _c) : index(_c)
    {
        this->colToVec();
        this->resort();
    }

    virtual void add(Basic* v) override
    {
        int sub=sortVec.size();
        this->sortVec.push_back(make_pair(binarySearchIndex::getVal(v),sub));
        this->resort();
    }

    virtual void mod(int opSub, Basic* v) override
    {
        int sub=-1;
        for(auto i : this->sortVec)
        {
            if(i.second==opSub)
                sub=i.second;
        }
        this->del(sub);
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
            int sub=binFind(this->sortVec,value); //返回的是sub和opSub
            int leftSub,rightSub;
            const float& target=sortVec[sub].first;
            for(int i=sub-1;i>=0;--i){
                if(sortVec[i].first!=target){
                    leftSub=i+1;
                    break;
                }
                if(i==0){
                    leftSub=0;
                }
            }
            for(int i=sub+1;i<sortVec.size();++i){
                if(sortVec[i].first!=target){
                    rightSub=i-1;
                    break;
                }
                if(i==sortVec.size()-1){
                    rightSub=sortVec.size()-1;
                }
            }
            //fix:找到以sub为中心，最左和最右等于value的，把它们赋值给leftSub和rightSub
            vector<int> result;
            if(rule->getOp()==EQU)
            {
                for(int i=leftSub;i<=rightSub;i++)
                    result.push_back(this->sortVec[i].second);
                return result;
            }
            else if(rule->getOp()==GRAT)
            {
                for(int i=rightSub+1;i<this->sortVec.size();i++)
                    result.push_back(this->sortVec[i].second);
                return result;
            }
            else if(rule->getOp()==SMAL)
            {
                for(int i=0;i<leftSub;i++)
                    result.push_back(this->sortVec[i].second);
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
                val=((Int*)v)->val;
            }
            else
            {
                val=((Float*)v)->val;
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

