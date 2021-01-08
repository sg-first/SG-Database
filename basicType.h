#pragma once
#include <string>
#include <memory>
#include "manageable.h"
#include <QObject>
using namespace std;

enum TYPE{FLOAT,INT,STR,BOOL,_NULL,PLACEHOLDER};

class Basic : public manageable
{
public:
    virtual TYPE getType() { return _NULL; } //直接创建的basic对象视为null
    virtual ~Basic() {}
    virtual string toStr() { return "Null"; }
};

class Placeholder : public Basic
{
public:
    virtual TYPE getType() override { return PLACEHOLDER; }
    virtual string toStr() override { return "Placeholder"; }
};

class Int : public Basic
{
public:
    int val;
    virtual TYPE getType() override {return INT;}
    Int(int val) : val(val) {}
    virtual string toStr() override { return to_string(val); }
};

class Float : public Basic
{
public:
    float val;
    virtual TYPE getType() override {return FLOAT;}
    Float(float val) : val(val) {}
    virtual string toStr() override { return to_string(val); }
};

class Str : public Basic
{
public:
    string val;
    virtual TYPE getType() override {return STR;}
    Str(string val) : val(val) {}
    virtual string toStr() override { return val; }
};

class Bool : public Basic
{
public:
    bool val;
    virtual TYPE getType() override {return BOOL;}
    Bool(bool val) : val(val) {}
    virtual string toStr() override
    {
        if(val)
            return "true";
        else
            return "false";
    }
};

class jsCollection;

class jsCollecManage{
    vector<jsCollection*> jsCollecs;
    jsCollecManage(){}
public:
    static jsCollecManage* delCollec;

    static jsCollecManage* getjsCollecManage(){
        if(delCollec==nullptr){
            delCollec=new jsCollecManage ();
        }
        return delCollec;
    }

    void addManage(jsCollection* tmp){
        delCollec->jsCollecs.push_back(tmp);
    }

    void domanage(){
        for(int i=0;i<delCollec->jsCollecs.size();++i){
            delete delCollec->jsCollecs[i];
        }
        delCollec->jsCollecs.clear();
    }

    ~jsCollecManage();
};

class jsCollection : public QObject{
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
