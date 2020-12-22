#pragma once
#include <string>
#include <map>
#include "table.hpp"
using namespace std;

class tableManager
{
private:
    const int maxManaged;
    vector<string> managedTableName;
    map<string,shared_ptr<operatTable>> managedTable;

public:
    tableManager(int maxNum=10):maxManaged(maxNum) {}

    shared_ptr<operatTable> loadTable(const string& tableName)
    {
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
