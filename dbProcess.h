#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <QVariant>
#include <tableManager.h>
#include "js.h"
#include "aggHelper.h"
using namespace std;
class processObject{
    string user;
    string passWord;
    string JS;
    string result;
public:
    processObject(string user,string passWord,string JS){
        this->user=user;
        this->JS=JS;
        this->passWord=passWord;
    }

    processObject(){}

    void setUser(const string& username){
        this->user=username;
    }

    string getUser()const{
        return user;
    }

    void setPassword(const string& password){
        this->passWord=passWord;
    }

    string getPassWord()const{
        return passWord;
    }

    void setJS(const string& JS){
        this->JS=JS;
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
     static shared_ptr<operatTable> countTable;
public:
     static queue<processObject> processQueue;

     static queue<processObject> correspondQueue;

    static void setCount(shared_ptr<operatTable> table){
        countTable=table;
    }

    static bool checkCount(const processObject& obj){
        string userName=obj.getUser();
        string passWord=obj.getPassWord();
        vector<int> countVec=countTable->find({"(x=='"+userName+"')","(x=='"+passWord+"')"});
        if(countVec.empty()){
            return false;
        }
        return true;
    }

    static void processRequst(){
        if(processQueue.empty()){
            return;
        }
        processObject tmpProcess=processQueue.front();
        processQueue.pop();
        tableManager::setcurOperatUser(tmpProcess.getUser());
        if(checkCount(tmpProcess)==false){
            tmpProcess.setResult("Account password error");
        }
        else{
            QString* mistake=nullptr;
            QVariant varResult=JSEval(QString::fromStdString(tmpProcess.getJS()),"",mistake,AddJSVM());
            if(mistake!=nullptr){
                tmpProcess.setResult(mistake->toStdString());
            }
            else{
                tmpProcess.setResult(varResult.toString().toStdString());
            }
        }
        correspondQueue.push(tmpProcess);
    }
};
