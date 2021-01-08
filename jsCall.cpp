#include "table.hpp"
#include "typeHelper.hpp"
#include "aggHelper.h"
using namespace std;
TYPE strToType(const string& str){
    if(str=="FLOAT"){
        return FLOAT;
    }
    else if(str=="INT"){
        return INT;
    }
    else if(str=="STR"){
        return STR;
    }
    else if(str=="BOOL"){
        return BOOL;
    }
    else if(str=="NULL"){
        return _NULL;
    }
    return PLACEHOLDER;
}

vector<string> strToVec(const string& str){
    vector<string> result;
    int firstIndex=1;
    for(int i=0;i<str.length();i++){
        if(str[i]==','&&(i-1)>=0&&(i+1)<str.length()){
            if(str[i-1]=='\''&&str[i+1]=='\''){
                result.push_back(str.substr(firstIndex,i-firstIndex-1));
                firstIndex=i+2;
            }
        }
    }
    result.push_back(str.substr(firstIndex,str.length()-firstIndex-1));
    return result;
}

vector<Basic*> strToBasicVec(const string& str){
    const vector<string>& tmp=strToVec(str);
    vector<Basic*> basicVec;
    for(int i=0;i<tmp.size();++i){
        basicVec.push_back(typeHelper::typehelper->strToBasic(tmp[i]));
    }
    return basicVec;
}

table::table(const QString& ID,vector<col*>allCol, bool hasOwnership){
    table(ID.toStdString(),allCol,hasOwnership);
}

table* table::genNewTable(const QString& colNames, jsCollection *tupSubList){
    return this->genNewTable(strToVec(colNames.toStdString()),tupSubList->getintVec());
}

void table::add(const QString& tuple){
    vector<Basic*> basicVec=strToBasicVec(tuple.toStdString());
    this->add(basicVec);
}

void table::mod(int opSub, const QString& tuple){
    vector<Basic*> basicVec=strToBasicVec(tuple.toStdString());
    this->mod(opSub,basicVec);
}

void table::del(jsCollection *allOpSub){
    this->del(allOpSub->getintVec());
}

jsCollection* table::find_in(const QString& colName, jsCollection *target_vec){
    const vector<int>& tmp=this->find_in(colName.toStdString(),target_vec->getbasicVec());
    return new jsCollection (tmp);
}

jsCollection* table::find_not_in(const QString& colName, jsCollection *target_vec){
    const vector<int>& tmp=this->find_not_in(colName.toStdString(),target_vec->getbasicVec());
    return new jsCollection (tmp);
}

jsCollection* table::find(const QString& allExp){
    return new jsCollection(find(strToVec(allExp.toStdString())));
}

col* table::getCol(const QString &colName){
    return this->getCol(colName.toStdString());
}

int table::getColIndex(const QString &colName){
    return this->getColIndex(colName.toStdString());
}

col::col(const QString& type,const QString& ID):type(strToType(type.toStdString())),ID(ID.toStdString()){}

jsCollection* col::getData(jsCollection *filtered_index){
    return new jsCollection(this->getData(filtered_index->getintVec()));
}

col* col::genNewCol(jsCollection *subList){
    return this->genNewCol(subList->getintVec());
}

const jsCollection* col::getallData(){
    return new jsCollection (this->allData);
}

Basic* typeHelper::strToBasic(const QString &val){
    return this->strToBasic(val.toStdString());
}

jsCollection* aggHelper::distinct(jsCollection *data_vec){
    return new jsCollection (this->distinct(data_vec->getbasicVec()));
}

Basic* aggHelper::avg(jsCollection* data_vec){
    return this->avg(data_vec->getbasicVec());
}

Basic* aggHelper::count(jsCollection *data_vec){
    return this->count(data_vec->getbasicVec());
}

Basic* aggHelper::first(jsCollection *data_vec){
    return this->first(data_vec->getbasicVec());
}

Basic* aggHelper::last(jsCollection *data_vec){
    return this->last(data_vec->getbasicVec());
}

Basic* aggHelper::max(jsCollection* data_vec){
    return this->max(data_vec->getbasicVec());
}

Basic* aggHelper::min(jsCollection* data_vec){
    return this->min(data_vec->getbasicVec());
}

Basic* aggHelper::sum(jsCollection *data_vec){
    return this->sum(data_vec->getbasicVec());
}





