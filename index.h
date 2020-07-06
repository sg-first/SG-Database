#pragma once
#include <functional>
#include <vector>
#include "rule.h"
#include "table.h"
#include "BPlusTree.h"

using namespace std;

//每种索引是一个函数工厂，参数是col，返回根据这个col生成的查询函数，替换原先的查询对象
//查询对象输入一个条件，返回查询结果。也就是说查询对象里面要使用解析条件的组件
//返回的下标组必须排序
//如果操作的是视图，就把查询结果和视图的allSub取交集（返回的还是原表的sub而不是视图表的）

typedef function<vector<int>(ruleExp rule)> findFun;

class index
{
protected:
    bool supportMod=false;

public:
    virtual vector<int> find(ruleExp rule)=0;
    col* c; //会直接跟着列变
    index(col* c) : c(c) {}

    bool isSupportMod() { return this->supportMod; }
    virtual void add(vector<Basic*> tuple) { throw string("This index does not support updating"); }
    virtual void mod(int opSub, vector<Basic*> tuple) { throw string("This index does not support updating"); }
    virtual void del(int opSub) { throw string("This index does not support updating"); }

    virtual ~index() {}
};

class traversalIndex : public index
{
public:
    virtual vector<int> find(ruleExp rule)
    {
        vector<int> result;
        auto data=c->getAllData();
        for(int i=0;i<data.size();i++)
        {
            Basic* v=data[i];
            if(rule.eval(v))
                result.push_back(i);
        }
        return result;
    }
};

class BPlusTreeIndex : public index
{
private:
    BPlusTree* pTree;

public:
    BPlusTreeIndex(col* _c) : index(_c)
    {
        supportMod=true;
        pTree = new BPlusTree;
    }

    virtual ~BPlusTreeIndex() { delete pTree; }
};

