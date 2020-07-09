#include <QCoreApplication>
#include <iostream>
#include "table.hpp"

void outputVec(const vector<int> &vec)
{
    for(int i : vec)
        cout<<i<<" ";
    cout<<endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //建表
    col* ID=new col(INT,"ID");
    for(int i=0;i<10;i++)
        ID->pushDate(new Int(i));

    col* score=new col(FLOAT,"SCORE");
    for(float i=0;i<1;i=i+float(0.1))
        score->pushDate(new Float(i));

    col* score_2=new col(FLOAT,"SCORE");
    for(float i=1;i<2;i=i+float(0.1))
        score_2->pushDate(new Float(i));

    table* try_table = new table("student table",{ID,score,score_2});
    cout<<try_table->toStr()<<endl;

    //构造查找条件
    ruleExp* r=new ruleExp(EQU,new Int(3));
    auto result=try_table->find({r,nullptr,nullptr});
    outputVec(result);
    ruleExp* r2=new numExp(GRAT,new Int(3));
    result=try_table->find({r2,nullptr,nullptr});
    outputVec(result);

    //表抽取
    auto t2=try_table->genNewTable(helper::getRange(0,2),result);
    cout<<t2->toStr()<<endl;

    //换索引
    index* ni=new BPlusTreeIndex(ID);
    try_table->changeIndex(0,ni);
    result=try_table->find({r,nullptr,nullptr});
    cout<<result[0]<<endl;

    delete r;
    delete r2;

    return a.exec();
}
