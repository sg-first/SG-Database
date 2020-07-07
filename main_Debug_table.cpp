#include <QCoreApplication>
#include "table.h"
#include <iostream>
#include <string>
#include "basicType.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    col* ID=new col(TYPE(1),"ID");
    for(int i=0;i<10;i++){
        ID->pushDate(typeHelper::strToBasic(to_string(i),INT));
    }
    col* score=new col(TYPE(0),"SCORE");
    for(float i=0;i<1;i=i+float(0.1)){
        score->pushDate(typeHelper::strToBasic(to_string(i),FLOAT));
    }
    col* score_2=new col(TYPE(0),"SCORE");
    for(float i=1;i<2;i=i+float(0.1)){
        score_2->pushDate(typeHelper::strToBasic(to_string(i),FLOAT));
    }
    table* try_table = new table("student table",{ID,score,score_2});
    //try_table=table::loadFile("F:\\predict_2.1_1.csv");
    try_table->saveFile("F:\\predict_2.1_1.csv");
    //try_table->add({typeHelper::strToBasic("789",INT),typeHelper::strToBasic("99.9",FLOAT),typeHelper::strToBasic("88.8",FLOAT)});
    try_table->del(0);
    try_table->del(4);
    try_table->del(4);
    try_table->del(5);
    try_table->del(5);
    try_table->updateFile("F:\\predict_2.1_1.csv");
    return a.exec();
}