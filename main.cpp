#include <QCoreApplication>
#include <iostream>
#include <string>
#include "table.hpp"
#include "basicType.h"
#include "aggHelper.h"
//#include "expParse.h"
using namespace std;
string operatTable::default_path;
aggHelper* aggHelper::helper;
void outputVec(const vector<int> &vec)
{
    for(int i : vec)
        cout<<i<<" ";
    cout<<endl;
}
int main(int argc, char *argv[])
{
    long start_time=time(0);
    QCoreApplication a(argc, argv);
    operatTable::default_path="D:\\personal_file\\download_files\\test_";
    col* ID=new col(TYPE(1),"ID");
    for(int i=0;i<10;i++){
        ID->pushData(typeHelper::strToBasic(to_string(i),INT));
    }
    col* score=new col(TYPE(0),"SCORE");
    for(float i=0;i<1;i=i+float(0.1)){
        score->pushData(typeHelper::strToBasic(to_string(i),FLOAT));
    }
    col* score_2=new col(TYPE(0),"SCORE");
    for(float i=1;i<2;i=i+float(0.1)){
        score_2->pushData(typeHelper::strToBasic(to_string(i),FLOAT));
    }
    operatTable* try_table = new operatTable("student table",{ID,score,score_2});
    //try_table=table::loadFile("F:\\predict_2.1_1.csv");



    ruleExp* r=new ruleExp(EQU,new Int(7));
    auto result=try_table->find({r,nullptr,nullptr});
    outputVec(result);
    ruleExp* r2=new logExp(OR,new ruleExp(EQU,new Int(3)),new ruleExp(EQU,new Int(4)));
    ruleExp* r3=new logExp(OR,r2,new ruleExp (EQU,new Int(5)));



    result=try_table->find({r3,nullptr,nullptr});
    result=try_table->find({"((((x<=3)||(x==4))||(x>=7))||(x==5))","",""});
    outputVec(result);
    result=try_table->find({new logExp(NOT,nullptr ,new ruleExp(EQU,new Int(7))),nullptr,nullptr});
    result=try_table->find({"(x!=7)","",""});
    result=try_table->find({"!((((x<=3)||(x==4))||(x>=7))||(x==5))","",""});
    outputVec(result);

    result=try_table->find_in("ID",{new Int(7),new Int(1),new Int(3),new Int(5),new Int(4)});
    outputVec(result);

    result=try_table->find_not_in("ID",{new Int(7),new Int(1),new Int(3),new Int(5),new Int(4)});
    outputVec(result);

    //表抽取
    auto t2=try_table->genNewTable({0,2},result);
    cout<<t2->toStr()<<endl;
    cout<<t2->getCol("ID")->toStr()<<endl;

    try_table->saveFile();

    try_table=operatTable::loadFile("student table");

    //换索引
    //index* ni=new binarySearchIndex(ID);
    //try_table->changeIndex(0,ni);

    try_table->updateFile();
    try_table->add({typeHelper::strToBasic("789",INT),typeHelper::strToBasic("99.9",FLOAT),typeHelper::strToBasic("88.8",FLOAT)});
    try_table->add({typeHelper::strToBasic("789",INT),typeHelper::strToBasic("99.9",FLOAT),typeHelper::strToBasic("88.8",FLOAT)});
    result=try_table->find({new ruleExp(EQU,new Int(789)),nullptr,nullptr});
    cout<<try_table->genNewTable({0,1,2},result)->toStr();
    try_table->updateFile();

    try_table->mod(0,{typeHelper::strToBasic("789",INT),typeHelper::strToBasic("99.9",FLOAT),typeHelper::strToBasic("88.8",FLOAT)});

    //result=try_table->find({nullptr,nullptr,nullptr});
    vector<Basic*> s_result=try_table->getCol("ID")->getDate(result);


    aggHelper* helper=aggHelper::getHelper();
    Basic* test=helper->sum(s_result);

    try_table->saveFile();
    try_table->del(0);
    try_table->del(4);
    try_table->del(4);
    try_table->del(5);
    try_table->del(5);
    try_table->updateFile();

    result=try_table->find({new ruleExp(EQU,new Int(789)),nullptr,nullptr});
    cout<<try_table->genNewTable({0,1,2},result)->toStr();

    try_table=operatTable::loadFile("student table");


    result=try_table->find({new ruleExp(EQU,new Int(789)),nullptr,nullptr});
    cout<<try_table->genNewTable({0,1,2},result)->toStr();
    try_table->saveFile();
    delete r;
    delete r2;
    delete r3;
    cout<<"Total running time:"<<time(0)-start_time;
    return a.exec();
}
