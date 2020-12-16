#pragma once
#include "col.hpp"
#include "index.hpp"
#include "IO.hpp"

class table
{
protected:
    vector<col*> allCol; //push进来直接视为持有所有权
    vector<index*> allIndex;
    list<record> allRecord;
    bool hasOwnership=true;

    void update_len(vector<vector<int> > & len_data,const string& data);

public:
    string ID;

    static table* loadFile(string path);

    string toStr();

    void saveFile(string path);

    void updateFile(string path);

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

    void changeIndex(int sub, index* ind)
    {
        delete this->allIndex[sub];
        this->allIndex[sub]=ind;
    }

    const vector<col*>& getAllCol() { return this->allCol; } //对col数据的修改必须经过table对象完成，否则无法

    table* genNewTable(const vector<int>& colSubList,const vector<int>& tupSubList)
    {
        vector<col*> newAllCol;
        for(int i : colSubList)
        {
            col* selectCol=allCol[i]->genNewCol(tupSubList);
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

            c->pushData(tuple[i]);
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

    vector<int> find(vector<ruleExp*> allExp)
    {
        //先生成一个完整的范围，用于作为nullptr（无条件）的结果
        vector<int> completeResult;
        for(int i=0;i<this->allCol[0]->getAllData().size();i++)
            completeResult.push_back(i);
        //正式逐个进行WHERE
        vector< vector<int> > allResult;
        for(int i=0;i<this->allIndex.size();i++)
        {
            if(allExp[i]!=nullptr)
            {
                auto aresult=this->allIndex[i]->find(allExp[i]);
                allResult.push_back(aresult);
            }
            else
                allResult.push_back(completeResult);
        }
        //求result中每个元素交集
        vector<int> result=allResult[0];
        for(int i=1;i<allResult.size();i++)
        {
            vector<int> v2=allResult[i];
            vector<int> intersection;
            set_intersection(result.begin(),result.end(),v2.begin(),v2.end(),inserter(intersection,intersection.begin()));
            result=intersection;
        }
        return result;
    }

    vector<int> findCol(vector<string> colID)
    {
        vector<int> result;
        for(int i=0;i<this->allCol.size();i++)
        {
            col* c=this->allCol[i];
            vector<string>::iterator it;
            it=std::find(colID.begin(),colID.end(),c->ID);
            if (it!=colID.end())
                result.push_back(i);
        }
        return result;
    }

    void clear()
    {
        for(col* c : allCol)
            delete c;
        for(index* i : allIndex)
            delete i;
    }

    ~table()
    {
        if(this->hasOwnership)
            this->clear();
    }
};


class operatTable : public table
{
private:
    bool isTrasaction=false;

public:
    static string default_path;
    operatTable(string _ID, vector<col*> _allCol) : table(_ID, _allCol) {}

    bool get_isTrasaction(){return isTrasaction;}

    void saveFile(){
        this->table::saveFile(default_path+"\\"+ID+".csv");
    }

    void updateFile(){
        this->table::updateFile(default_path+"\\"+ID+".csv");
    }

    static operatTable* loadFile(string _ID){
        return new operatTable(_ID,table::loadFile(default_path+"/"+_ID+".csv")->getAllCol());
    }

    void open_trasaction(){
        this->updateFile();
        isTrasaction=true;
    }

    void submit_trasaction(){
        if(isTrasaction==false){
            throw string("Trasacation not open");
        }
        this->updateFile();
        isTrasaction=false;
    }

    void rollback_trasaction() {
        if(isTrasaction==false){
            throw string("Trasacation not open");
        }
        this->clear();
        operatTable* newTable=operatTable::loadFile(this->ID);
        newTable->hasOwnership=false;
        this->allCol=newTable->allCol;
        this->allRecord=newTable->allRecord;
        this->allIndex=newTable->allIndex;
        isTrasaction=false;
    }
};
