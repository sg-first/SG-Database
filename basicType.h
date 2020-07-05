#pragma once
#include <string>
using namespace std;

//enum nodeType{Float,Int,String,Bool,Arr};

class Basic
{
public:
    virtual string getType() { return "Null"; } //直接创建的basic对象视为null
    virtual ~Basic() {}
    virtual string toStr() { return "Null"; }
};

class Placeholder : public Basic
{
public:
    virtual string getType() override { return "Placeholder"; }
    virtual string toStr() override { return "Placeholder"; }
};

class Int : public Basic
{
public:
    int val;
    virtual string getType() override {return "Int";}
    Int(int val) : val(val) {}
    virtual string toStr() override { return to_string(val); }
};

class Float : public Basic
{
public:
    float val;
    virtual string getType() override {return "Float";}
    Float(float val) : val(val) {}
    virtual string toStr() override { return to_string(val); }
};

class Str : public Basic
{
public:
    string val;
    virtual string getType() override {return "Str";}
    Str(string val) : val(val) {}
    virtual string toStr() override { return "\""+val+"\""; }
};

class Bool : public Basic
{
public:
    bool val;
    virtual string getType() override {return "Bool";}
    Bool(bool val) : val(val) {}
    virtual string toStr() override
    {
        if(val)
            return "true";
        else
            return "false";
    }
};
