#include <QCoreApplication>
#include "table.hpp"
#include <iostream>
#include <string>
#include "basicType.h"
using namespace std;
string operatTable::default_path;
void outputVec(const vector<int> &vec)
{
    for(int i : vec)
        cout<<i<<" ";
    cout<<endl;
}
int main(int argc, char *argv[])
{
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
    ruleExp* r2=new logExp(OR,new ruleExp(EQU,new Int(3)),new numExp(GRAT,new Int(3)));
    ruleExp* r3=new numExp(GRAT,new Float(0.6));
    result=try_table->find({r2,r3,nullptr});
    outputVec(result);

    //表抽取
    auto t2=try_table->genNewTable({0,2},result);
    cout<<t2->toStr()<<endl;

    try_table->saveFile();

    try_table=operatTable::loadFile("student table");

    //换索引
    //index* ni=new binarySearchIndex(ID);
    //try_table->changeIndex(0,ni);

    try_table->open_trasaction();
    try_table->add({typeHelper::strToBasic("789",INT),typeHelper::strToBasic("99.9",FLOAT),typeHelper::strToBasic("88.8",FLOAT)});
    result=try_table->find({new ruleExp(EQU,new Int(789)),nullptr,nullptr});
    cout<<try_table->genNewTable({0,1,2},result)->toStr();
    try_table->rollback_trasaction();

    try_table->saveFile();
    try_table->del(0);
    try_table->del(4);
    try_table->del(4);
    try_table->del(5);
    try_table->del(5);
    try_table->updateFile();

    delete r;
    delete r2;
    delete r3;
    return a.exec();
}
