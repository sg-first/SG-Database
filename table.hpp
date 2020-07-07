#pragma once
#include "col.hpp"
#include "index.hpp"
#include "IO.hpp"

class table
{
private:
    vector<col*> allCol; //push进来直接视为持有所有权
    vector<index*> allIndex;
    list<record> allRecord;

    string table_to_str(vector<vector<int>>& len_data);

public:
    string ID;
    table(string ID, vector<col*>allCol) : allCol(allCol), ID(ID) //allCol中元素转移所有权
    {
        for(col* c : allCol)
            this->allIndex.push_back(new traversalIndex(c)); //默认都是遍历索引
    }

    table(const table& t) : ID(t.ID)
    {
        for(col* c : t.allCol)
        {
            col* rc=new col(*c);
            this->allCol.push_back(rc);
            this->allIndex.push_back(new traversalIndex(rc)); //索引不拷贝，重建
        }
    }

    const vector<col*>& getAllCol() { return this->allCol; } //对col数据的修改必须经过table对象完成，否则无法

    static table* loadFile(string path);

    void saveFile(string path) //将整个表的内容按约定格式写入空文件
    {
        vector<vector<int>> len_data;
        string data=this->table_to_str(len_data);
        IO::write_to_file(path,data);
        IO::write_to_len_file(path,len_data);
    }

    void updateFile(string path);

    table* genNewTable(vector<int> subList) //注意这个视图是个新表，和原表没有对应关系
    {
        vector<col*> newAllCol;
        for(col* c : allCol)
        {
            col* selectCol=c->genNewCol(subList);
            newAllCol.push_back(selectCol);
        }
        table* result=new table(this->ID, newAllCol);
        return result;
    }

    void add(vector<Basic*> tuple)
    {
        if(tuple.size()!=this->allCol.size())
            throw string("Col Size mismatch");
        for(int i=0;i<this->allCol.size();i++)
        {
            col* c=this->allCol[i];

            index* ind=this->allIndex[i];
            if(ind->isSupportMod())
                ind->add(tuple[i]);

            c->pushDate(tuple[i]);
        }
        //写入记录
        this->allRecord.push_back(record(tuple));
    }

    void mod(int opSub, vector<Basic*> tuple)
    {
        if(tuple.size()!=this->allCol.size())
            throw string("Col Size mismatch");

        vector<Basic*> recordTuple;
        for(int i=0;i<this->allCol.size();i++)
        {
            col* c=this->allCol[i];

            index* ind=this->allIndex[i];
            if(ind->isSupportMod())
                ind->mod(opSub,tuple[i]);

            bool modResult=c->mod(opSub,tuple[i]);
            if(modResult)
                recordTuple.push_back(typeHelper::copy(tuple[i]));
            else
            {
                #ifdef logStrategy
                recordTuple.push_back(new Placeholder()); //如果是不起实际作用的修改，就用Placeholder占位以免多占record空间
                #else
                recordTuple.push_back(c->getAllData()[opSub]);
                #endif
            }
        }

        //写入记录
        this->allRecord.push_back(record(opSub,recordTuple));
    }

    void del(int opSub)
    {
        for(int i=0;i<this->allCol.size();i++)
        {
            index* ind=this->allIndex[i];
            if(ind->isSupportMod())
                ind->del(opSub);
            this->allCol[i]->del(opSub);
        }
        //写入记录
        this->allRecord.push_back(record(opSub));
    }

    void del(vector<int> allOpSub) //对选择出来的一系列下标一起进行删除，要求有序
    {
        int opFinishedNum=0;
        for(int i : allOpSub)
        {
            this->del(i-opFinishedNum); //前面的删掉了后面下标会向前串
            opFinishedNum++;
        }
    }

    ~table()
    {
        for(col* c : allCol)
            delete c;
        for(index* i : allIndex)
            delete i;
    }
};
