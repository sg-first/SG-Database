#pragma once
#include "basicType.h"
#include <vector>
#include <set>
#include<algorithm>

class typeHelper
{
public:
    static shared_ptr<Basic> copy(shared_ptr<Basic> v)
    {
        //调用前应该对参数类型进行检查
        if(v->getType()==INT)
            return shared_ptr<Basic>(new Int(dynamic_pointer_cast<Int>(v)->val));
        if(v->getType()==FLOAT)
            return shared_ptr<Basic>(new Float(dynamic_pointer_cast<Float>(v)->val));
        if(v->getType()==BOOL)
            return shared_ptr<Basic>(new Bool(dynamic_pointer_cast<Bool>(v)->val));
        if(v->getType()==STR)
            return shared_ptr<Basic>(new Str(dynamic_pointer_cast<Str>(v)->val));
        if(v->getType()==_NULL)
            return shared_ptr<Basic>(new Basic());
        if(v->getType()==PLACEHOLDER)
            return shared_ptr<Basic>(new Placeholder());
        return nullptr; //如果进行参数检查了不会走到这一步
    }

    #define CONVCMP(strType,type) if(v1->getType()==strType) { \
        auto nv1=dynamic_pointer_cast<type>(v1);\
        auto nv2=dynamic_pointer_cast<type>(v2);\
        return nv1->val==nv2->val;\
    }

    static bool isEqu(shared_ptr<Basic> v1, shared_ptr<Basic> v2)
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

    static TYPE judgeType(const string& val){
        if(val=="placeholder"){
            return PLACEHOLDER;
        }
        else if(val=="true"||val=="false"){
            return BOOL;
        }
        else if(val[0]=='\''){
            return STR;
        }
        for(const char& c:val){
            if(c=='.'){
                return FLOAT;
            }
        }
        return INT;
    }

    static shared_ptr<Basic> strToBasic(string val){
        return strToBasic(val,judgeType(val));
    }

    static shared_ptr<Basic> strToBasic(string val, TYPE type) //val需要与Basic.toStr结果对应
    {
        if(type==_NULL)
            return shared_ptr<Basic>(new Basic());
        if(type==PLACEHOLDER)
            return shared_ptr<Basic>(new Placeholder());
        if(type==INT)
            return shared_ptr<Basic>(new Int(stoi(val)));
        if(type==FLOAT)
            return shared_ptr<Basic>(new Float(stof(val)));
        if(type==STR)
            return shared_ptr<Basic>(new Str(val));
        if(type==BOOL)
        {
            if(val=="true")
                return shared_ptr<Basic>(new Bool(true));
            else
                return shared_ptr<Basic>(new Bool(false));
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
