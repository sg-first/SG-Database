#include "tableManager.h"
#include "typeHelper.hpp"
#include "aggHelper.h"
#include <QtScript/QScriptEngine>
Q_DECLARE_METATYPE(QVector<QString>)
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

vector<string> jsvalueTostrVec(const QScriptValue& jsvalue){
    vector<string> StrVec;
    const QVector<QString>& strVec=qscriptvalue_cast<QVector<QString>>(jsvalue);
    for(const QString& str:strVec){
        StrVec.push_back(str.toStdString());
    }
    return StrVec;
}

vector<col*> strVecToColVec(const vector<string>& colStrVec){
    vector<col*> colVec;
    for(const string& str:colStrVec){
        for(int i=0;i<str.length();++i){
            if(str[i]==':'){
                colVec.push_back(new col (str.substr(i+1),str.substr(0,i)));
                break;
            }
        }
    }
    return colVec;
}

vector<Basic*> jsvalueToBasicVec(const QScriptValue& str){
    const vector<string>& tmp=jsvalueTostrVec(str);
    vector<Basic*> basicVec;
    for(int i=0;i<tmp.size();++i){
        basicVec.push_back(typeHelper::typehelper->strToBasic(QString::fromStdString(tmp[i])));
    }
    return basicVec;
}

table* tableManager::tableJoin(const QString &newTableID, const QString &lTableName, const QScriptValue &lColName, const QString &rTableName, const QScriptValue &rColName, const QString &lKey, const QString &rKey, const QString &joinWay){
    return tableJoin(newTableID,lTableName,jsvalueTostrVec(lColName),rTableName,jsvalueTostrVec(rColName),lKey,rKey,joinWay);
}


table* table::genNewTable(const QScriptValue& colNames, jsCollection *tupSubList){
    return this->genNewTable(jsvalueTostrVec(colNames),tupSubList->getintVec());
}

void table::add(const QScriptValue& tuple){
    vector<Basic*> basicVec=jsvalueToBasicVec(tuple);
    this->add(basicVec);
}

void table::mod(int opSub, const QScriptValue& tuple){
    vector<Basic*> basicVec=jsvalueToBasicVec(tuple);
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

jsCollection* table::find(const QScriptValue& allExp){
    vector<string> StrVec=jsvalueTostrVec(allExp);
    return new jsCollection(find(StrVec));
}

col* table::getCol(const QString &colName){
    return getCol(this->findCol({colName.toStdString()})[0]);
}

int table::getColIndex(const QString &colName){
    return this->getColIndex(colName.toStdString());
}

table* tableManager::createTable(const QString& ID,const QScriptValue& strVec){
    vector<string> colStrVec=jsvalueTostrVec(strVec);
    vector<col*> allCols=strVecToColVec(colStrVec);
    return new table(ID.toStdString(),allCols);
}

col::col(const string& type,const string& ID):type(strToType(type)),ID(ID){}

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
    string tmp=val.toStdString();
    TYPE tp=judgeType(tmp);
    return strToBasic(tmp,tp);
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





