#pragma once
#include "index.hpp"
#include "IO.hpp"
#include "expParse.h"
#include <QThread>
#include <QRunnable>
#include <QThreadPool>
#ifdef JsInBasicClass
#include "jsCollection.h"
#endif


class table : public QObject, public manageable
{
    Q_OBJECT
protected:
    vector<col*> allCol; //如果hasOwnership=true，push进来的由table对象持有所有权
    vector<index*> allIndex;
    list<record> allRecord;
    vector<int> allBlocksLen;
    bool hasOwnership;
    map<vector<string>,vector<int>> memSearchSet;

    static table* loadFile(const string& path,int mark);

    void saveFile(const string& path);

    void updateFile(const string& path);

    void clear()
    {
        for(col* c : allCol)
            delete c;
        for(index* i : allIndex)
            delete i;
    }

public:
    string ID;

    static string default_path;

    Q_INVOKABLE QString toStr();

    vector<string> toStr(const int& fileLen);

    table(string ID, vector<col*> allCol,const vector<int>& blocksLen={}, bool hasOwnership=true) : ID(ID), hasOwnership(hasOwnership)
    {
        for(col* c : allCol)
        {
            if(c->getAllData().size()!=allCol[0]->getAllData().size()) { //以allCol第一个长度为准
                throw string("The number of columns is different");
            }
            if(this->hasOwnership)
                c->setSystemManage(); //默认hasOwnership=true，所以一定转移所有权
            this->allCol.push_back(c);
            this->allIndex.push_back(new traversalIndex(c)); //默认都是遍历索引
        }
        this->allBlocksLen=blocksLen;
    }

    Q_INVOKABLE table(const table& t, bool hasOwnership=true) : ID(t.ID), hasOwnership(hasOwnership)
    {
        for(col* c : t.allCol)
        {
            col* rc=new col(*c);
            if(this->hasOwnership)
                rc->setSystemManage();
            this->allCol.push_back(rc);
            this->allIndex.push_back(new traversalIndex(rc)); //索引不拷贝，重建
        }
        this->memSearchSet=t.memSearchSet;
        this->allBlocksLen=t.allBlocksLen;
    }

    void abandonOwnShip(){
        this->hasOwnership=false;
    }

    Q_INVOKABLE void changeIndex(int sub, index* ind)
    {
        delete this->allIndex[sub];
        this->allIndex[sub]=ind;
    }

    vector<col*> getAllCol(){return this->allCol;}

    Q_INVOKABLE col* getCol(int i) { return this->allCol[i]; }

    Q_INVOKABLE col* getCol(const QString& colName);

    int getColIndex(const string& colName)
    {
        for(int i=0;i<allCol.size();++i) {
            if(allCol[i]->ID==colName) {
                return i;
            }
        }
        throw string("Wrong column name");
    }

    Q_INVOKABLE int getColIndex(const QString& colName);

    table* genNewTable(const vector<int>& colSubList,const vector<int>& tupSubList) //会拷贝
    {
        vector<col*> newAllCol;
        for(int i : colSubList)
        {
            col* selectCol=allCol[i]->genNewCol(tupSubList);
            newAllCol.push_back(selectCol);
        }
        return new table(this->ID, newAllCol);
    }

    table* genNewTable(const vector<string>& colNames,const vector<int>& tupSubList)
    {
        return genNewTable(findCol(colNames),tupSubList);
    }

    #ifdef JsInBasicClass
    Q_INVOKABLE table* genNewTable(const QScriptValue& colNames,jsCollection* tupSubList);
    #endif

    Q_INVOKABLE void saveFile(){
        this->table::saveFile(default_path+"\\"+ID+".csv");
    }

    Q_INVOKABLE void updateFile(){
        this->table::updateFile(default_path+"\\"+ID+".csv");
    }

    static table* loadFile(string _ID){
        return loadFile(default_path+"\\"+_ID+".csv",0);
    }

