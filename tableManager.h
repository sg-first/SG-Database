#pragma once
#include <string>
#include <map>
#include "table.hpp"
using namespace std;

class tableManager: public QObject
{
private:
    Q_OBJECT;

    const int maxManaged;

    vector<string> managedTableName;

    map<string,shared_ptr<operatTable>> managedTable;

    tableManager(int maxNum=10):maxManaged(maxNum) {}

    static string curOperatUser;

    static shared_ptr<operatTable> jurisdictionTable;

public:

    static shared_ptr<tableManager> tablemanager;

    static shared_ptr<tableManager> getTableManager(){
        if(tablemanager==nullptr){
            tablemanager=shared_ptr<tableManager>(new tableManager());
        }
        return tablemanager;
    }

    static void setcurOperatUser(const string& user){
        curOperatUser=user;
    }

    static void setJurisdiction(shared_ptr<operatTable> table){
        jurisdictionTable=table;
    }

    Q_INVOKABLE shared_ptr<operatTable> loadTable(const string& tableName)
    {
        if(jurisdictionTable->find({"(x=='"+curOperatUser+"')","(x=='"+tableName+"')"}).empty()){
            return nullptr;
        }
        if(managedTable.find(tableName)==managedTable.end()){
            managedTable[tableName] = operatTable::loadFile(tableName);
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
            managedTable.erase(delTableName);
        }
    }
};
