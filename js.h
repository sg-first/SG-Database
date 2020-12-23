//-----本文件是关于maincall类中JS操作的实现-----
//-----本文件是游戏中的一些函数调用的实现，引擎的三层封装-----
#pragma once
#include <QString>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QVariant>

typedef QScriptEngine JSVM;
typedef QString String;
typedef QVariant Variant;
#define NULL_String ""
JSVM* AddJSVM()
{
    JSVM *VM=new JSVM;
    //QScriptValue lib=VM->newQObject(thob);
    //VM->globalObject().setProperty("lib",lib);
    //QScriptValue lfvar=VM->newQObject(lfevent);
    //VM->globalObject().setProperty("lfvar",lfvar);
    return VM;
}

Variant JSEval(String code,String functionname,String *mistake,JSVM *VM)
{
    QScriptValue ret;
    if(code!=NULL_String)
    {ret=VM->evaluate(code);}
    if(functionname!=NULL_String)
    {
        ret=VM->globalObject().property(functionname);
        ret.call(QScriptValue());
    }
    if(ret.isError())
    {
        if(mistake!=nullptr)
        {*mistake=String::number(ret.property("lineNumber").toInt32())+" line:"+ret.toString();}
        return NULL;
    }
    return ret.toVariant();
}
/*
void JSSendPar(ParametersStru *Parame,String ParameName,JSVM *VM) //注意，使用指针仅因迫不得已，实际创建请不要new
{
    QScriptValue para=VM->newQObject(Parame);
    VM->globalObject().setProperty(ParameName,para);
}

void JSSendJSPar(JSParStru Parame,JSVM *VM)
{
    for(int i=0;i<Parame.pointerVec.length();i++)
    {
        QScriptValue para=VM->newQObject(Parame.pointerVec[i]);
        VM->globalObject().setProperty(Parame.nameVec[i],para);
    }
}

Variant JSEvalFile(String path,String functionname,String *mistake,JSVM *VM)
{return JSEval(ReadTXT(path),functionname,mistake,VM);}
*/

Variant JSCallFun(String functionname,String *mistake,JSVM *VM)
{return JSEval(NULL_String,functionname,mistake,VM);}
