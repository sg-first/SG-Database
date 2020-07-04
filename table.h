#pragma once
#include <vector>
#include <list>
#include "basicType.h"
#include "record.h"

class col
{
private:
    const string type;

public:
    col(string type,string ID) : type(type), ID(ID) {}
    col(const col &c) : type(c.type), ID(c.ID)
    {
        for(Basic* v : this->allData)
            this->allData.push_back(helper::copy(v));
    }
    string ID;
    //fix:还要有触发器和约束
    vector<Basic*> allData; //push进来直接视为持有所有权
    list<colModTable> allRecord;
    string getType() { return this->type; }

    void pushDate(Basic* v) //应该使用这个函数push
    {
        if(v->getType()!=this->getType())
            throw string("type mismatch");
        else
            this->allData.push_back(v);
    }

    col* genView(vector<int> subList)
    {
        col* result=new col(type,ID);
        for(int i : subList)
            result->allData.push_back(helper::copy(this->allData[i]));
        return result;
    }

    void mod(int opSub,Basic* v) //会拷贝
    {
        if(!helper::isEqu(v,this->allData[opSub]))
        {
            delete this->allData[opSub];
            this->allData[opSub]=helper::copy(v);
            //写入记录
            this->allRecord.push_back(colModTable(opSub,v));
        }
    }

    ~col()
    {
        for(Basic* v : allData)
            delete v;
    }
};


class table
{
public:
    string ID;
    table(string ID) : ID(ID) {}
    table(const table& t) : ID(t.ID)
    {
        for(col* c : t.allCol)
            this->allCol.push_back(new col(*c));
    }

    vector<col*> allCol; //push进来直接视为持有所有权
    list<tableRecord> allRecord;

    static table* loadFile(string path) //按约定格式从文件中读取表
    {
        //fix:编写此函数
    }

    void saveFile(string path) //将整个表的内容按约定格式写入空文件
    {
        //fix:编写此函数
    }

    void updateFile(string path) //根据table.allRecord（对表中元组的增删）和每个col的allRecord（对某列某元素的改）更新文件内容
    {
        //fix:编写此函数

        //全部写入后清除record记录
        this->allRecord.clear();
        for(col* c : allCol)
            c->allRecord.clear();
    }

    table* genView(vector<int> subList) //注意这个视图是个新表，和原表没有对应关系
    {
        table* result=new table(this->ID);
        for(col* c : allCol)
        {
            col* selectCol=c->genView(subList);
            result->allCol.push_back(selectCol);
        }
        return result;
    }

    void add(vector<Basic*> tuple)
    {
        if(tuple.size()!=this->allCol.size())
            throw string("Col Size mismatch");
        for(int i=0;i<this->allCol.size();i++)
        {
            col* c=this->allCol[i];
            c->pushDate(tuple[i]);
        }
        //写入记录
        this->allRecord.push_back(tableRecord(tuple));
    }

    ~table()
    {
        for(col* c : allCol)
            delete c;
    }
};
