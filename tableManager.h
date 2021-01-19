#pragma once
#include <string>
#include <map>
#include "table.hpp"
#include <QtScript/QScriptEngine>
using namespace std;

static vector<ruleExp*> constructQuery(const int& size,const int& loc,Basic* tmpCopy){
    vector<ruleExp*> queryVec(size,nullptr);
    queryVec[loc]=new ruleExp(EQU,tmpCopy);
    return queryVec;
}

static tuple<vector<int>,vector<int>> inner(table* lTable,table* rTable,const QString& lKey,const QString& rKey){
    col* lCol=lTable->getCol(lKey);
    col* rCol=rTable->getCol(rKey);
    if(lCol->getType()!=rCol->getType()){
        throw string("Wrong data type");
    }
    vector<int> lIndex;
    vector<int> rIndex;
    for(int i=0;i<lCol->getAllData().size();++i){
        Basic* tmpCopy=typeHelper::typehelper->copy(lCol->getAllData()[i]);
        int size=rTable->getAllCol().size();
        int loc=rTable->findCol({rKey.toStdString()})[0];
        vector<int> tmpVec=rTable->doFind(constructQuery(size,loc,tmpCopy));
        if(tmpVec.empty()==true){
            continue;
        }
        lIndex.insert(lIndex.end(),tmpVec.size(),i);
        rIndex.insert(rIndex.end(),tmpVec.begin(),tmpVec.end());
    }
    return make_tuple(lIndex,rIndex);
}

static tuple<vector<int>,vector<int>> devition(table* lTable,table* rTable,const QString& lKey,const QString& rKey){
    col* lCol=lTable->getCol(lKey);
    col* rCol=rTable->getCol(rKey);
    if(lCol->getType()!=rCol->getType()){
        throw string("Wrong data type");
    }
    vector<int> lIndex;
    vector<int> rIndex;
    for(int i=0;i<lCol->getAllData().size();++i){
        Basic* tmpCopy=typeHelper::typehelper->copy(lCol->getAllData()[i]);
        int size=rTable->getAllCol().size();
        int loc=rTable->findCol({rKey.toStdString()})[0];
        vector<int> tmpVec=rTable->doFind(constructQuery(size,loc,tmpCopy));
        if(tmpVec.empty()==true){
            lIndex.insert(lIndex.end(),i);
            rIndex.insert(rIndex.end(),-1);
        }
        else{
            lIndex.insert(lIndex.end(),tmpVec.size(),i);
            rIndex.insert(rIndex.end(),tmpVec.begin(),tmpVec.end());
        }
    }
    return make_tuple(lIndex,rIndex);
}

static tuple<vector<int>,vector<int>> outer(table* lTable,table* rTable,const QString& lKey,const QString& rKey){
    col* lCol=lTable->getCol(lKey);
    col* rCol=rTable->getCol(rKey);
    if(lCol->getType()!=rCol->getType()){
        throw string("Wrong data type");
    }
    vector<int> lIndex;
    vector<int> rIndex;
    for(int i=0;i<lCol->getAllData().size();++i){
        Basic* tmpCopy=typeHelper::typehelper->copy(lCol->getAllData()[i]);
        int size=rTable->getAllCol().size();
        int loc=rTable->findCol({rKey.toStdString()})[0];
        vector<int> tmpVec=rTable->doFind(constructQuery(size,loc,tmpCopy));
        if(tmpVec.empty()==true){
            lIndex.insert(lIndex.end(),i);
            rIndex.insert(rIndex.end(),-1);
        }
        else{
            lIndex.insert(lIndex.end(),tmpVec.size(),i);
            rIndex.insert(rIndex.end(),tmpVec.begin(),tmpVec.end());
        }
    }
    for(int i=0;i<rCol->getAllData().size();++i){
        Basic* tmpCopy=typeHelper::typehelper->copy(rCol->getAllData()[i]);
        int size=lTable->getAllCol().size();
        int loc=lTable->findCol({lKey.toStdString()})[0];
        vector<int> tmpVec=lTable->doFind(constructQuery(size,loc,tmpCopy));
        if(tmpVec.empty()==true){
            rIndex.insert(rIndex.end(),i);
            lIndex.insert(lIndex.end(),-1);
        }
    }
    return make_tuple(lIndex,rIndex);
}


