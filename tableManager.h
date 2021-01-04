#pragma once
#include <string>
#include <map>
#include "table.hpp"
using namespace std;

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

    static void setJurisdiction(table* table){
        jurisdictionTable=table;
    }

    Q_INVOKABLE table* loadTable(QString tName)
    {
        string tableName=tName.toStdString();
        if(jurisdictionTable->find({"(x=='"+curOperatUser+"')","(x=='"+tableName+"')"}).empty()){
            return nullptr;
        }
        if(managedTable.find(tableName)==managedTable.end()){
            managedTable[tableName] = table::loadFile(tableName);
        }
        else{
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
};