    Q_INVOKABLE void rollback() {
        this->clear();
        table* newTable=table::loadFile(this->ID);
        newTable->hasOwnership=false; //放弃所有权，不delete里面东西了
        //在这里，之后必须接着转移数据，否则就内存泄漏了（没有人指向刚读进来那些数据对象）
        this->allCol=newTable->allCol;
        this->allRecord=newTable->allRecord;
        this->allIndex=newTable->allIndex;
        delete newTable;
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

            c->add(tuple[i]);
        }
        //写入记录
        this->allRecord.push_back(record(tuple));
        memSearchSet.clear();
    }

    Q_INVOKABLE void add(const QScriptValue& tuple);

    int mod(int opSub, vector<Basic*> tuple) //tuple里的东西会拷贝
    {
        if(tuple.size()!=this->allCol.size())
            throw string("Col Size mismatch");

        int resultSub=opSub; //记录结果

        vector<Basic*> recordTuple;
        for(int i=0;i<this->allCol.size();i++)
        {
            col* c=this->allCol[i];

            index* ind=this->allIndex[i];
            if(ind->isSupportMod())
                ind->mod(opSub,tuple[i]); //fix:下标变了索引咋整啊

            int modResult=c->mod(opSub,tuple[i]);
            if(modResult!=opSub)
            {
                auto copyObj=typeHelper::typehelper->copy(tuple[i]);
                copyObj->setSystemManage();
                recordTuple.push_back(copyObj);
                resultSub=opSub;
            }
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
        memSearchSet.clear();
        return resultSub; //返回
    }

    Q_INVOKABLE void mod(int opSub,const QScriptValue& tuple);

    Q_INVOKABLE void del(int opSub)
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
        memSearchSet.clear();
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

    #ifdef JsInBasicClass
    Q_INVOKABLE void del(jsCollection* allOpSub);
    #endif

    vector<int> find_in(const string& colName,vector<Basic*> target_vec)
    {
        vector<int> result_index_vec;
        int index=getColIndex(colName);
        for(int i=0;i<target_vec.size();++i)
        {
            vector<ruleExp*> rule_vec(this->allCol.size(),nullptr);
            rule_vec[index]=new ruleExp (EQU,typeHelper::typehelper->copy(target_vec[i]));
            vector<int> temp_result=this->doFind(rule_vec);
            result_index_vec.insert(result_index_vec.end(),temp_result.begin(),temp_result.end());
        }
        set<int>s(result_index_vec.begin(), result_index_vec.end());
        result_index_vec.assign(s.begin(), s.end());
        sort(result_index_vec.begin(),result_index_vec.end());
        return result_index_vec;
    }

    #ifdef JsInBasicClass
    Q_INVOKABLE jsCollection* find_in(const QString& colName,jsCollection* target_vec);
    #endif

    vector<int> find_not_in(const string& colName,vector<Basic*> target_vec)
    {
        vector<int> notResult;
        vector<int> inResult=find_in(colName,target_vec);
        int col_size=this->allCol[0]->getAllData().size();
        int tmp=0;
        for(int i=0;i<col_size;++i)
        {
            if(i==inResult[tmp])
            {
                ++tmp;
                continue;
            }
            notResult.push_back(i);
        }
        return notResult;
    }

    #ifdef JsInBasicClass
    Q_INVOKABLE jsCollection* find_not_in(const QString& colName,jsCollection* target_vec);
    #endif

    vector<int> doFind(vector<ruleExp*> allExp)
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

    vector<int> find(const vector<string>& allExp)
    {
        auto res=memSearchSet.find(allExp);
        if(res!=memSearchSet.end()){
            return res->second;
        }

        vector<ruleExp*> allRule;
        for(const string& str:allExp){
            allRule.push_back(expParse::total_strrule_parse(str));
        }
        auto result=doFind(allRule);

        memSearchSet[allExp]=result;

        for(ruleExp* i : allRule){
            delete i;
        }

        return result;
    }

    #ifdef JsInBasicClass
    Q_INVOKABLE jsCollection* find(const QScriptValue& allExp);
    #endif

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

    static table* doJoin(const string& newID,table* lTable,table* rTable){
        if(lTable->allCol.empty()||rTable->allCol.empty()){
            throw string("Empty table");
        }
        if(lTable->allCol[0]->getAllData().size()!=rTable->allCol[0]->getAllData().size()){
            throw string("Tables of different lengths");
        }
        vector<col*> joinCols;
        for(int i=0;i<lTable->allCol.size();++i){
            joinCols.push_back(lTable->allCol[i]);
        }
        for(int i=0;i<rTable->allCol.size();++i){
            joinCols.push_back(rTable->allCol[i]);
        }
        return new table (newID,joinCols);
    }

    virtual ~table()
    {
        if(this->hasOwnership)
            this->clear();
    }
};
