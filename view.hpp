#pragma once
#include "table.hpp"

class view //视图主要是为了方便删除修改，因此不支持添加。如果基本表被删除修改，视图即失效，因此注意生命周期
{
private:
    table *t;
    vector<int> allSub;

    int find(int i)
    {
        int result=helper::find(this->allSub,i);
        if(result==-1)
            throw string("operand elm is not in the view");
        return result;
    }

    void delElm(int opSub) //删除视图中的某个元素（不改变基本表，应当是在基本表删完了之后调用）
    {
        this->allSub.erase(this->allSub.begin()+opSub); //把视图里的元组也删了
        //后面的值得前移，因为基本表往前串了
        for(int i=opSub;i<this->allSub.size();i++) //原先的opSub删掉了，新的对应位置是后面串上来的
            this->allSub[i]--;
    }

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
        this->delElm(opSub);
    }

    void del(vector<int> allOpSub)
    {
        int opFinishedNum=0;
        for(int i : allOpSub)
        {
            this->del(i-opFinishedNum); //前面的删掉了后面下标会向前串
            opFinishedNum++;
        }
    }

    void modDir(int opSub, vector<Basic*> tuple)
    {
        t->mod(opSub,tuple);
    }

    void delDir(int opSub)
    {
        int viewSub=find(opSub); //找到视图中对应的下标
        t->del(opSub); //确认都能找到，把基本表的删了
        this->delElm(viewSub);
    }

    void delDir(vector<int> allOpSub) //warn:所有vector<int>sub全都要求排序
    {
        int opFinishedNum=0;
        for(int i : allOpSub)
        {
            this->delDir(i-opFinishedNum); //前面的删掉了后面下标会向前串
            opFinishedNum++;
        }
    }
};
