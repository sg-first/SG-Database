#pragma once
#include "basicType.h"
#include "typeHelper.h"

enum ruleOp{EQU,GRAT,SMAL,AND,OR,NOT};

class expBase
{
protected:
    ruleOp op;
    Basic* operand2B=nullptr;
    expBase* operand2E=nullptr; //可以给两个布尔值比较

    bool operandIsBasic() { return operand2B==nullptr; }

public:
    expBase(ruleOp op, Basic* operand) : op(op)
    { this->operand2B=operand; }
    expBase(ruleOp op, expBase* operand) : op(op)
    { this->operand2E=operand; }

    virtual bool eval(vector<Basic*> &allOperand1B, int nowSub=0)
    {
        if(op==EQU)
        {
            if(operandIsBasic()) //basic之间运算
                return typeHelper::isEqu(allOperand1B[nowSub],operand2B);
            else
            {
                if(allOperand1B[nowSub]->getType()==BOOL)
                {
                    Bool* boolOp1B=(Bool*)allOperand1B[nowSub];
                    bool result=operand2E->eval(allOperand1B,nowSub+1);
                    return boolOp1B->getType()==result;
                }
                else
                    throw string("type mismatch");
            }
        }
        else
            throw string("error eval case");
    }

    virtual ~expBase()
    {
        delete operand2B;
        delete operand2E;
    }
};

class numExp : public expBase
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
    virtual bool eval(vector<Basic*> &allOperand1B, int nowSub=0)
    {
        if(op==GRAT || op==SMAL)
        {
            if(operandIsBasic())
            {
                if(isNumType(operand2B) && isNumType(allOperand1B[nowSub]))
                {
                    float val2=getVal(operand2B);
                    float val1=getVal(allOperand1B[nowSub]);
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

class logExp : public expBase
{
public:
    virtual bool eval(vector<Basic*> &allOperand1B, int nowSub=0)
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
                bool result=operand2E->eval(allOperand1B,nowSub+1);
                return !result;
            }
        }
        else if(op==AND || op==OR)
        {
            if(operandIsBasic())
            {
                if(operand2B->getType()==BOOL && allOperand1B[nowSub]->getType()==BOOL)
                {
                    Bool* boolOp1B=(Bool*)allOperand1B[nowSub];
                    Bool* boolOp2B=(Bool*)operand2B;
                    if(op==AND)
                        return boolOp1B->val && boolOp2B->val;
                    else
                        return boolOp1B->val || boolOp2B->val;
                }
                else
                    throw string("type mismatch");
            }
            else
            {
                if(allOperand1B[nowSub]->getType()==BOOL)
                {
                    Bool* boolOp1B=(Bool*)allOperand1B[nowSub];
                    bool result=operand2E->eval(allOperand1B,nowSub+1);
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
