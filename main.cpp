#include <QCoreApplication>
#include <queue>
#include <thread>
#include "TcpSocketServer.h"
#include "tableManager.h"
#include "dbProcess.h"
string operatTable::default_path;
queue<processObject> dbProcess::processQueue;
queue<processObject> dbProcess::correspondQueue;
operatTable* dbProcess::countTable;
operatTable* dbProcess::jurisdictionTable;
using namespace std;
void dbRunFunc(){
    while(true){
        dbProcess::processRequst();
    }
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    operatTable::default_path="D:\\personal_file\\download_files\\test_";
    /*col* userName=new col (STR,"userName") ;
    col* passWord=new col(STR,"passWord");
    operatTable* count=new operatTable("Count",{userName,passWord});
    count->add({typeHelper::strToBasic("'ZT'"),typeHelper::strToBasic("'zt'")});
    count->add({typeHelper::strToBasic("'LTZ'"),typeHelper::strToBasic("'ltz'")});
    count->add({typeHelper::strToBasic("'GJ'"),typeHelper::strToBasic("'gj'")});
    count->add({typeHelper::strToBasic("'JYF'"),typeHelper::strToBasic("'jyf'")});
    count->saveFile();

    col* user=new col (STR,"user") ;
    col* table=new col(STR,"table");
    operatTable* jurisdiction=new operatTable ("jurisdiction",{user,table});
    jurisdiction->add({typeHelper::strToBasic("'ZT'"),typeHelper::strToBasic("'student table'")});
    jurisdiction->add({typeHelper::strToBasic("'LTZ'"),typeHelper::strToBasic("'student table'")});
    jurisdiction->add({typeHelper::strToBasic("'GJ'"),typeHelper::strToBasic("'student table'")});
    jurisdiction->add({typeHelper::strToBasic("'JYF'"),typeHelper::strToBasic("'student table'")});
    jurisdiction->saveFile();
    */
    tableManager manager(1);
    operatTable* jur= manager.loadTable("jurisdiction");
    operatTable* count=manager.loadTable("Count");
    jur->updateFile();
    jur->saveFile();
    manager.doManage();
    thread serverThread(testTcpSocketServer);
    serverThread.join();
    thread dbThread(dbRunFunc);
    dbThread.join();
    return a.exec();
}
