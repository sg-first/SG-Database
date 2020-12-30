#include <QCoreApplication>
#include <queue>
#include <thread>
#include "aggHelper.h"
#include "TcpSocketServer.h"
#include "tableManager.h"
#include "dbProcess.h"
#include "view.h"
string operatTable::default_path;
string tableManager::curOperatUser;
queue<processObject> dbProcess::processQueue;
queue<processObject> dbProcess::correspondQueue;
shared_ptr<tableManager> tableManager::tablemanager;
shared_ptr<operatTable> dbProcess::countTable;
shared_ptr<operatTable> tableManager::jurisdictionTable;
shared_ptr<aggHelper> aggHelper::helper;
#define RegisterJSType(TypeName,JSName) qRegisterMetaType<TypeName>(JSName)
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

    RegisterJSType(shared_ptr<operatTable>,"shared_ptr<operatTable>");
    RegisterJSType(string,"string");

    operatTable::default_path="D:\\personal_file\\download_files\\test_";

    tableManager::tablemanager=tableManager::getTableManager();
    aggHelper::helper=aggHelper::getHelper();

    dbProcess::setCount(operatTable::loadFile("Count"));
    tableManager::setJurisdiction(operatTable::loadFile("Jurisdiction"));

    TcpSocketServer *m_pTcpServer=new TcpSocketServer();
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }

    while(true){
        dbProcess::processRequst();
    }
    return a.exec();
};

