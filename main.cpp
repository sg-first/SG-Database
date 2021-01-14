#include <QCoreApplication>
#include <queue>
#include <thread>
#include "aggHelper.h"
#include "TcpSocketServer.h"
#include "tableManager.h"
#include "dbProcess.h"
#include "view.h"
#include "jsCall.cpp"
string table::default_path;
string tableManager::curOperatUser;
queue<processObject> dbProcess::processQueue;
queue<processObject> dbProcess::correspondQueue;
tableManager* tableManager::tablemanager;
view* view::viewmanager;
table* dbProcess::countTable;
table* tableManager::jurisdictionTable;
aggHelper* aggHelper::agghelper;
typeHelper* typeHelper::typehelper;
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

    RegisterJSType(table,"table");
    RegisterJSType(table*,"table*");
    RegisterJSType(col*,"col*");
    RegisterJSType(Basic*,"Basic*");
    RegisterJSType(jsCollection*,"jsCollection*");

    table::default_path="D:\\personal_file\\download_files\\test_";

    manageContain::contain->init();
    typeHelper::typehelper=typeHelper::getTypeHelper();
    tableManager::tablemanager=tableManager::getTableManager(10);
    aggHelper::agghelper=aggHelper::getHelper();
    view::viewmanager=view::getViewManager();


    dbProcess::setCount(table::loadFile("Count"));
    tableManager::setJurisdiction(table::loadFile("Jurisdiction"));

    TcpSocketServer *m_pTcpServer=new TcpSocketServer();
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }
    dbRun* dbrun=new dbRun();
    dbrun->start();

/*
    //创建表 增加数据
    col* NAME=new col(STR,"NAME");
    col* SCORE=new col (INT,"SCORE");
    col* TEACHER=new col (STR,"TEACHER");
    table* student=new table ("student",{NAME,SCORE,TEACHER});
    student->add({typeHelper::strToBasic("'zt'"),typeHelper::strToBasic(("57")),typeHelper::strToBasic("'jp'")});
    student->add({typeHelper::strToBasic("'gj'"),typeHelper::strToBasic(("71")),typeHelper::strToBasic("'gb'")});
    student->add({typeHelper::strToBasic("'yf'"),typeHelper::strToBasic(("78")),typeHelper::strToBasic("'gjb'")});
    student->add({typeHelper::strToBasic("'xr'"),typeHelper::strToBasic(("89")),typeHelper::strToBasic("'gjb'")});
    student->add({typeHelper::strToBasic("'wx'"),typeHelper::strToBasic(("63")),typeHelper::strToBasic("'jp'")});
    student->add({typeHelper::strToBasic("'ww'"),typeHelper::strToBasic(("78")),typeHelper::strToBasic("'gb'")});
    student->saveFile();

    col* tName=new col(STR,"TEACHER");
    col* loc=new col (STR,"LOC");
    table* teacher=new table ("teacher",{tName,loc});
    teacher->add({typeHelper::strToBasic("'jp'"),typeHelper::strToBasic(("'HangZhou'"))});
    teacher->add({typeHelper::strToBasic("'gb'"),typeHelper::strToBasic(("'BeiJing'"))});
    teacher->add({typeHelper::strToBasic("'gjb'"),typeHelper::strToBasic(("'ShangHai'"))});
    teacher->saveFile();

    //对表查询
    //select max（student.SCORE）from student where student.NAME!="zt"
    table* stu_1=tableManager::tablemanager->loadTable("student");
    vector<int> resIndex=stu_1->find({"(x!='zt')","",""});
    vector<Basic*> resBasic=stu_1->getCol("SCORE")->getData(resIndex);
    cout<<aggHelper::helper->max(resBasic)->toStr()<<endl;


    //select teacher.loc from teacher where teacher.tName==(select max(student.SCORE) from student )
    table* stu_2=tableManager::tablemanager->loadTable("student");
    table* tch=tableManager::tablemanager->loadTable("teacher");
    string maxScore=aggHelper::helper->max(stu_2->getCol("SCORE")->getAllData())->toStr();
    vector<int> getData=stu_2->find({"","(x=="+maxScore+")",""});
    vector<Basic*> tchs=stu_2->getCol("TEACHER")->getData(stu_2->find({"","(x=="+maxScore+")",""}));
    cout<<tch->getCol("LOC")->genNewCol(tch->find_in("TEACHER",tchs))->toStr()<<endl;


    //定点修改
    stu_1->del(stu_1->find({"(((x=='xr')||(x=='wx'))||(x=='ww'))","",""}));
    stu_1->updateFile();
    stu_1=table::loadFile("student");
    stu_1->saveFile();



    //更换索引
    index* ni=new binarySearchIndex(SCORE);
    stu_1->changeIndex(1,ni);
    cout<<stu_1->genNewTable({"NAME","SCORE","TEACHER"},stu_1->find({"","(x<71)",""}))->toStr()<<endl;





    //事务功能
    cout<<stu_1->toStr()<<endl;
    stu_1->updateFile();
    stu_1->add({typeHelper::strToBasic("'lz'"),typeHelper::strToBasic(("54")),typeHelper::strToBasic("'cnnndsb'")});
    cout<<stu_1->toStr()<<endl;
    stu_1->rollback();
    cout<<stu_1->toStr()<<endl;

    manageContain::contain->reset();

    col* USERNAME=new col(STR,"USERNAME");
    col* PASSWORD=new col (STR,"PASSWORD");
    table* Count=new table ("Count",{USERNAME,PASSWORD});
    Count->add({typeHelper::typehelper->strToBasic("'ZT'"),typeHelper::typehelper->strToBasic(("'zt'"))});
    Count->saveFile();

    col* USER=new col(STR,"USERNAME");
    col* TABLE=new col (STR,"TABLE");
    table* Jurisdiction=new table ("Jurisdiction",{USER,TABLE});
    Jurisdiction->add({typeHelper::typehelper->strToBasic("'ZT'"),typeHelper::typehelper->strToBasic(("'student'"))});
    Jurisdiction->add({typeHelper::typehelper->strToBasic("'ZT'"),typeHelper::typehelper->strToBasic(("'teacher'"))});
    Jurisdiction->saveFile();

    col* viewName=new col(STR,"viewName");
    col* viewCode=new col (STR,"viewCode");
    table* views=new table ("views",{viewName,viewCode});
    views->saveFile();

*/




    return a.exec();
};

