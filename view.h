#pragma once
#include "table.h"

class view //视图主要是为了方便删除修改，因此不支持添加。如果原表被删除修改，视图即失效，因此注意生命周期
{
private:
    table *t;
    vector<int> allSub;

public:
    string ID;
    view(table* t, vector<int> allSub) : t(t), allSub(allSub) {} //allSub必须排序

    void mod(int opSub, vector<Basic*> tuple)
    {
        t->mod(this->allSub[opSub],tuple);
    }

    void del(int opSub)
    {
        t->del(this->allSub[opSub]);
        this->allSub.erase(this->allSub.begin()+opSub); //把视图里的元组也删了
    }

    void del(vector<int> allOpSub)
    {
        //把视图的sub转成原表的sub
        vector<int> realSub;
        for(int i : allOpSub)
            realSub.push_back(this->allSub[i]);
        t->del(realSub);
        //删视图里的
        int opFinishedNum=0;
        for(int i : allOpSub)
        {
            this->allSub.erase(this->allSub.begin()+i-opFinishedNum); //前面的删掉了后面下标会向前串
            opFinishedNum++;
        }
    }

    void modDir(int opSub, vector<Basic*> tuple)
    {
        t->mod(opSub,tuple);
    }

    void delDir(int opSub)
    {
        t->del(opSub);
        //找到原先的删掉
        int viewSub=helper::find(this->allSub,opSub);
        this->allSub.erase(this->allSub.begin()+viewSub);
    }

    void delDir(vector<int> allOpSub) //warn:所有vector<int>sub全都要求排序
    {
        t->del(allOpSub);
        //按顺序找到视图的下标
        vector<int>viewSub;
        for(int i : allOpSub)
        {
            int sub=helper::find(this->allSub,i);
            viewSub.push_back(sub);
        }
        //删视图里的
        int opFinishedNum=0;
        for(int i : viewSub)
        {
            this->allSub.erase(this->allSub.begin()+i-opFinishedNum); //前面的删掉了后面下标会向前串
            opFinishedNum++;
        }
    }
};
