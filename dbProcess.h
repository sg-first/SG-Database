#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <QtScript>
#include <QVariant>
#include <table.hpp>
#include "js.h"
using namespace std;
class processObject{
    string user;
    string passWord;
    string JS;
    string result;
public:
    processObject(const string& user,const string& JS){
        this->user=user;
        this->JS=JS;
    }

    string getUser()const{
        return user;
    }

    string getPassWord()const{
        return passWord;
    }

    string getJS()const{
        return JS;
    }

    void setResult(const string& result){
        this->result=result;
    }

    string getResult()const{
        return result;
    }
};

class dbProcess{

    static queue<processObject> processQueue;

    static queue<processObject> correspondQueue;

    static operatTable* countTable;

    static operatTable* jurisdictionTable;

public:
    static vector<string> getUsedTable(string JS){

    }

    static bool checkCount(const processObject& obj){
        string userName=obj.getUser();
        string passWord=obj.getPassWord();
        vector<int>countVec=countTable->find({"(x=="+userName+")","(x=="+passWord+")"});
        if(countVec.empty()){
            return false;
        }
        return true;
    }

    static bool checkJurisdiction(const processObject& obj){
        string userName=obj.getUser();
        string JS=obj.getJS();
        vector<string> tableVec=getUsedTable(JS);
        for(const string& tbName:tableVec){
            if(jurisdictionTable->find({"(x=="+userName+")","(x=="+tbName+")"}).empty()){
                return false;
            }
        }
        return true;
    }

    static void processRequst(){
        if(processQueue.empty()){
            return;
        }
        processObject tmpProcess=processQueue.front();
        processQueue.pop();
        if(checkCount(tmpProcess)==false){
            tmpProcess.setResult("Account password error");
        }
        else if(checkJurisdiction(tmpProcess)==false){
            tmpProcess.setResult("No access");
        }
        else{
            QString* mistake=nullptr;
            QVariant varResult=JSEval(QString::fromStdString(tmpProcess.getJS()),"",mistake,AddJSVM());
            if(*mistake!=nullptr){
                tmpProcess.setResult(mistake->toStdString());
            }
            else{
                tmpProcess.setResult(varResult.toString().toStdString());
            }
        }
        correspondQueue.push(tmpProcess);
    }
};
