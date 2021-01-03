#include <QCoreApplication>
#include <queue>
#include <thread>
#include "aggHelper.h"
#include "TcpSocketServer.h"
#include "tableManager.h"
#include "dbProcess.h"
#include "view.h"
string table::default_path;
string tableManager::curOperatUser;
queue<processObject> dbProcess::processQueue;
queue<processObject> dbProcess::correspondQueue;
tableManager* tableManager::tablemanager;
table* dbProcess::countTable;
table* tableManager::jurisdictionTable;
aggHelper* aggHelper::helper;
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

    //RegisterJSType(tablePtr*,"tablePtr*");
    RegisterJSType(table*,"table*");
    RegisterJSType(string,"string");

    table::default_path="D:\\personal_file\\download_files\\test_";

    tableManager::tablemanager=tableManager::getTableManager(1);
    aggHelper::helper=aggHelper::getHelper();

    dbProcess::setCount(table::loadFile("Count"));
    tableManager::setJurisdiction(table::loadFile("Jurisdiction"));

    tableManager::tablemanager->loadTable("Count");
    tableManager::tablemanager->loadTable("Jurisdiction");
    tableManager::tablemanager->doManage();
    tableManager* check=tableManager::tablemanager;
    TcpSocketServer *m_pTcpServer=new TcpSocketServer();
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }
    dbRun* dbrun=new dbRun();
    dbrun->start();
    return a.exec();
};

