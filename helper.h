#pragma once
#include "basicType.h"

class helper
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
};
