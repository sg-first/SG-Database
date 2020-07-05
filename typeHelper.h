#pragma once
#include "basicType.h"
#include <vector>

class typeHelper
{
public:
    static Basic* copy(Basic* v)
    {
        //调用前应该对参数类型进行检查
        if(v->getType()=="Int")
            return new Int(((Int*)v)->val);
        if(v->getType()=="Float")
            return new Float(((Float*)v)->val);
        if(v->getType()=="Bool")
            return new Bool(((Bool*)v)->val);
        if(v->getType()=="Str")
            return new Str(((Str*)v)->val);
        if(v->getType()=="Null")
            return new Basic();
        if(v->getType()=="Placeholder")
            return new Placeholder();
        return nullptr; //如果进行参数检查了不会走到这一步
    }

    #define CONVCMP(strType,type) if(v1->getType()==strType) { \
        auto nv1=(type*)v1;\
        auto nv2=(type*)v2;\
        return nv1->val==nv2->val;\
    }

    static bool isEqu(Basic* v1, Basic *v2)
    {
        if(v1->getType()!=v2->getType())
            return false;
        {
            CONVCMP("Int",Int)
            CONVCMP("Float",Float)
            CONVCMP("Bool",Bool)
            CONVCMP("Str",Str)
            return true;
        }
    }

    static Basic* strToBasic(string val, string type) //val需要与Basic.toStr结果对应
    {
        if(type=="Null")
            return new Basic();
        if(type=="Placeholder")
            return new Placeholder();
        if(type=="Int")
            return new Int(stoi(val));
        if(type=="Float")
            return new Float(stof(val));
        if(type=="Str")
            return new Str(val);
        if(type=="Bool")
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
    static int find(vector<int> &invec, int value)
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
};
