#pragma once
#include "basicType.h"
#include <vector>

class typeHelper
{
public:
    static Basic* copy(Basic* v)
    {
        //调用前应该对参数类型进行检查
        if(v->getType()==INT)
            return new Int(((Int*)v)->val);
        if(v->getType()==FLOAT)
            return new Float(((Float*)v)->val);
        if(v->getType()==BOOL)
            return new Bool(((Bool*)v)->val);
        if(v->getType()==STR)
            return new Str(((Str*)v)->val);
        if(v->getType()==_NULL)
            return new Basic();
        if(v->getType()==PLACEHOLDER)
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
            CONVCMP(INT,Int)
            CONVCMP(FLOAT,Float)
            CONVCMP(BOOL,Bool)
            CONVCMP(STR,Str)
            return true;
        }
    }

    static Basic* strToBasic(string val, TYPE type) //val需要与Basic.toStr结果对应
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