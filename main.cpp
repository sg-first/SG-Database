#include <QtCore/QCoreApplication>
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
int IO::singleFileLen;
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

    IO::singleFileLen=1000;

    RegisterJSType(table*,"table*");
    RegisterJSType(col*,"col*");
    RegisterJSType(Basic*,"Basic*");
    RegisterJSType(jsCollection*,"jsCollection*");
    RegisterJSType(processObject,"processObject");

    table::default_path="D:\\personal_file\\download_files\\test_";

    typeHelper::typehelper=typeHelper::getTypeHelper();
    manageContain::contain->init();
    tableManager::tablemanager=tableManager::getTableManager(10);
    aggHelper::agghelper=aggHelper::getHelper();
    view::viewmanager=view::getViewManager();


    dbProcess::setCount(table::loadFile("Count"));
    tableManager::setJurisdiction(table::loadFile("Jurisdiction"));



    table* student=tableManager::tablemanager->loadTable("student");
    student->add({typeHelper::typehelper->strToBasic("'wuwei'"),typeHelper::typehelper->strToBasic(("63")),typeHelper::typehelper->strToBasic("'sbjp'")});
    student->add({typeHelper::typehelper->strToBasic("'wuweibb'"),typeHelper::typehelper->strToBasic(("63")),typeHelper::typehelper->strToBasic("'sbjp'")});

    table* teacher=tableManager::tablemanager->loadTable("teacher");
    teacher->add({typeHelper::typehelper->strToBasic("'jpsb'"),typeHelper::typehelper->strToBasic("'YANGZHOU'")});

    vector<string> stuVec={"NAME","SCORE","TEACHER"};
    vector<string> teaVec={"TEACHER","LOC"};

    cout<<tableManager::tablemanager->tableJoin("tsleft","student",stuVec,"teacher",teaVec,"TEACHER","TEACHER","leftjoin")->toStr().toStdString();
    cout<<tableManager::tablemanager->tableJoin("tsouter","student",stuVec,"teacher",teaVec,"TEACHER","TEACHER","outerjoin")->toStr().toStdString();












/*
    long long startTime=getCurrentTime();
    student=tableManager::tablemanager->loadTable("student");
    cout<<getCurrentTime()-startTime<<endl;

    startTime=getCurrentTime();
    for(int i=0;i<10000;++i){
        student->add({typeHelper::typehelper->strToBasic("'xr'"),typeHelper::typehelper->strToBasic((QString::fromStdString(to_string(i)))),typeHelper::typehelper->strToBasic("'jp'")});
    }
    cout<<getCurrentTime()-startTime<<endl;

    startTime=getCurrentTime();
    resVec=student->find({"(x=='zt')","",""});
    cout<<getCurrentTime()-startTime<<endl;

    startTime=getCurrentTime();
    student->del(resVec);
    cout<<getCurrentTime()-startTime<<endl;

    startTime=getCurrentTime();
    student->updateFile();
    cout<<getCurrentTime()-startTime<<endl;*/

    TcpSocketServer *m_pTcpServer=new TcpSocketServer();
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
    }
    dbRun* dbrun=new dbRun();
    dbrun->start();
    ServerResponseThread RT;
    QObject::connect(&RT, SIGNAL(db_response_signal(processObject)), m_pTcpServer, SLOT(response_handle(processObject)));
    RT.start();


/*
    多表连接
    table* student=tableManager::tablemanager->loadTable("student");
    student->add({typeHelper::typehelper->strToBasic("'wuwei'"),typeHelper::typehelper->strToBasic(("63")),typeHelper::typehelper->strToBasic("'sbjp'")});
    student->add({typeHelper::typehelper->strToBasic("'wuweibb'"),typeHelper::typehelper->strToBasic(("63")),typeHelper::typehelper->strToBasic("'sbjp'")});

    table* teacher=tableManager::tablemanager->loadTable("teacher");
    teacher->add({typeHelper::typehelper->strToBasic("'jpsb'"),typeHelper::typehelper->strToBasic("'YANGZHOU'")});

    vector<string> stuVec={"NAME","SCORE","TEACHER"};
    vector<string> teaVec={"TEACHER","LOC"};

    cout<<tableManager::tablemanager->tableJoin("tsleft","student",stuVec,"teacher",teaVec,"TEACHER","TEACHER","leftjoin")->toStr().toStdString();
    cout<<tableManager::tablemanager->tableJoin("tsouter","teacher",teaVec,"student",stuVec,"TEACHER","TEACHER","outerjoin")->toStr().toStdString();*/


    /*
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
    views->saveFile();*/






//    //开启C++11数据库响应监听线程函数
//    std::thread t1(db_listener_thread_function);


/*
    //创建表 增加数据
    col* NAME=new col(STR,"NAME");
    col* SCORE=new col (INT,"SCORE");
    col* TEACHER=new col (STR,"TEACHER");
    table* student=new table ("student",{NAME,SCORE,TEACHER});
    student->add({typeHelper::typehelper->strToBasic("'zt'"),typeHelper::typehelper->strToBasic(("57")),typeHelper::typehelper->strToBasic("'jp'")});
    student->add({typeHelper::typehelper->strToBasic("'gj'"),typeHelper::typehelper->strToBasic(("71")),typeHelper::typehelper->strToBasic("'gb'")});
    student->add({typeHelper::typehelper->strToBasic("'yf'"),typeHelper::typehelper->strToBasic(("78")),typeHelper::typehelper->strToBasic("'gjb'")});
    student->add({typeHelper::typehelper->strToBasic("'xr'"),typeHelper::typehelper->strToBasic(("89")),typeHelper::typehelper->strToBasic("'gjb'")});
    student->add({typeHelper::typehelper->strToBasic("'wx'"),typeHelper::typehelper->strToBasic(("63")),typeHelper::typehelper->strToBasic("'jp'")});
    student->add({typeHelper::typehelper->strToBasic("'ww'"),typeHelper::typehelper->strToBasic(("78")),typeHelper::typehelper->strToBasic("'gb'")});
    student->saveFile();

    col* tName=new col(STR,"TEACHER");
    col* loc=new col (STR,"LOC");
    table* teacher=new table ("teacher",{tName,loc});
    teacher->add({typeHelper::typehelper->strToBasic("'jp'"),typeHelper::typehelper->strToBasic(("'HangZhou'"))});
    teacher->add({typeHelper::typehelper->strToBasic("'gb'"),typeHelper::typehelper->strToBasic(("'BeiJing'"))});
    teacher->add({typeHelper::typehelper->strToBasic("'gjb'"),typeHelper::typehelper->strToBasic(("'ShangHai'"))});
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
    stu_1->add({typeHelper::typehelper->strToBasic("'lz'"),typeHelper::typehelper->strToBasic(("54")),typeHelper::typehelper->strToBasic("'cnnndsb'")});
    cout<<stu_1->toStr()<<endl;
    stu_1->rollback();
    cout<<stu_1->toStr()<<endl;


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

