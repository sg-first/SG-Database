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
manageContain* manageContain::contain;
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

    RegisterJSType(table*,"table*");
    RegisterJSType(col*,"col*");
    RegisterJSType(Basic*,"Basic*");
    RegisterJSType(vector<int>,"vector<int>");
    RegisterJSType(vector<Basic*>,"vector<Basic*>");
    RegisterJSType(vector<QString>,"vector<QString>");
    RegisterJSType(QString[],"QString[]");

    table::default_path="D:\\personal_file\\download_files\\test_";

    manageContain::contain->init();

    tableManager::tablemanager=tableManager::getTableManager(1);
    aggHelper::helper=aggHelper::getHelper();

    dbProcess::setCount(table::loadFile("Count"));
    tableManager::setJurisdiction(table::loadFile("Jurisdiction"));

    TcpSocketServer *m_pTcpServer=new TcpSocketServer();
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }
    dbRun* dbrun=new dbRun();
    dbrun->start();
    return a.exec();
};

