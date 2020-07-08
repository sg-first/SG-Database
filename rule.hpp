#pragma once
#include "basicType.h"
#include "typeHelper.hpp"

enum ruleOp{EQU,GRAT,SMAL,AND,OR,NOT,UNRUAL};

class ruleExp
{
protected:
    ruleOp op;
    Basic* operand2B=nullptr;
    ruleExp* operand2E=nullptr; //可以给两个布尔值比较
    int nestingLevel=0;

    bool operandIsBasic() { return operand2B!=nullptr; }

public:
    ruleExp(ruleOp op, Basic* operand) : op(op), operand2B(operand) {}
    ruleExp(ruleOp op, ruleExp* operand) : op(op), operand2E(operand), nestingLevel(operand->nestingLevel+1) {}
    ruleExp() : op(UNRUAL) {}

    void resetOperand(Basic* operand)
    {
        this->operand2B=operand;
        delete this->operand2E;
        this->operand2E=nullptr;
    }
    //一般认为规则构造之后整体结构就不变了，如果把原先的reluExp操作符改成basic相当于舍弃了一些层级，是在改变结构

    ruleOp getOp() { return op; }
    int getNestingLevel() { return nestingLevel; }
    Basic* getOperand2B() { return this->operand2B; }
    ruleExp* getOperand2E() { return this->operand2E; }

    virtual bool eval(Basic* operand1B)
    {
        if(op==EQU)
        {
            if(operandIsBasic()) //basic之间运算
                return typeHelper::isEqu(operand1B,operand2B);
            else
            {
                if(operand1B->getType()==BOOL)
                {
                    Bool* boolOp1B=(Bool*)operand1B;
                    bool result=operand2E->eval(operand1B);
                    return boolOp1B->val==result;
                }
                else
                    throw string("type mismatch");
            }
        }
        else
            throw string("error eval case");
    }

    virtual ~ruleExp()
    {
        delete operand2B; //持有所有权
        delete operand2E;
    }
};


class numExp : public ruleExp
{
private:
    static bool isNumType(Basic* v)
    {
        return v->getType()==INT || v->getType()==FLOAT;
    }

    static float getVal(Basic* v)
    {
        if(v->getType()==INT)
            return ((Int*)v)->val;
        else if(v->getType()==FLOAT)
            return ((Float*)v)->val;
        else
            throw string("type mismatch");
    }

public:
    virtual bool eval(Basic* operand1B)
    {
        if(op==GRAT || op==SMAL)
        {
            if(operandIsBasic())
            {
                if(isNumType(operand2B) && isNumType(operand1B))
                {
                    float val2=getVal(operand2B);
                    float val1=getVal(operand1B);
                    if(op==GRAT)
                        return val1>val2;
                    else
                        return val1<val2;
                }
                else
                    throw string("type mismatch");
            }
            //数值比较必须两个都是basic，因为expBase必然是bool
        }
        throw string("error eval case");
    }
};


class logExp : public ruleExp
{
protected:
    ruleExp* operand1E=nullptr;
    bool isDoubleExp() { return operand2E!=nullptr && operand1E!=nullptr; }

public:
    logExp(ruleOp _op, Basic* _operand) : ruleExp(_op,_operand) {}
    logExp(ruleOp _op, ruleExp* o1e, ruleExp* o2e) : ruleExp(_op,o2e), operand1E(o1e) {}

    virtual bool eval(Basic* operand1B)
    {
        if(op==NOT)
        {
            if(operandIsBasic())
            {
                if(operand2B->getType()==BOOL)
                {
                    Bool* boolOp2B=(Bool*)operand2B;
                    return !boolOp2B->val;
                }
                else
                    throw string("type mismatch");
            }
            else
            {
                bool result=operand2E->eval(operand1B);
                return !result;
            }
        }
        else if(op==AND || op==OR)
        {
            if(operandIsBasic())
            {
                if(operand2B->getType()==BOOL && operand1B->getType()==BOOL)
                {
                    Bool* boolOp1B=(Bool*)operand1B;
                    Bool* boolOp2B=(Bool*)operand2B;
                    if(op==AND)
                        return boolOp1B->val && boolOp2B->val;
                    else
                        return boolOp1B->val || boolOp2B->val;
                }
                else
                    throw string("type mismatch");
            }
            else if(isDoubleExp())
            {
                bool r1=operand1E->eval(operand1B);
                bool r2=operand2E->eval(operand1B);
                if(op==AND)
                    return r1 && r2;
                else
                    return r1 || r2;
            }
            else
            {
                if(operand1B->getType()==BOOL)
                {
                    Bool* boolOp1B=(Bool*)operand1B;
                    bool result=operand2E->eval(operand1B);
                    if(op==AND)
                        return boolOp1B->val && result;
                    else
                        return boolOp1B->val || result;
                }
                else
                    throw string("type mismatch");
            }
        }
        else
            throw string("error eval case");
    }
};
