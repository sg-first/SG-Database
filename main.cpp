#include <QCoreApplication>
#include <queue>
#include <thread>
#include "aggHelper.h"
#include "TcpSocketServer.h"
#include "tableManager.h"
#include "dbProcess.h"
string operatTable::default_path;
queue<processObject> dbProcess::processQueue;
queue<processObject> dbProcess::correspondQueue;
shared_ptr<operatTable> dbProcess::countTable;
shared_ptr<operatTable> dbProcess::jurisdictionTable;
aggHelper* aggHelper::helper;
using namespace std;
void outputVec(const vector<int> &vec)
{
    for(int i : vec)
        cout<<i<<" ";
    cout<<endl;
}
void dbRunFunc(){
    while(true){
        dbProcess::processRequst();
    }
}
int main(int argc, char *argv[])
{long start_time=time(0);
    QCoreApplication a(argc, argv);
    operatTable::default_path="D:\\personal_file\\download_files\\test_";
    shared_ptr<col> ID(new col(TYPE(1),"ID"));
    for(int i=0;i<10;i++){
        ID->pushData(typeHelper::strToBasic(to_string(i),INT));
    }
    shared_ptr<col> score(new col(TYPE(0),"SCORE"));
    for(float i=0;i<1;i=i+float(0.1)){
        score->pushData(typeHelper::strToBasic(to_string(i),FLOAT));
    }
    shared_ptr<col> score_2(new col(TYPE(0),"SCORE"));
    for(float i=1;i<2;i=i+float(0.1)){
        score_2->pushData(typeHelper::strToBasic(to_string(i),FLOAT));
    }
    shared_ptr<operatTable> try_table(new operatTable("student table",{ID,score,score_2}));
    //try_table=table::loadFile("F:\\predict_2.1_1.csv");







    vector<int> result=try_table->find({"((((x<=3)||(x==4))||(x>=7))||(x==5))","",""});
    outputVec(result);
    result=try_table->find({"(x!=7)","",""});
    result=try_table->find({"!((((x<=3)||(x==4))||(x>=7))||(x==5))","",""});
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
    ruleExp* rule=new ruleExp(EQU,typeHelper::strToBasic("789"));
    result=try_table->find({"(x==789)","",""});
    cout<<try_table->genNewTable({0,1,2},result)->toStr();
    try_table->updateFile();

    try_table->mod(0,{typeHelper::strToBasic("789",INT),typeHelper::strToBasic("99.9",FLOAT),typeHelper::strToBasic("88.8",FLOAT)});

    //result=try_table->find({nullptr,nullptr,nullptr});
    vector<shared_ptr<Basic>> s_result=try_table->getCol("ID")->getData(result);


    aggHelper* helper=aggHelper::getHelper();
    shared_ptr<Basic> test=helper->avg(s_result);

    try_table->saveFile();
    try_table->del(0);
    try_table->del(4);
    try_table->del(4);
    try_table->del(5);
    try_table->del(5);
    try_table->updateFile();

    result=try_table->find({new ruleExp(EQU,typeHelper::strToBasic("789")),nullptr,nullptr});
    cout<<try_table->genNewTable({0,1,2},result)->toStr();

    try_table=operatTable::loadFile("student table");


    result=try_table->find({new ruleExp(EQU,typeHelper::strToBasic("789")),nullptr,nullptr});
    cout<<try_table->genNewTable({0,1,2},result)->toStr();
    try_table->saveFile();
    cout<<"Total running time:"<<time(0)-start_time;
    /*QCoreApplication a(argc, argv);
    operatTable::default_path="D:\\personal_file\\download_files\\test_";
    tableManager manager=tableManager();
    dbProcess::setCount(operatTable::loadFile("Count"));
    dbProcess::setJurisdiction(operatTable::loadFile("jurisdiction"));
    thread serverThread(testTcpSocketServer);
    serverThread.join();
    thread dbThread(dbRunFunc);
    dbThread.join();*/
    return a.exec();
}
