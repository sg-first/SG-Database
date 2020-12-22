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
{
    QCoreApplication a(argc, argv);
    operatTable::default_path="D:\\personal_file\\download_files\\test_";
    tableManager manager=tableManager();
    dbProcess::setCount(operatTable::loadFile("Count"));
    dbProcess::setJurisdiction(operatTable::loadFile("jurisdiction"));
    shared_ptr<operatTable> st= manager.loadTable("student table");
    index* ni=new binarySearchIndex(st->getCol("ID"));
    st->changeIndex(0,ni);
    vector<int>res_vec=st->find({"(x<789)","",""});
    thread serverThread(testTcpSocketServer);
    serverThread.join();
    thread dbThread(dbRunFunc);
    dbThread.join();
    return a.exec();
}