class tableManager: public QObject
{
private:
    Q_OBJECT

    const int maxManaged;

    vector<string> managedTableName;

    map<string,table*> managedTable;

    tableManager(int maxNum):maxManaged(maxNum) {}

    static string curOperatUser;

    static table* jurisdictionTable;

public:

    static tableManager* tablemanager;

    static tableManager* getTableManager(int maxNum=10){
        if(tablemanager==nullptr){
            tablemanager=new tableManager(maxNum);
        }
        return tablemanager;
    }

    static void setcurOperatUser(const string& user){
        curOperatUser=user;
    }

    static string getcurOperatUser(){
        return curOperatUser;
    }

    static void setJurisdiction(table* table)
    {
        table->setSystemManage();
        jurisdictionTable=table;
    }

    Q_INVOKABLE table* loadTable(QString tName)
    {
        string tableName=tName.toStdString();/*
        if(jurisdictionTable->find({"(x=='"+curOperatUser+"')","(x=='"+tableName+"')"}).empty()) {
            return nullptr;
        }*/
        if(managedTable.find(tableName)==managedTable.end())
        {
            table* t=table::loadFile(tableName);
            t->setSystemManage();
            managedTable[tableName] = t;
        }
        else
        {
            auto iter=find(managedTableName.begin(),managedTableName.end(),tableName);
            managedTableName.erase(iter);
        }
        managedTableName.insert(managedTableName.begin(),tableName);
        return managedTable[tableName];
    }

    void doManage()
    {
        for(auto iter=managedTable.begin();iter!=managedTable.end();iter++){
            iter->second->updateFile();
        }
        while(managedTableName.size()>this->maxManaged){
            string delTableName=managedTableName.back();
            managedTableName.pop_back();
            delete managedTable[delTableName];
            managedTable.erase(delTableName);
        }
    }

    void saveAllTable(){
        for(auto iter=managedTable.begin();iter!=managedTable.end();iter++){
            iter->second->saveFile();
        }
    }

    Q_INVOKABLE table* createTable(const QString& ID,const QScriptValue& strVec);

    table* tableJoin(const QString &newTableID, const QString &lTableName,const vector<string>& lColName,const QString &rTableName,const vector<string>& rColName,const QString &lKey, const QString &rKey, const QString &joinWay="innerjoin"){
        table* lTable=tableManager::tablemanager->loadTable(lTableName);
        table* rTable=tableManager::tablemanager->loadTable(rTableName);
        tuple<vector<int>,vector<int>> resultIndex;
        if(joinWay=="innerjoin"){
            resultIndex=inner(lTable,rTable,lKey,rKey);
        }
        else if(joinWay=="leftjoin"){
            resultIndex=devition(lTable,rTable,lKey,rKey);
        }
        else if(joinWay=="rightjoin"){
            resultIndex=devition(rTable,lTable,lKey,rKey);
        }
        else if(joinWay=="outerjoin"){
            resultIndex=outer(lTable,rTable,lKey,rKey);
        }
        else{
            throw string("The wrong way to join");
        }
        table* lNewTable=lTable->genNewTable(lColName,get<0>(resultIndex));
        table* rNewTable=rTable->genNewTable(rColName,get<1>(resultIndex));
        table* result=table::doJoin(newTableID.toStdString(),lNewTable,rNewTable);
        lNewTable->abandonOwnShip();
        rNewTable->abandonOwnShip();
        delete lNewTable;
        delete rNewTable;
        return result;
    }

    Q_INVOKABLE table* tableJoin(const QString &newTableID, const QString &lTableName,const QScriptValue& lColName,const QString &rTableName,const QScriptValue& rColName,const QString &lKey, const QString &rKey, const QString &joinWay="innerjoin");
};
