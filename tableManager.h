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
    map<string,operatTable*> managedTable;

public:
    tableManager(int maxNum):maxManaged(maxNum) {}

    operatTable* loadTable(const string& tableName)
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
            operatTable* delTable=managedTable[delTableName];
            managedTable.erase(delTableName);
            delete delTable;
        }
    }
};
