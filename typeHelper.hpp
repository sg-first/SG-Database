#pragma once
#include "basicType.h"
#include <vector>
#include <set>
#include <algorithm>

class typeHelper:public QObject
{
    Q_OBJECT

    typeHelper(){}
public:

    static typeHelper* typehelper;

    static typeHelper* getTypeHelper(){
        if(typehelper==nullptr){
            typehelper=new typeHelper () ;
        }
        return typehelper;
    }

    Q_INVOKABLE Basic* copy(Basic* v)
    {
        //调用前应该对参数类型进行检查
        if(v->getType()==INT)
            return new Int(((Int*)(v))->val);
        if(v->getType()==FLOAT)
            return new Float(((Float*)(v))->val);
        if(v->getType()==BOOL)
            return new Bool(((Bool*)(v))->val);
        if(v->getType()==STR)
            return new Str(((Str*)(v))->val);
        if(v->getType()==_NULL)
            return new Basic();
        if(v->getType()==PLACEHOLDER)
            return new Placeholder();
        return nullptr; //如果进行参数检查了不会走到这一步
    }

    #define CONVCMP(strType,type) if(v1->getType()==strType) { \
        auto nv1=((type*)(v1));\
        auto nv2=((type*)(v2));\
        return nv1->val==nv2->val;\
    }

    Q_INVOKABLE bool isEqu(Basic* v1, Basic* v2)
    {
        if(v1->getType()!=v2->getType())
            return false;
        {
            CONVCMP(INT,Int)
            CONVCMP(FLOAT,Float)
            CONVCMP(BOOL,Bool)
            CONVCMP(STR,Str)
            return false;
        }
    }

    TYPE judgeType(string& val){
        if(val=="placeholder"){
            return PLACEHOLDER;
        }
        else if(val=="true"||val=="false"){
            return BOOL;
        }
        else if(val[0]=='\''){
            val=val.substr(1,val.size()-2);
            return STR;
        }
        for(const char& c:val){
            if(c=='.'){
                return FLOAT;
            }
        }
        return INT;
    }

    Basic* strToBasic(string val){
        TYPE tp=judgeType(val);
        return strToBasic(val,tp);
    }

    Q_INVOKABLE Basic* strToBasic(const QString& val);

    Basic* strToBasic(const string& val, TYPE type) //val需要与Basic.toStr结果对应
    {
        if(type==_NULL)
            return new Basic();
        if(type==PLACEHOLDER)
            return new Placeholder();
        if(type==INT)
            return new Int(stoi(val));
        if(type==FLOAT)
            return new Float(stof(val));
        if(type==STR)
            return new Str(val);
        if(type==BOOL)
        {
            if(val=="true")
                return new Bool(true);
            else
                return new Bool(false);
        }
        return nullptr;
    }
};

class helper
{
public:
    template<class T>
    static int find(const vector<T> &invec, int value)
    {
      int low = 0, high = invec.size()-1;
      //assert(!invec.empty() && pos>=0);
      while(low <=high)
      {
          int mid = (low+high)/2;
          if(invec[mid] == value)
            return mid;
          else if(invec[mid] < value)
            low = mid+1;
          else
            high = mid-1;
      }
      return -1;
    }

    /*static set<int> toSet(const vector<int> &vec)
    {
        return set<int>(vec.begin(), vec.end());
    }

    static vector<int> toVec(const set<int> &s)
    {
        vector<int> vec;
        vec.assign(s.begin(), s.end());
        return vec;
    }*/

    static void sort(vector<int> &vec)
    {
        std::sort(vec.begin(),vec.end());
    }

    static vector<int> getRange(int start,int end)
    {
        vector<int> result;
        for(int i=start;i<=end;i++)
            result.push_back(i);
        return result;
    }
};
