#pragma once
#include "basicType.h"

class jsCollection;

class jsCollecManage
{
private:
    vector<jsCollection*> jsCollecs;
    jsCollecManage(){}

public:
    static jsCollecManage* delCollec;

    static jsCollecManage* getjsCollecManage()
    {
        if(delCollec==nullptr){
            delCollec=new jsCollecManage ();
        }
        return delCollec;
    }

    void addManage(jsCollection* tmp)
    {
        delCollec->jsCollecs.push_back(tmp);
    }

    void domanage()
    {
        for(int i=0;i<delCollec->jsCollecs.size();++i){
            delete delCollec->jsCollecs[i];
        }
        delCollec->jsCollecs.clear();
    }

    ~jsCollecManage();
};

class jsCollection : public QObject
{
    Q_OBJECT
    vector<int> intVec;
    vector<Basic*> basicVec;

public:
    jsCollection(const vector<int>& intVec){
        this->intVec=intVec;
        jsCollecManage::delCollec->addManage(this);
    }

    jsCollection(vector<Basic*> basicVec){
        this->basicVec=basicVec;
        jsCollecManage::delCollec->addManage(this);
    }

    vector<int> getintVec(){
        return intVec;
    }

    vector<Basic*> getbasicVec(){
        return basicVec;
    }

    ~jsCollection(){
        intVec.clear();
        for(Basic* b:basicVec){
            b=nullptr;
        }
    }

};
