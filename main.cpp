#include <QCoreApplication>
#include <queue>
#include <thread>
#include "aggHelper.h"
#include "TcpSocketServer.h"
#include "tableManager.h"
#include "dbProcess.h"
#include "view.h"
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
class dbRun :public QThread{
virtual void run(){
    while(true){
        dbProcess::processRequst();
    }
}
};
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    operatTable::default_path="D:\\personal_file\\download_files\\test_";
    tableManager tablemanager=tableManager();
    //viewManager viewmanager=viewManager();
    dbProcess::setCount(operatTable::loadFile("Count"));
    dbProcess::setJurisdiction(operatTable::loadFile("Jurisdiction"));
    TcpSocketServer *m_pTcpServer=new TcpSocketServer();
    //2. 启动服务端
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }
    dbRun* dbrun=new dbRun();
    dbrun->start();
    return a.exec();
};
