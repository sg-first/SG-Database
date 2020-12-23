#include <QCoreApplication>
#include <queue>
#include <thread>
#include "aggHelper.h"
#include "TcpSocketServer.h"
#include "tableManager.h"
#include "dbProcess.h"
#include "view.hpp"
string operatTable::default_path;
string dbProcess::curOperatUser;
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
    tableManager tablemanager=tableManager();
    viewManager viewmanager=viewManager();
    dbProcess::setCount(operatTable::loadFile("Count"));
    dbProcess::setJurisdiction(operatTable::loadFile("Jurisdiction"));
    thread serverThread(testTcpSocketServer);
    serverThread.join();
    thread dbThread(dbRunFunc);
    dbThread.join();
    return a.exec();
}
