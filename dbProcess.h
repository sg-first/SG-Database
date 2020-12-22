#pragma once
#include <iostream>
#include <string>
#include <queue>
//#include <QtScript>
#include <QVariant>
#include <table.hpp>
#include "js.h"
using namespace std;
class processObject{

public:
    string user;
    string passWord;
    string JS;
    string result;
    processObject(const string& user,const string& JS){
        this->user=user;
        this->JS=JS;
    }
    processObject(string user,string passWord,string JS){
        this->user=user;
        this->JS=JS;
        this->passWord=passWord;
    }

    processObject(){}

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



public:
    static queue<processObject> processQueue;

    static queue<processObject> correspondQueue;

    static shared_ptr<operatTable> countTable;

    static shared_ptr<operatTable> jurisdictionTable;
     static string GetString(string JS, int tag)
    {
        int index = tag;
        int tag2;
        string str = "";
        for (;; index++)
        {
            if (JS[index] == ')')
            {
                tag2 = index;
                break;
            }
        }
        str.append(JS, tag + 2, tag2-3-tag );
        return str;
    }

     static bool Judge(string s, int index)
    {
        string tmp = "";
        string comp = "loadTable";
        tmp.append(s, index - 9, 9);

        if (tmp.compare(comp) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static vector<string> getUsedTable(string JS){
        set<string> waitput;
        vector<string> sv;
        int tag=0;
        for (int i=0;i<JS.length();i++)
        {
            if (JS[i] == '(')
            {
                tag = i;
                if (Judge(JS, tag))
                {
                    string data=GetString(JS, tag);
                    waitput.insert(data);
                }
            }
        }
        for (string s : waitput)
        {
            sv.push_back(s);
        }
        return sv;

    }

    static void setCount(shared_ptr<operatTable> table){
        countTable=table;
    }

    static void setJurisdiction(shared_ptr<operatTable> table){
        jurisdictionTable=table;
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
