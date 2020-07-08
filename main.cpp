#include <QCoreApplication>
#include <iostream>
#include "table.hpp"

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

    //构造查找条件
    ruleExp* r=new ruleExp(EQU,new Int(3));
    ruleExp* unr=new ruleExp;
    auto result=try_table->find({r,unr,unr});
    cout<<result[0]<<endl;

    //换索引
    index* ni=new BPlusTreeIndex(ID);
    try_table->changeIndex(0,ni);
    result=try_table->find({r,unr,unr});
    cout<<result[0]<<endl;

    delete r;
    delete unr;

    return a.exec();
}
